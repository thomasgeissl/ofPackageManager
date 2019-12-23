#include "ofApp.h"
#include "ofxCommandLineUtils.h"
#define IFNOTSILENT(do) ({if(!_silent){do;} })
#define IFSILENT(do) ({if(_silent){do;} })
namespace fs = std::filesystem;

ofPackageManager::ofPackageManager(std::string cwdPath) : _cwdPath(cwdPath),
														  _configJson(getConfig()),
														  _silent(false)
{
	// ofSetLogLevel(OF_LOG_VERBOSE);
}

bool ofPackageManager::addPackageToAddonsMakeFile(ofPackage package)
{
	if (package.isEmpty())
	{
		return false;
	}
	ofFile addonsMakeFile(getAbsolutePath("addons.make"), ofFile::ReadOnly);
	if (!addonsMakeFile.exists())
	{
		addonsMakeFile.create();
	}
	auto stringToAdd = package._path + " #" + package._url + "@" + package._checkout;
	ofBuffer fileBuffer = addonsMakeFile.readToBuffer();
	std::string content;
	auto foundPackage = false;
	for (auto line : fileBuffer.getLines())
	{
		auto words = ofSplitString(ofTrim(line), "#");
		// if path addon already listed in addon.make
		// then update it
		// else append a new line
		if (ofTrim(words[0]) == package._path)
		{
			foundPackage = true;
			content += stringToAdd;
			content += "\n";
		}
		else
		{
			content += line + "\n";
		}
	}
	if (!foundPackage)
	{
		content += stringToAdd;
	}
	fileBuffer.set(content.c_str(), content.size());
	addonsMakeFile.close();
	ofFile newAddonsMakeFile(getAbsolutePath("addons.make"), ofFile::ReadWrite);
	if (newAddonsMakeFile.writeFromBuffer(fileBuffer))
	{
		IFNOTSILENT(ofLogNotice("ofPackageManager") << "successfully updated addons.make";);
	}
	else
	{
		IFNOTSILENT(ofLogError("ofPackageManager") << "Could not update addons.make";);
	}
	return true;
}

bool ofPackageManager::addPackageToAddonsMakeFile(std::string path)
{
	ofxGit::repository repo(ofFilePath::join(_cwdPath, path));
	if (repo.isRepository())
	{
		auto url = repo.getRemoteUrl();
		auto checkout = repo.getCommitHash();
		return addPackageToAddonsMakeFile(ofPackage(path, url, checkout));
	}
	else
	{
		ofDirectory directory(ofFilePath::join(_cwdPath, path));
		if (directory.exists()) // && directory.size() > 0
		{
			if (getBoolAnswer(path + " is not a git repository, but a non-empty directory. Do you want to add all its children?"))
			{
				return addPackagesToAddonsMakeFile(path);
			}
		}
	}
	return false;
}

