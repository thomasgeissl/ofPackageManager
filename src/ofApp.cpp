#include "ofApp.h"
#include "./helpers.h"

#define IFNOTSILENT(...) \
	if (!_silent)        \
	{                    \
		__VA_ARGS__      \
	}
#define IFSILENT(...) \
	if (_silent)      \
	{                 \
		__VA_ARGS__   \
	}
namespace fs = std::filesystem;

ofPackageManager::ofPackageManager(std::string cwdPath) : _silent(false),
														  _cwdPath(cwdPath),
														  _configDirPath(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager")),
														  _packagesPath(ofFilePath::join(_configDirPath, "ofPackages")),
														  _ofPackagesUrl("https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/ofPackages.json"),
														  _globalConfigPath(ofFilePath::join(_configDirPath, "cli.config.json")),
														  _localAddonsPath("local_addons"),
														  _configJson(getConfig())
{
}

bool ofPackageManager::addPackageToAddonsMakeFile(ofPackage package)
{
	if (package.empty())
	{
		return false;
	}
	ofFile addonsMakeFile(getAbsolutePath("addons.make"), ofFile::ReadOnly);
	if (!addonsMakeFile.exists())
	{
		addonsMakeFile.create();
	}

	auto stringToAdd = package.getPath();
	if (!package.getUrl().empty())
	{
		stringToAdd += " #" + package.getUrl();
		if (!package.getCheckout().empty())
		{
			stringToAdd += "@" + package.getCheckout();
		}
	}

	ofBuffer fileBuffer = addonsMakeFile.readToBuffer();
	std::string content;
	auto foundPackage = false;
	for (auto line : fileBuffer.getLines())
	{
		auto words = ofSplitString(ofTrim(line), "#");
		// if path addon already listed in addon.make
		// then update it
		// else append a new line
		if (ofTrim(words[0]) == package.getPath())
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
			if (_clu.getBoolAnswer(path + " is not a git repository, but a non-empty directory. Do you want to add all its children?"))
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
bool ofPackageManager::configure(bool global)
{
	auto configPath = ofFilePath::join(_cwdPath, "ofPackageManager.cli.config.json");
	std::string relativeOrAbsolute = "relative";
	if (global)
	{
		configPath = _globalConfigPath;
		relativeOrAbsolute = "absolute";
	}
	ofFile configFile(configPath);
	if (configFile.exists())
	{
		if (!_silent)
		{
			ofLogWarning("config") << "Config file already exits.";
			if (!_clu.getBoolAnswer("Do you want to override it?"))
			{
				return false;
			}
		}
	}

	auto ofPath = ofFilePath::getAbsolutePath(getAbsolutePath("../../.."), false);

	if (!_silent)
	{
		auto findOfAutomatically = _clu.getBoolAnswer("Do you want to automatically detect the openFrameworks directory? This will iterate through your home directory and its children, three levels deep. It might be annoying because your OS might ask for permission to access those directories.");
		if (findOfAutomatically)
		{
			auto foundPath = findOfPathInwardly(ofFilePath::getUserHomeDir(), 3);
			if (!foundPath.empty())
			{
				ofPath = foundPath;
			}
		}
	}

	ofJson configJson;
	configJson["ofPath"] = _clu.getStringAnswer("Absolute path to openFrameworks?", ofPath);
	auto ofPackagesPath = _clu.getStringAnswer("Absolute path to packages directory?", ofFilePath::join(_configDirPath, "ofPackages"));
	configJson["ofPackagesPath"] = ofPackagesPath;
	// configJson["localAddonsPath"] = _clu.getStringAnswer("local addons directory?", "local_addons");
	// configJson["pgPath"] = _clu.getStringAnswer("Absolute path to the projet generator?", ofFilePath::join(configJson["ofPath"].get<std::string>(), "projectGenerator-osx"));

	configFile.create();
	configFile.open(configPath, ofFile::WriteOnly);
	configFile << configJson.dump(4);
	configFile.close();

	_configJson = configJson;

	installPackagesDatabase();

	return true;
}

bool ofPackageManager::isNewerVersionAvailable()
{
	return getNewestAvailableVersion() > getVersion();
}
ofVersion ofPackageManager::getNewestAvailableVersion()
{
	ofHttpResponse request = ofLoadURL("https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/src/defines.h");
	auto defines = request.data.getText();
	ofLogNotice() << defines;
	auto mostRecentVersion = ofVersion(-1, -1, -1);
	auto lines = ofSplitString(defines, "\n");
	for (auto line : lines)
	{
		auto parts = ofSplitString(ofToLower(line), " ");
		if (parts.size() == 3)
		{
			if (parts[1].find("major") != std::string::npos)
			{
				mostRecentVersion.setMajor(ofToInt(parts[2]));
			}
			if (parts[1].find("minor") != std::string::npos)
			{
				mostRecentVersion.setMinor(ofToInt(parts[2]));
			}
			if (parts[1].find("patch") != std::string::npos)
			{
				mostRecentVersion.setPatch(ofToInt(parts[2]));
			}
		}
	}
	return mostRecentVersion;
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
std::vector<ghRepo> ofPackageManager::searchPackageOnGithubByName2(std::string name)
{
	std::string url = "https://api.github.com/search/repositories?q=" + name;
	ofHttpRequest request(url, "ofPackageManager");
	request.headers["User-Agent"] = "ofPackageManager";
	ofURLFileLoader loader;
	auto response = loader.handleRequest(request);
	auto resultJson = ofJson::parse(response.data.getText());
	std::vector<ghRepo> repos;
	for (auto repo : resultJson["items"])
	{
		repos.push_back(ghRepo(repo));
	}
	return repos;
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
		destinationPath = getLocalAddonsPath();
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
				if (_clu.getBoolAnswer(destinationPath + "/" + name + " already exists. Do you want to pull and checkout the specified commit?", true))
				{
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

ofPackage ofPackageManager::maybeInstallOneOfThesePackages(ofJson packages, std::string destinationPath = "")
{
	IFSILENT(return ofPackage(););
	if (_clu.getBoolAnswer("Do you wanna install any of them?"))
	{
		auto index = _clu.getIntAnswer("Which one? Please enter the corresponding number.", 0);
		if (index < packages["items"].size())
		{
			if (destinationPath == "")
			{
				destinationPath = "local_addons";
				if (!_clu.getBoolAnswer("Where do you want to install the package. It is recommended to install it locally, are you fine with that? Otherwise it will be installed to your global addons directory"))
				{
					destinationPath = ofFilePath::join(getOfPath(), "addons");
				}
			}
			return installPackageByUrl(packages["items"][index]["clone_url"], "latest", destinationPath);
		}
	}
	return ofPackage();
}

std::vector<ofPackage> ofPackageManager::getCorePackages()
{
	std::vector<ofPackage> corePackages;

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
				corePackages.push_back(ofPackage(file.getFileName()));
			}
		}
	}

	return corePackages;
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
		if (name.find("ofx", 0) == 0 && !isCorePackage(name))
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
	auto packagesPath = getAbsolutePath(getLocalAddonsPath());
	auto packagesDir = ofDirectory(packagesPath);
	if (packagesDir.exists())
	{
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
					packages.push_back(ofPackage(ofFilePath::join(getLocalAddonsPath(), name), repo.getRemoteUrl(), repo.getCommitHash()));
				}
			}
		}
	}
	return packages;
}
std::vector<ofPackage> ofPackageManager::getPackagesListedInAddonsMakeFile()
{
	// local_addons/ofxMidi#url@checkout
	// ofxMidi#url@checkout
	// ofxMidi

	std::vector<ofPackage> packages;
	ofFile addonsMakeFile(getAbsolutePath("addons.make"));
	if (addonsMakeFile.exists())
	{
		ofBuffer fileBuffer = addonsMakeFile.readToBuffer();
		for (auto &line : fileBuffer.getLines())
		{
			auto parts = ofSplitString(ofTrim(line), "#");

			auto path = parts[0];
			std::string url = "";
			std::string checkout = "latest";

			auto dir = getPathAndName(path).first;
			auto name = getPathAndName(path).second;

			if (dir.empty())
			{
				dir = ofFilePath::join(getOfPath(), "addons");
			}

			switch (parts.size())
			{
			case 1:
			{
				// packages.push_back(ofPackage(name, "", "")); //path vs name
				break;
			}
			case 2:
			{
				auto comment = parts[1];
				url = ofSplitString(comment, "@").front();
				checkout = ofSplitString(comment, "@").back();
				if (url == checkout)
				{
					checkout = "latest";
				}
				break;
			}
			default:
				break;
			}
			packages.push_back(ofPackage(ofTrim(path), ofTrim(url), ofTrim(checkout))); //path vs name
		}
	}
	return packages;
}