bool ofPackageManager::addPackagesToAddonsMakeFile(std::string path)
{
	ofDirectory directory(ofFilePath::join(_cwdPath, path));
	if (!directory.exists())
	{
		IFNOTSILENT(ofLogError() << "directory does not exit";);
		return false;
	}
	directory.listDir();
	auto result = true;
	for (auto file : directory.getFiles())
	{
		if (file.isDirectory())
		{
			if (!addPackageToAddonsMakeFile(ofFilePath::join(path, file.getFileName())))
			{
				result = false;
			}
		}
	}
	return result;
}
bool ofPackageManager::addPackagesToAddonsMakeFile(std::vector<std::string> paths)
{
	auto result = true;
	for (auto path : paths)
	{
		if (!addPackageToAddonsMakeFile(ofFilePath::join(_cwdPath, path)))
		{
			result = false;
		}
	}
	return result;
}
void ofPackageManager::configure(bool global)
{
	auto configPath = ofFilePath::join(_cwdPath, "ofPackageManager.json");
	std::string relativeOrAbsolute = "relative";
	if (global)
	{
		configPath = ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json");
		relativeOrAbsolute = "absolute";
	}
	ofFile configFile(configPath);
	if (configFile.exists())
	{
		if (!_silent)
		{
			ofLogWarning("config") << "Config file already exits.";
			if (!getBoolAnswer("Do you want to override it?"))
			{
				return;
			}
		}
	}

	ofJson configJson;
	configJson["ofPath"] = getStringAnswer("Absolute path to openFrameworks?", ofFilePath::getAbsolutePath(getAbsolutePath("../../.."), false));
	auto packagesPath = getStringAnswer("Absolute path to packages directory?", ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackages"));
	configJson["packagesPath"] = packagesPath;
	configJson["localAddonsPath"] = getStringAnswer("local addons directory?", "local_addons");
	configJson["pgPath"] = getStringAnswer("Absolute path to the projet generator?", ofFilePath::join(configJson["ofPath"].get<std::string>(), "projectGenerator-osx"));

	configFile.create();
	configFile.open(configPath, ofFile::WriteOnly);
	configFile << configJson.dump(4);
	configFile.close();

	if (ofDirectory::doesDirectoryExist(packagesPath, false))
	{
		IFNOTSILENT(ofLogError("config") << "The packages database exits already. Please update manually (cd " + packagesPath + " && git pull).";);
	}
	else
	{
		ofxGit::repository repo(packagesPath);
		if (repo.clone("https://github.com/thomasgeissl/ofPackages.git"))
		{
			IFNOTSILENT(ofLogNotice("config") << "Successfully cloned packages database";);
		}
		else
		{
			IFNOTSILENT(ofLogError("config") << "Could not clone packages database";);
		}
	}
}

bool ofPackageManager::isNewerVersionAvailable()
{
	ofHttpResponse request = ofLoadURL("https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/defines.h");
	auto defines = request.data.getText();
	auto currentVersion = getVersion();
	auto mostRecentVersion = currentVersion;
	auto lines = ofSplitString(defines, "\n");
	for (auto line : lines)
	{
		auto parts = ofSplitString(line, " ");
		if (parts.size() == 3)
		{
			if (parts[1] == "major")
			{
				mostRecentVersion._major = ofToInt(parts[2]);
			}
			else if (parts[1] == "minor")
			{
				mostRecentVersion._minor = ofToInt(parts[2]);
			}
			else if (parts[1] == "patch")
			{
				mostRecentVersion._patch = ofToInt(parts[2]);
			}
		}
	}

	// TODO: move as an operator to version class
	return (
		mostRecentVersion._major > currentVersion._major ||
		mostRecentVersion._minor > currentVersion._minor ||
		mostRecentVersion._patch > currentVersion._patch);
}

void ofPackageManager::generateDatabaseEntryFile()
{
	ofJson dataBaseEntryJson;

	dataBaseEntryJson["name"] = getStringAnswer("package name?");
	dataBaseEntryJson["author"] = getStringAnswer("author?");
	dataBaseEntryJson["url"] = getStringAnswer("url?");
	dataBaseEntryJson["cloneUrl"] = getStringAnswer("cloneUrl?");
	dataBaseEntryJson["license"] = getStringAnswer("license?");
	dataBaseEntryJson["type"] = getOptionAnswer("type", {"app", "addon"});

	std::string name = dataBaseEntryJson["name"];
	ofFile dataBaseEntryFile(ofFilePath::addTrailingSlash(getAbsolutePath(_cwdPath)) + name + ".json", ofFile::ReadWrite);
	if (!dataBaseEntryFile.exists())
	{
		dataBaseEntryFile.create();
	}
	dataBaseEntryFile << dataBaseEntryJson.dump(4);
}

ofJson ofPackageManager::searchPackageOnGithubByName(string name)
{
	std::string url = "https://api.github.com/search/repositories?q=" + name;
	ofHttpRequest request(url, "TODO: check name");
	request.headers["User-Agent"] = "ofPackageManager";
	ofURLFileLoader loader;
	auto response = loader.handleRequest(request);
	auto resultJson = ofJson::parse(response.data.getText());
	std::string outputString;
	outputString += "The following repositories contain " + name + ":\n";
	auto counter = 0;
	for (auto repo : resultJson["items"])
	{
		std::string name = repo["full_name"];
		std::string url = repo["html_url"];
		std::string updatedAt = repo["updated_at"];
		int stars = repo["stargazers_count"];
		int forks = repo["forks_count"];
		std::string isFork = repo["fork"].get<bool>() ? "true" : "false";
		int openIssues = repo["open_issues_count"];
		outputString += ofToString(counter++) + ": " + name;
		outputString += "\n\t";
		outputString += url;
		outputString += "\n\t";
		outputString += "stars: " + ofToString(stars) + ", open issues: " + ofToString(openIssues) + ", updated at: " + updatedAt + ", forks: " + ofToString(forks) + ", isFork: " + isFork;
		outputString += "\n\n";
	}
	IFNOTSILENT(
		ofLogNotice("search") << outputString;);

	return resultJson;
}

ofJson ofPackageManager::searchPackageOnGithubByUser(std::string user)
{
	std::string url = "https://api.github.com/users/" + user + "/repos?per_page=100";
	ofHttpRequest request(url, "TODO: check name");
	request.headers["User-Agent"] = "ofPackageManager";
	ofURLFileLoader loader;
	auto response = loader.handleRequest(request);
	auto resultJson = ofJson::parse(response.data.getText());
	std::string outputString;
	outputString += "These are the first 100 repositories by " + user + ":\n";
	auto counter = 0;
	for (auto repo : resultJson)
	{
		std::string repoName = repo["name"];
		std::string name = repo["full_name"];
		std::string url = repo["html_url"];
		std::string updatedAt = repo["updated_at"];
		int stars = repo["stargazers_count"];
		int forks = repo["forks_count"];
		std::string isFork = repo["fork"].get<bool>() ? "true" : "false";
		int openIssues = repo["open_issues_count"];
		if (repoName.substr(0, 3) == "ofx")
		{
			outputString += ofToString(counter++) + ": " + name;
			outputString += "\n\t";
			outputString += url;
			outputString += "\n\t";
			outputString += "stars: " + ofToString(stars) + ", open issues: " + ofToString(openIssues) + ", updated at: " + updatedAt + ", forks: " + ofToString(forks) + ", isFork: " + isFork;
			outputString += "\n\n";
		}
	}
	IFNOTSILENT(
		ofLogNotice("search") << outputString;);
	return resultJson;
}

ofPackage ofPackageManager::installPackageByGithub(std::string github, std::string checkout, std::string destinationPath)
{
	return installPackageByUrl(generateGithubUrl(github), checkout, destinationPath);
}

ofPackage ofPackageManager::installPackageByUrl(std::string url, std::string checkout, std::string destinationPath)
{
	IFNOTSILENT(ofLogNotice("ofPackageManager") << "install package by url (" << url << "@" << checkout << " to " << destinationPath;);
	if (destinationPath.empty())
	{
		destinationPath = _configJson["localAddonsPath"].get<std::string>();
	}
	destinationPath = getAbsolutePath(destinationPath);
	auto name = ofSplitString(url, "/").back();
	std::string suffix = ".git";
	if (0 == name.compare(name.size() - suffix.size(), suffix.size(), suffix))
	{
		name = name.substr(0, name.size() - suffix.size());
	}
	else
	{
		url += suffix;
	}

	ofDirectory destinationDirectory(destinationPath);
	auto repoPath = ofFilePath::join(destinationPath, name);
	ofDirectory repoDirectory(repoPath);
	ofxGit::repository repo(ofFilePath::join(destinationPath, name));
	if (destinationDirectory.exists())
	{
		if (repoDirectory.exists())
		{
			IFNOTSILENT(
				if (getBoolAnswer(destinationPath + "/" + name + " already exists. Do you want to pull and checkout the specified commit?", true)) {
					ofLogNotice("TODO") << "Unfortunately it is not yet implemented due to missing ofxGit::repo::pull";
					// TODO: pull and checkout, pull still does not work in ofxGit2
				});
		}
		else
		{
			if (repo.clone(url))
			{
				IFNOTSILENT(ofLogNotice("install") << "Successfully cloned repo " << url;);
			}
			if (checkout != "latest")
			{
				if (!repo.checkout(checkout))
				{
					IFNOTSILENT(ofLogNotice("install") << "could not checkout " << checkout;);
					return ofPackage();
				}
			}
		}
	}
	else
	{
		destinationDirectory.create();
		if (repo.clone(url))
		{
			IFNOTSILENT(ofLogNotice("install") << "Successfully cloned repo " << url;);
		}
		if (checkout != "latest")
		{
			if (!repo.checkout(checkout))
			{
				IFNOTSILENT(ofLogNotice("install") << "could not checkout " << checkout;);
			}
		}
	}

	if (hasAddonConfigFile(ofFilePath::join(destinationPath, name)))
	{
		installDependenciesFromAddonConfig(ofFilePath::join(destinationPath, name), destinationPath);
	}

	return ofPackage(ofFilePath::join(ofFilePath::makeRelative(_cwdPath, destinationPath), name), url, repo.getCommitHash());
}

ofPackage ofPackageManager::maybeInstallOneOfThePackages(ofJson packages, std::string destinationPath = "")
{
	IFSILENT(return ofPackage(););
	if (getBoolAnswer("Do you wanna install any of them?"))
	{
		auto index = getIntAnswer("Which one? Please enter the corresponding number.", 0);
		if (index < packages["items"].size())
		{
			if (destinationPath == "")
			{
				destinationPath = "local_addons";
				if (!getBoolAnswer("Where do you want to install the package. It is recommended to install it locally, are you fine with that? Otherwise it will be installed to your global addons directory"))
				{
					destinationPath = ofFilePath::join(getOfPath(), "addons");
				}
			}
			return installPackageByUrl(packages["items"][index]["clone_url"], "latest", destinationPath);
		}
	}
	return ofPackage();
}

ofJson ofPackageManager::getAvailablePackages()
{
	std::string databasePath = _configJson["packagesPath"];
	ofDirectory ofPackagesDirectory(databasePath);
	ofPackagesDirectory.listDir();

	auto counter = 0;
	ofJson result = ofJson::array();

	for (auto file : ofPackagesDirectory.getFiles())
	{
		if (file.getExtension() == "json")
		{
			ofJson packageJson;
			file.open(file.getAbsolutePath());
			file >> packageJson;
			file.close();

			result[counter] = packageJson;
			counter++;
		}
	}
	return result;
}
std::vector<std::string> ofPackageManager::getCoreAddons()
{
	std::vector<std::string> coreAddons;

	auto globalAddonsDir = ofDirectory(ofFilePath::join(getOfPath(), "addons"));
	globalAddonsDir.listDir();
	for (auto file : globalAddonsDir.getFiles())
	{
		auto path = file.getAbsolutePath();
		if (file.getFileName().find("ofx", 0) == 0 && file.isDirectory())
		{
			ofxGit::repository addon(path);
			if (!addon.isRepository())
			{
				coreAddons.push_back(file.getFileName());
			}
		}
	}

	return coreAddons;
}
std::vector<ofPackage> ofPackageManager::getGloballyInstalledPackages()
{
	std::vector<ofPackage> globallyInstalledPackages;
	auto globalAddonsDir = ofDirectory(ofFilePath::join(getOfPath(), "addons"));
	globalAddonsDir.listDir();
	for (auto file : globalAddonsDir.getFiles())
	{
		auto path = file.getAbsolutePath();
		auto name = file.getFileName();
		if (name.find("ofx", 0) == 0 && !isCoreAddon(name))
		{

			ofxGit::repository repo(path);
			if (repo.isRepository())
			{
				globallyInstalledPackages.push_back(ofPackage(name, repo.getRemoteUrl(), repo.getCommitHash()));
			}
		}
	}
	return globallyInstalledPackages;
}

std::vector<ofPackage> ofPackageManager::getLocallyInstalledPackages()
{
	std::vector<ofPackage> packages;
	auto packagesDir = ofDirectory(getAbsolutePath("local_addons"));
	packagesDir.listDir();
	for (auto file : packagesDir.getFiles())
	{
		auto path = file.getAbsolutePath();
		auto name = file.getFileName();
		if (name.find("ofx", 0) == 0)
		{
			ofxGit::repository repo(path);
			if (repo.isRepository())
			{
				packages.push_back(ofPackage(name, repo.getRemoteUrl(), repo.getCommitHash()));
			}
		}
	}
	return packages;
}

void ofPackageManager::generateProject()
{
	// ofSystem();
	ofLogNotice() << _configJson["pgPath"];
}
ofJson ofPackageManager::searchPackageInDatabaseById(std::string name)
{
	std::string databasePath = _configJson["packagesPath"];
	ofDirectory ofPackagesDirectory(databasePath);
	ofPackagesDirectory.listDir();

	auto counter = 0;
	ofJson result = ofJson::object();
	result["items"] = ofJson::array();
	std::string outputString = "The following packages were found in the database: \n";

	for (auto file : ofPackagesDirectory.getFiles())
	{
		if (file.getExtension() == "json")
		{
			ofJson packageJson;
			file.open(file.getAbsolutePath());
			file >> packageJson;

			std::size_t found = ofToLower(file.getFileName()).find(ofToLower(name));
			if (found != std::string::npos)
			{
				outputString += ofToString(counter) + ": " + packageJson["name"].get<std::string>() + "\n";
				outputString += "\t" + packageJson["website"].get<std::string>() + "\n";
				outputString += "\t" + packageJson["cloneUrl"].get<std::string>() + "\n";
				outputString += "\t" + packageJson["author"].get<std::string>() + "\n";
				outputString += "\t" + packageJson["type"].get<std::string>() + "\n";
				outputString += "\t" + packageJson["license"].get<std::string>() + "\n";

				result["items"][counter] = ofJson::object();
				result["items"][counter]["clone_url"] = packageJson["cloneUrl"].get<std::string>();

				counter++;
			}
			else
			{
				//            TODO: check tags
				//            ofLogNotice("tags")<<packageJson["tags"];
			}
			file.close();
		}
	}

	IFNOTSILENT(
		if (counter > 0) {
			std::cout << outputString << endl;

			if (!addPackageToAddonsMakeFile(maybeInstallOneOfThePackages(result)))
			{
				if (getBoolAnswer("Okey-dokey, do you want to search it on github?"))
				{
					auto package = maybeInstallOneOfThePackages(searchPackageOnGithubByName(name));
					if (!package.isEmpty())
					{
						addPackageToAddonsMakeFile(package);
					}
				}
			}
		} else {
			std::cout << "Unfortunately this package was not found in the database." << endl;
			if (getBoolAnswer("But it is probably available on github. Wanna give it a try?"))
			{
				addPackageToAddonsMakeFile(maybeInstallOneOfThePackages(searchPackageOnGithubByName(name)));
			}
		});
	return result;
}

bool ofPackageManager::installPackagesFromAddonsMakeFile()
{
	IFNOTSILENT(ofLogNotice("ofPackageManager") << "installing packages listed in addons.make";);
	ofFile addonsMakeFile(getAbsolutePath("addons.make"));
	if (addonsMakeFile.exists())
	{
		ofBuffer fileBuffer = addonsMakeFile.readToBuffer();
		for (auto line : fileBuffer.getLines())
		{
			auto words = ofSplitString(ofTrim(line), "#");
			auto path = getPathAndName(words[0]).first;
			auto name = getPathAndName(words[0]).second;

			if (path.empty())
			{
				path = ofFilePath::join(getOfPath(), "addons");
			}

			switch (words.size())
			{
			case 1:
			{
				if (!isCoreAddon(name))
				{
					installPackageById(name, "latest", path);
				}
				break;
			}
			case 2:
			{
				std::vector<std::string> packageParts = ofSplitString(words[1], "@");
				std::string checkout = "latest";
				std::string package = packageParts[0];
				if (packageParts.size() > 1)
				{
					checkout = packageParts[1];
				}
				if (isGitUrl(words[1]))
				{
					installPackageByUrl(package, checkout, path);
				}
				else if (isGithubPair(package))
				{
					auto url = "https://github.com/" + package + ".git";
					installPackageByUrl(url, checkout, path);
				}
				else
				{
					if (!isCoreAddon(package))
					{
						installPackageById(package, checkout, path);
					}
				}
				break;
			}
			default:
				break;
			}
		}
		return true;
	}
	else
	{
		IFNOTSILENT(ofLogError("install") << "Sorry, but there is no addons.make file in this directory.";);
		return false;
	}
}

void ofPackageManager::installDependenciesFromAddonConfig(std::string path, std::string destination)
{
	if (!ofFilePath::isAbsolute(path))
	{
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile addonConfigFile(ofFilePath::join(path, "addon_config.mk"));
	if (addonConfigFile.exists())
	{
		ofBuffer fileBuffer = addonConfigFile.readToBuffer();
		for (auto line : fileBuffer.getLines())
		{
			line = ofTrim(line);
			// check if line starts with ADDON_DEPENDENCIES =
			if (ofTrim(ofSplitString(line, "=").front()) == "ADDON_DEPENDENCIES")
			{
				auto rightSide = ofSplitString(line, "=").back();
				// check if right side contains a comment
				auto parts = ofSplitString(rightSide, "#");
				if (parts.size() > 1)
				{ // contains a comment
					auto key = parts[1];
					if (!isCoreAddon(key))
					{
						installPackage(key, destination);
					}
				}
				else
				{ // does not contain a comment
					for (auto key : ofSplitString(ofTrim(rightSide), " "))
					{
						if (!isCoreAddon(key))
						{
							installPackage(ofTrim(key), destination);
						}
					}
				}

				// check if line starts with ADDON_DEPENDENCIES +=
			}
			else if (ofTrim(ofSplitString(line, "+=").front()) == "ADDON_DEPENDENCIES")
			{
				auto rightSide = ofSplitString(line, "+=").back();
				// check if right side contains a comment
				auto parts = ofSplitString(rightSide, "#");
				if (parts.size() > 1)
				{ //contains a comment
					auto key = parts[1];
					ofLogNotice("install key") << key;
					if (!isCoreAddon(key))
					{
						installPackage(key, destination);
					}
				}
				else
				{
					auto key = ofSplitString(line, "+=").back();
					// TODO: multiple ids
					if (!isCoreAddon(key))
					{
						installPackage(ofTrim(key), destination);
					}
				}
			}
		}
	}
	else
	{
		IFNOTSILENT(ofLogError("install") << "Package does not contain an addon_config file" << path;);
	}
}

ofPackage ofPackageManager::installPackage(std::string addon, std::string destinationPath)
{
	auto parts = ofSplitString(addon, "@");
	std::string checkout = "latest";
	addon = parts[0];
	if (parts.size() > 1)
	{
		checkout = parts[1];
	}
	if (isCoreAddon(addon))
	{
		IFNOTSILENT(ofLogNotice("install") << addon << " seems to be a core addon.";);
		return ofPackage();
	}

	if (isGitUrl(addon))
	{
		return installPackageByUrl(addon, checkout, destinationPath);
	}
	else if (isGithubPair(addon))
	{
		return installPackageByGithub(addon, checkout, destinationPath);
	}
	else
	{
		return installPackageById(addon, checkout, destinationPath);
	}
}
ofPackage ofPackageManager::installPackageById(std::string id, std::string checkout, std::string destinationPath)
{
	if (isCoreAddon(id))
	{
		IFNOTSILENT(ofLogNotice("install") << id << " seems to be a core addon.";);
		return ofPackage();
	}
	if (destinationPath.empty())
	{
		destinationPath = _configJson["localAddonsPath"];
	}
	destinationPath = getAbsolutePath(destinationPath);
	std::string packagesPath = _configJson["packagesPath"];
	ofDirectory packagesDirectory(packagesPath);
	packagesDirectory.listDir();
	bool foundPackage = false;
	for (auto file : packagesDirectory.getFiles())
	{
		if (file.getFileName() == id + ".json")
		{
			foundPackage = true;
			ofJson packageJson;
			file.open(file.getAbsolutePath());
			file >> packageJson;
			ofDirectory directory(ofFilePath::join(_cwdPath, ofFilePath::addTrailingSlash(destinationPath) + id));
			if (directory.exists())
			{
				directory.remove(true);
			}
			std::string url;
			if (!packageJson["cloneUrl"].empty())
			{
				url = packageJson["cloneUrl"];
			}
			else if (!packageJson["github"].empty())
			{
				url = generateGithubUrl(packageJson["github"]);
			}
			return installPackageByUrl(url, checkout, destinationPath);
		}
	}
	if (!foundPackage)
	{
		IFNOTSILENT(
			ofLogError("search") << "Unfortunately " << id << " was not found in the database.";
			if (getBoolAnswer("But it is probably available on github. Wanna give it a try?")) {
				return maybeInstallOneOfThePackages(searchPackageOnGithubByName(id), destinationPath);
			});
	}
	return ofPackage();
}

void ofPackageManager::updatePackagesDatabase()
{
	IFNOTSILENT(
		ofLogWarning("update") << "This is currently not yet implemented. Please pull the packages database manually.";);
}

bool ofPackageManager::isCoreAddon(std::string id)
{
	id = ofSplitString(id, "@").front();
	auto coreAddons = getCoreAddons();
	return std::find(coreAddons.begin(), coreAddons.end(), id) != coreAddons.end();

	auto globalAddonsDir = ofDirectory(ofFilePath::join(getOfPath(), "addons"));
	globalAddonsDir.listDir();
	for (auto file : globalAddonsDir.getFiles())
	{
		auto path = file.getAbsolutePath();
		if (file.getFileName() == id && file.isDirectory())
		{ //starts with ofx
			ofxGit::repository addon(path);
			if (!addon.isRepository())
			{
				return true;
			}
		}
	}
	return false;
}

std::string ofPackageManager::getOfPath()
{
	return _configJson["ofPath"];
}

ofJson ofPackageManager::getConfig()
{
	ofJson packageManagerJson;
	ofFile packageFile(getAbsolutePath("ofPackageManager.json"));
	std::string path = _cwdPath;
	while (!hasPackageManagerConfig(getAbsolutePath(path)))
	{
		ofFile file(path);
		fs::path p(path);
		path = p.parent_path().string();
		// TODO: does that work on windows
		if (path.size() < 4)
		{
			break;
		}
	}
	packageFile.open(ofFilePath::join(path, "ofPackageManager.json"));
	ofFile hiddenPackageFile(ofFilePath::join(path, ".ofPackageManager.json"));
	if (packageFile.exists())
	{
		packageFile >> packageManagerJson;
	}
	else if (hiddenPackageFile.exists())
	{
		hiddenPackageFile >> packageManagerJson;
	}
	else
	{
		packageFile.open(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json"));
		if (packageFile.exists())
		{
			packageFile >> packageManagerJson;
		}
	}
	return packageManagerJson;
}
version ofPackageManager::getVersion()
{
	return version(OFAPP_MAJOR_VERSION, OFAPP_MINOR_VERSION, OFAPP_PATCH_VERSION);
}

void ofPackageManager::setConfig(ofJson config)
{
	_configJson = config;
}
void ofPackageManager::setCwdPath(std::string path)
{
	_cwdPath = path;
}
void ofPackageManager::setSilent(bool value)
{
	_silent = value;
	ofxGit::repository::setSilent(value);
}
std::string ofPackageManager::generateGithubUrl(std::string github)
{
	return "https://github.com/" + github + ".git";
}

std::string ofPackageManager::getAbsolutePath(std::string path)
{
	if (ofFilePath::isAbsolute(path))
	{
		return path;
	}
	return ofFilePath::join(_cwdPath, path);
}

std::pair<std::string, std::string> ofPackageManager::getPathAndName(std::string name)
{
	auto words = ofSplitString(name, "/");
	if (words.size() > 1)
	{
		auto path = words[0];
		for (auto i = 1; i < words.size() - 2; i++)
		{
			path + "/" + words[i];
		}
		return std::make_pair(path, words[words.size() - 1]);
	}
	return std::make_pair("", name);
}

bool ofPackageManager::isGitUrl(std::string path)
{
	// TODO: check if string ends with .git
	return path.substr(0, 4) == "git@" || path.substr(0, 4) == "http";
}

bool ofPackageManager::isGithubPair(std::string path)
{
	return ofSplitString(path, "/").size() > 1;
}

bool ofPackageManager::hasAddonsMakeFile(std::string path)
{
	if (!ofFilePath::isAbsolute(path))
	{
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile addonsMakeFile(ofFilePath::join(path, "addons.make"));
	return addonsMakeFile.exists();
}

bool ofPackageManager::hasAddonConfigFile(std::string path)
{
	if (!ofFilePath::isAbsolute(path))
	{
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile addonConfigFile(ofFilePath::join(path, "addon_config.mk"));
	return addonConfigFile.exists();
}

bool ofPackageManager::hasPackageManagerConfig(std::string path)
{
	if (!ofDirectory::doesDirectoryExist(path))
	{
		IFNOTSILENT(
			ofLogWarning() << "dir does not exist " << path;);
		return false;
	}
	ofFile packageManagerConfigFile(ofFilePath::join(path, "ofPackageManager.json"));
	ofFile hiddenPackageManagerConfigFile(ofFilePath::join(path, ".ofPackageManager.json"));
	return packageManagerConfigFile.exists() || hiddenPackageManagerConfigFile.exists();
}
bool ofPackageManager::isConfigured()
{
	std::string path = _cwdPath;
	while (!hasPackageManagerConfig(getAbsolutePath(path)))
	{
		ofFile file(path);
		fs::path p(path);
		path = p.parent_path().string();
		// TODO: does that work on windows
		if (path.size() < 4)
		{
			return hasPackageManagerConfig(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json"));
		}
	}
	return true;
}