// bool ofPackageManager::generateProject()
// {
// 	auto isWindows = true;
// #if defined(TARGET_OSX) || defined(TARGET_LINUX)
// 	isWindows = false;
// #endif
// 	std::string pgPath = _configJson["pgPath"].get<std::string>();
// 	std::string ofPath = isWindows ? "/ofPath=" : "-o";
// 	ofPath += "\"";
// 	ofPath += getConfig()["ofPath"].get<std::string>();
// 	ofPath += "\"";
// 	std::string packages = isWindows ? "/addons=" : "-a";
// 	packages += "\"";
// 	auto requiredPackages = getPackagesListedInAddonsMakeFile();
// 	for (auto package : requiredPackages)
// 	{
// 		packages += package.getPath();
// 		packages += ", ";
// 	}
// 	packages += "\"";
// 	auto addons = getConfig()["ofPath"].get<std::string>();
// 	// TODO: platforms and templates

// 	std::string verbose = isWindows ? "/verbose" : "-v";
// 	std::string command = pgPath + " " + verbose + " " + ofPath + " " + packages;
// 	auto result = ofSystem(command);
// 	// TODO: parse result and return success or error
// 	return false;
// }
ofJson ofPackageManager::searchPackageInDatabaseById(std::string name)
{
	installPackagesDatabase();
	ofJson ofPackages = getPackagesDatabase();
	std::string outputString = "The following packages were found in the database: \n";
	std::vector<std::string> keys;
	std::vector<std::string> matchedKeys;
	ofJson result = ofJson::object();
	result["items"] = ofJson::array();

	// currently uses the same structure as github api, should be changed
	auto counter = 0;
	for (auto it : ofJson::iterator_wrapper(ofPackages))
	{
		keys.push_back(it.key());
		auto key = ofToLower(it.key());
		if (key.find(ofToLower(name)) != std::string::npos)
		{
			matchedKeys.push_back(it.key());
			result["items"].push_back({{"id", it.key()}, {"clone_url", it.value()["git"]}});
			outputString += ofToString(counter);
			outputString += ": ";
			outputString += it.key();
			outputString += "(";
			outputString += it.value()["git"];
			outputString += ")";
			outputString += "\n";
		}
	}

	IFNOTSILENT(
		if (matchedKeys.size() > 0)
		{
			std::cout << outputString << endl;

			if (!addPackageToAddonsMakeFile(maybeInstallOneOfThesePackages(result)))
			{
				if (_clu.getBoolAnswer("Okey-dokey, do you want to search it on github?"))
				{
					auto package = maybeInstallOneOfThesePackages(searchPackageOnGithubByName(name));
					if (!package.empty())
					{
						addPackageToAddonsMakeFile(package);
					}
				}
			}
		} else
		{
			std::cout << "Unfortunately this package was not found in the database." << endl;
			if (_clu.getBoolAnswer("But it is probably available on github. Wanna give it a try?"))
			{
				addPackageToAddonsMakeFile(maybeInstallOneOfThesePackages(searchPackageOnGithubByName(name)));
			}
		});
	return ofJson::array();
}

bool ofPackageManager::installPackagesFromAddonsMakeFile()
{
	auto packages = getPackagesListedInAddonsMakeFile();
	for (auto &package : packages)
	{
		if (!isCorePackage(package.getPath()))
		{
			installPackage(package);
		}
	}
	return true;
}

bool ofPackageManager::installDependenciesFromAddonConfig(std::string path, std::string destination)
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
					if (!isCorePackage(key))
					{
						installPackage(key, destination);
					}
				}
				else
				{ // does not contain a comment
					for (auto key : ofSplitString(ofTrim(rightSide), " "))
					{
						if (!isCorePackage(key))
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
					if (!isCorePackage(key))
					{
						installPackage(key, destination);
					}
				}
				else
				{
					auto key = ofSplitString(line, "+=").back();
					// TODO: multiple ids
					if (!isCorePackage(key))
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
		return false;
	}
	return true;
}

bool ofPackageManager::installPackage(ofPackage package)
{
	auto result = false;
	if (package.getUrl() != "")
	{
		auto installedPackage = installPackageByUrl(package.getUrl(), package.getCheckout(), ofFilePath::getEnclosingDirectory(getAbsolutePath(package.getPath())));
		result = true;
	}
	return result;
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
	if (isCorePackage(addon))
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
	installPackagesDatabase();
	if (isCorePackage(id))
	{
		IFNOTSILENT(ofLogNotice("install") << id << " seems to be a core addon.";);
		return ofPackage();
	}
	if (destinationPath.empty())
	{
		destinationPath = getLocalAddonsPath();
	}
	destinationPath = getAbsolutePath(destinationPath);
	auto ofPackages = getPackagesDatabase();
	if (ofPackages.contains(id))
	{
		return installPackageByUrl(ofPackages[id]["git"], checkout, destinationPath);
	}

	IFNOTSILENT(
		ofLogError("search") << "Unfortunately " << id << " was not found in the database.";
		if (_clu.getBoolAnswer("But it is probably available on github. Wanna give it a try?")) {
			return maybeInstallOneOfThesePackages(searchPackageOnGithubByName(id), destinationPath);
		});
	return ofPackage();
}

bool ofPackageManager::installPackagesDatabase(bool force)
{
	if (!force && hasPackagesDatabase())
	{
		return false;
	}
	ofHttpResponse response = ofLoadURL(_ofPackagesUrl);
	auto ofPackagesJson = ofJson::parse(response.data.getText());
	auto path = ofFilePath::join(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager"), "ofPackages.json");
	ofFile file(path, ofFile::ReadWrite);
	if (!file.exists())
	{
		file.create();
	}
	ofPackagesJson >> file;
	return true;
}
bool ofPackageManager::updatePackagesDatabase()
{
	IFNOTSILENT(
		ofLogWarning("update") << "This is currently not yet implemented. Please pull the packages database manually.";);
	return false;
}

bool ofPackageManager::hasPackagesDatabase()
{
	return ofFile::doesFileExist(getPackagesDatabasePath());
}

bool ofPackageManager::isCorePackage(std::string id)
{
	id = ofSplitString(id, "@").front();
	for (auto corePackage : getCorePackages())
	{
		if (corePackage.getPath() == id)
		{
			return true;
		}
	}
	return false;
}

std::string ofPackageManager::getCwdPath()
{
	return _cwdPath;
}
std::string ofPackageManager::getOfPath()
{
	return findOfPathOutwardly(_cwdPath);
}
std::string ofPackageManager::getAddonsPath()
{
	return ofFilePath::join(getOfPath(), "addons");
}
std::string ofPackageManager::getMyAppsPath()
{
	return ofFilePath::join(getOfPath(), ofFilePath::join("apps", "myApps"));
}
std::string ofPackageManager::getLocalAddonsPath()
{
	return _localAddonsPath;
}
std::string ofPackageManager::getPackagesDatabasePath()
{
	return ofFilePath::join(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager"), "ofPackages.json");
}
ofJson ofPackageManager::getPackagesDatabase()
{
	ofFile file(getPackagesDatabasePath(), ofFile::ReadWrite);
	ofJson ofPackages;
	file >> ofPackages;
	return ofPackages;
}

ofJson ofPackageManager::getConfig()
{
	auto isInsideOf = isLocatedInsideOfDirectory(_cwdPath);
	auto hasLocalConfig = false;
	auto hasGlobalConfig = false;

	ofLogNotice() << "getting config";
	ofLogNotice() << isInsideOf;

	ofJson configJson;
	ofFile configFile;
	ofFile globalConfigFile;
	configFile.open(_globalConfigPath);
	hasGlobalConfig = globalConfigFile.exists();

	std::string path = _cwdPath;
	auto level = 0;
	while (!hasPackageManagerConfig(getAbsolutePath(path)))
	{
		fs::path p(path);
		path = p.parent_path().string();
		level++;
		// TODO: does that work on windows
		if (path.size() < 4 || level > 3)
		{
			break;
		}
	}
	configFile.open(ofFilePath::join(path, "ofPackageManager.cli.config.json"));
	if (configFile.exists())
	{
		hasLocalConfig = true;
		configFile >> configJson;
	}
	else if (isInsideOf)
	{
		configJson["ofPackagesPath"] = ofFilePath::join(_configDirPath, "ofPackages");
		configJson["ofPath"] = findOfPathOutwardly(_cwdPath);
	}
	else if (hasGlobalConfig)
	{
		globalConfigFile >> configJson;
	}
	else
	{
	}

	return configJson;
}
ofVersion ofPackageManager::getVersion()
{
	return ofVersion(OFAPP_MAJOR_VERSION, OFAPP_MINOR_VERSION, OFAPP_PATCH_VERSION);
}

void ofPackageManager::setConfig(ofJson config)
{
	_configJson = config;
}
void ofPackageManager::setCwdPath(std::string path)
{
	_cwdPath = path;
	_configJson = getConfig();
}
void ofPackageManager::setSilent(bool value)
{
	_silent = value;
	ofxGit::repository::setSilent(value);
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
	ofFile packageManagerConfigFile(ofFilePath::join(path, "ofPackageManager.cli.config.json"));
	ofFile cliConfigFile(ofFilePath::join(path, "cli.config.json"));
	return cliConfigFile.exists() || packageManagerConfigFile.exists();
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
			return hasPackageManagerConfig(_configDirPath);
		}
	}
	return true;
}

bool ofPackageManager::isLocatedInsideOfDirectory(std::string path)
{
	return !findOfPathOutwardly(path).empty();
	auto level = 0;
	std::string ofPath = "";
	while (ofPath.empty() && level < 4)
	{
		fs::path p(path);
		if (!findOfPathInwardly(path, 0).empty())
		{
			return true;
		}
		path = p.parent_path().string();
		level++;
	}
	return false;
}
bool ofPackageManager::isProject(std::string path)
{
	ofDirectory dir(path);
	if (!dir.isDirectory())
	{
		return false;
	}
	dir.listDir();
	for (size_t i = 0; i < dir.size(); i++)
	{
		if (dir.getName(i) == "src")
		{
			return true;
		}
	}
	return false;
}

std::string ofPackageManager::findOfPathInwardly(std::string path, int depth)
{
	ofDirectory dir(path);
	if (!dir.canRead() || !dir.canExecute() || depth < 0)
	{
		return "";
	}
	try
	{
		dir.listDir();
	}
	catch (...)
	{
		return "";
	}
	auto foundAddons = false;
	auto foundLibs = false;
	auto foundApps = false;
	for (auto child : dir.getFiles())
	{
		if (child.isDirectory())
		{
			if (child.getFileName() == "addons")
			{
				foundAddons = true;
			}
			if (child.getFileName() == "libs")
			{
				foundLibs = true;
			}
			if (child.getFileName() == "apps")
			{
				foundApps = true;
			}
		}
	}
	if (foundAddons && foundLibs && foundApps)
	{
		return path;
	}
	else
	{
		for (auto child : dir.getFiles())
		{
			if (child.isDirectory())
			{
				auto p = findOfPathInwardly(child.getAbsolutePath(), depth - 1);
				if (!p.empty())
				{
					return p;
				}
			}
		}
		return "";
	}
}

std::string ofPackageManager::findOfPathOutwardly(std::string path, int maxLevel)
{
	auto level = 0;
	std::string ofPath = "";
	while (ofPath.empty() && level < maxLevel)
	{
		fs::path p(path);
		ofPath = findOfPathInwardly(path, 0);
		if (!ofPath.empty())
		{
			return ofPath;
		}
		path = p.parent_path().string();
		level++;
	}
	return ofPath;
}

bool ofPackageManager::generateProject(std::string path, std::vector<ofPackage> packages, std::vector<ofTargetPlatform> platforms)
{
	auto projectPath = path.empty() ? getCwdPath() : path;
	if (platforms.empty())
	{
		platforms.push_back(ofGetTargetPlatform());
	}
	setCwdPath(projectPath);
	setOFRoot(getOfPath());
	ofFile addonsMakeFile = ofFile(ofFilePath::join(projectPath, "addons.make"), ofFile::ReadWrite);
	std::vector<std::string> addonsMakeBackup;
	if (addonsMakeFile.exists())
	{
		ofBuffer buffer = ofBufferFromFile(ofFilePath::join(projectPath, "addons.make"));
		for (auto line : buffer.getLines())
		{
			addonsMakeBackup.push_back(line);
		}
	}
	auto success = true;
	for (auto platform : platforms)
	{
		std::string addonsMakeText = "";
		auto project = getTargetProject(platform);
		auto templateName = "";
		project->create(projectPath, templateName);
		for (auto package : packages)
		{
			project->addAddon(package.getPath());
			addonsMakeText += package.toString();
			addonsMakeText += "\n";
		}
		if(project->save()){
			addonsMakeFile << addonsMakeText;
		}else{
			success = false;
		}
	}
	return success;
}

bool ofPackageManager::recursivelyGenerateProjects(std::string path, std::vector<ofTargetPlatform> platforms)
{
	ofDirectory dir(path);
	if (!dir.isDirectory())
	{
		return false;
	}

	if (isProject(path))
	{
		// nProjectsUpdated++;
		generateProject(path, {}, platforms);
		return true;
	}

	dir.listDir();
	for (size_t i = 0; i < dir.size(); i++)
	{
		ofDirectory subDir(dir.getPath(i));
		if (subDir.isDirectory())
		{
			recursivelyGenerateProjects(dir.getPath(i), platforms);
		}
	}
	return true;
}