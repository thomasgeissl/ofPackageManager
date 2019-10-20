#include "ofApp.h"
#include "ofxCommandLineUtils.h"
namespace fs = std::filesystem;

ofPackageManager::ofPackageManager(std::string cwdPath) : _cwdPath(cwdPath),
														  _configJson(getConfig())
{
}

void ofPackageManager::addPackageToAddonsMakeFile(ofPackage package)
{
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
		// ofLogNotice() << content;
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
		ofLogNotice("ofPackageManager") << "successfully updated addons.make";
	}
	else
	{
		ofLogError("ofPackageManager") << "Could not update addons.make";
	}
}

void ofPackageManager::addPackageToAddonsMakeFile(std::string path)
{
	ofxGit::repository repo(ofFilePath::join(_cwdPath, path));
	if (repo.isRepository())
	{
		auto url = repo.getRemoteUrl();
		auto checkout = repo.getCommitHash();
		addPackageToAddonsMakeFile(ofPackage(path, url, checkout));
	}
	else
	{
		ofDirectory directory(ofFilePath::join(_cwdPath, path));
		if (directory.exists()) // && directory.size() > 0
		{
			if (getBoolAnswer(path + " is not a git repository, but a non-empty directory. Do you want to add all its children?"))
			{
				addPackagesToAddonsMakeFile(path);
			}
		}
	}
}

void ofPackageManager::addPackagesToAddonsMakeFile(std::string path)
{
	ofDirectory directory(ofFilePath::join(_cwdPath, path));
	if (!directory.exists())
	{
		ofLogError() << "directory does not exit";
		return;
	}
	directory.listDir();
	for (auto file : directory.getFiles())
	{
		if (file.isDirectory())
		{
			addPackageToAddonsMakeFile(ofFilePath::join(path, file.getFileName()));
		}
	}
}
void ofPackageManager::addPackagesToAddonsMakeFile(std::vector<std::string> paths)
{
	for (auto path : paths)
	{
		addPackageToAddonsMakeFile(ofFilePath::join(_cwdPath, path));
	}
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
		ofLogWarning("config") << "Config file already exits.";
		if (!getBoolAnswer("Do you want to override it?"))
		{
			return;
		}
	}

	ofJson configJson;
	configJson["ofPath"] = getStringAnswer("Absolute path to openFrameworks?", ofFilePath::getAbsolutePath(getAbsolutePath("../../.."), false));
	auto packagesPath = getStringAnswer("Absolute path to packages directory?", ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackages"));
	configJson["packagesPath"] = packagesPath;
	configJson["localAddonsPath"] = getStringAnswer("local addons directory?", "local_addons");

	configFile.create();
	configFile.open(configPath, ofFile::WriteOnly);
	configFile << configJson.dump(4);
	configFile.close();

	if (ofDirectory::doesDirectoryExist(packagesPath, false))
	{
		ofLogError("config") << "The packages database exits already. Please update manually (cd " + packagesPath + " && git pull).";
	}
	else
	{
		ofxGit::repository repo(packagesPath);
		if (repo.clone("https://github.com/thomasgeissl/ofPackages.git"))
		{
			ofLogNotice("config") << "Successfully cloned packages database";
		}
		else
		{
			ofLogError("config") << "Could not clone packages database";
		}
	}
}

void ofPackageManager::doctor()
{
	printVersion();

	// check version of ofPackageManager
	ofHttpResponse request = ofLoadURL("https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/defines.h");
	auto defines = request.data.getText();
	auto currentVersion = version(OFAPP_MAJOR_VERSION, OFAPP_MINOR_VERSION, OFAPP_PATCH_VERSION);
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
	if (
		mostRecentVersion._major > currentVersion._major ||
		mostRecentVersion._minor > currentVersion._minor ||
		mostRecentVersion._patch > currentVersion._patch)
	{
		ofLogNotice("doctor") << "There is a new version of ofPackageManager available! "
							  << "\nThe most recent version is " << mostRecentVersion._major << "." << mostRecentVersion._minor << "." << mostRecentVersion._patch;
	}
	else
	{
		ofLogNotice("doctor") << "You are up to date. Currently there is no newer version of ofPackageManager available";
	}

	// check version of ofPackages
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
		outputString += ofToString(counter++);
		outputString += ": ";
		outputString += name;
		outputString += "\n";
	}
	ofLogNotice("search") << outputString;
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
		if (repoName.substr(0, 3) == "ofx")
		{
			outputString += ofToString(counter++);
			outputString += ": ";
			std::string name = repo["full_name"];
			outputString += name;
			outputString += "\n";
		}
	}
	ofLogNotice("search") << outputString;
	return resultJson;
}

ofPackage ofPackageManager::installPackageByGithub(std::string github, std::string checkout, std::string destinationPath)
{
	return installPackageByUrl(generateGithubUrl(github), checkout, destinationPath);
}

ofPackage ofPackageManager::installPackageByUrl(std::string url, std::string checkout, std::string destinationPath)
{
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
			if (getBoolAnswer(destinationPath + "/" + name + " already exists. Do you want to pull and checkout the specified commit?", true))
			{
				ofLogNotice("TODO") << "Unfortunately it is not yet implemented due to missing ofxGit::repo::pull";
				// TODO: pull and checkout, pull still does not work in ofxGit2
			}
		}
		else
		{
			if (repo.clone(url))
			{
				ofLogNotice("install") << "Successfully cloned repo " << url;
			}
			if (checkout != "latest")
			{
				// checkout the version
				repo.checkout(checkout);
			}
			else
			{
				// get commit hash
				checkout = repo.getCommitHash();
			}
		}
	}
	else
	{
		destinationDirectory.create();
		if (repo.clone(url))
		{
			ofLogNotice("install") << "Successfully cloned repo " << url;
		}
		if (checkout != "latest")
		{
			// checkout the version
			repo.checkout(checkout);
		}
		else
		{
			// get commit hash
			checkout = repo.getCommitHash();
		}
	}

	if (hasAddonConfigFile(ofFilePath::join(destinationPath, name)))
	{
		installDependenciesFromAddonConfig(ofFilePath::join(destinationPath, name), destinationPath);
	}

	return ofPackage(ofFilePath::join(ofFilePath::makeRelative(_cwdPath, destinationPath), name), url, checkout);
}

ofPackage ofPackageManager::maybeInstallOneOfThePackages(ofJson packages, std::string destinationPath)
{
	if (getBoolAnswer("Do you wanna install any of them?"))
	{
		auto index = getIntAnswer("Which one? Please enter the corresponding number.", 0);
		if (index < packages["items"].size())
		{
			ofLogNotice() << packages["items"][index]["name"];
			return installPackageByUrl(packages["items"][index]["clone_url"], "latest", destinationPath);
		}
	}
	return ofPackage();
}

void ofPackageManager::searchPackageInDatabaseById(std::string name)
{
	std::string databasePath = _configJson["packagesPath"];
	ofDirectory ofPackagesDirectory(databasePath);
	ofPackagesDirectory.listDir();
	auto foundPackage = false;
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
				foundPackage = true;
				ofLogNotice("search") << packageJson["cloneUrl"];
			}
			else
			{
				//            TODO: check tags
				//            ofLogNotice("tags")<<packageJson["tags"];
			}
			file.close();
		}
	}
	if (!foundPackage)
	{
		ofLogError("search") << "Unfortunately this package was not found in the database.";
		if (getBoolAnswer("But it is probably available on github. Wanna give it a try?"))
		{
			searchPackageOnGithubByName(name);
		}
	}
}

void ofPackageManager::printInfo()
{
	ofLogNotice("ofPackageManager") << "info";
	printVersion();
	printPaths();
}
void ofPackageManager::printPaths()
{
	ofLogNotice("path") << "TODO";
	ofLogNotice("global config path") << ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json");
}
void ofPackageManager::printManual()
{
	cout << "ofPackageManager version " << OFAPP_MAJOR_VERSION << "." << OFAPP_MINOR_VERSION << "." << OFAPP_PATCH_VERSION << endl
		 << endl;

	cout << "Configuration" << endl;
	cout << "The package manager needs to know where to find the openFrameworks installation and the packages database. It can be configured globally or locally. If there is no local (per project) config, the package manager will use the global configuration." << endl;
	cout << "* Configure the package manager globally after the installation: ofPackageManager config -g" << endl;
	cout << "* Configure the package manager locally: ofPackageManager config" << endl
		 << endl;

	cout << "Installation of packages" << endl;
	cout << "Packages can be installed via an id (if available in the database), github name or git url. It is also possible to specify the version by appending the commit hash or tag. Packages can be installed locally or globally. It is recommended to install them locally. The package manager will also install the packages' dependencies." << endl;
	cout << "The package manager will add git url and commit hash to the addons.make file." << endl;
	cout << "* Install by id: ofPackageManager install ofxMidi" << endl;
	cout << "* Install by github name: ofPackageManager install danomatika/ofxMidi" << endl;
	cout << "* Install by git url: ofPackageManager install https://github.com/danomatika/ofxMidi.git" << endl;
	cout << "* Install a specific version: ofPackageManager install ofxMidi@1.1.1" << endl
		 << endl;

	cout << "Installation of a project's dependencies" << endl;
	cout << "It will clone all dependencies and check out the specified version in the addons.make file." << endl;
	cout << "* ofPackageManager install" << endl
		 << endl;

	cout << "Adding manually cloned addons" << endl;
	cout << "It will clone all dependencies and check out the specified version in the addons.make file." << endl;
	cout << "* Add a single package: ofPackageManager add local_addons/ofxMidi" << endl;
	cout << "* Add all packages inside one directory: ofPackageManager add -A local_addons" << endl
		 << endl;

	cout << "Further information can be found on github (https://github.com/thomasgeissl/ofPackageManager/)." << endl;
}

void ofPackageManager::printAvailablePackages()
{
	std::string databasePath = _configJson["packagesPath"];
	ofDirectory ofPackagesDirectory(databasePath);
	ofPackagesDirectory.listDir();
	for (auto file : ofPackagesDirectory.getFiles())
	{
		if (file.getExtension() == "json")
		{
			ofJson packageJson;
			file.open(file.getAbsolutePath());
			file >> packageJson;

			std::cout << "# " << packageJson["name"] << std::endl;
			std::cout << "## Description" << std::endl;
			std::cout << packageJson["description"] << std::endl;
			std::cout << "## Author" << std::endl;
			std::cout << packageJson["author"] << std::endl;
			std::cout << "## cloneUrl" << std::endl;
			std::cout << packageJson["cloneUrl"] << std::endl;
			std::cout << "## License" << std::endl;
			std::cout << packageJson["license"] << std::endl;
			std::cout << std::endl;

			file.close();
		}
	}
}

void ofPackageManager::installPackagesFromAddonsMakeFile()
{
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
	}
	else
	{
		ofLogError("install") << "Sorry, but there is no addons.make file in this directory.";
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
		ofLogError("install") << "Package does not contain an addon_config file" << path;
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
		ofLogNotice("install") << addon << " seems to be a core addon.";
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
		ofLogNotice("install") << id << " seems to be a core addon.";
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
		ofLogError("search") << "Unfortunately this package was not found in the database.";
		if (getBoolAnswer("But it is probably available on github. Wanna give it a try?"))
		{
			return maybeInstallOneOfThePackages(searchPackageOnGithubByName(id), destinationPath);
		}
	}
	return ofPackage();
}

void ofPackageManager::updatePackagesDatabase()
{
	ofLogWarning("update") << "This is currently not yet implemented. Please pull the packages database manually.";
}

void ofPackageManager::printVersion()
{
	ofLogNotice("version") << OFAPP_MAJOR_VERSION << "." << OFAPP_MINOR_VERSION << "." << OFAPP_PATCH_VERSION;
}

bool ofPackageManager::isCoreAddon(std::string id)
{
	id = ofSplitString(id, "@").front();
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
	ofLogNotice() << "using config in " << path;
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

bool ofPackageManager::isGitRepository(std::string path)
{
	if (!ofFilePath::isAbsolute(path))
	{
		path = ofFilePath::join(_cwdPath, path);
	}
	ofDirectory gitDirectory(ofFilePath::join(path, ".git"));
	return gitDirectory.exists();
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
		ofLogNotice() << "dir does not exist " << path;
		return false;
	}
	ofFile packageManagerConfigFile(ofFilePath::join(path, "ofPackageManager.json"));
	ofFile hiddenPackageManagerConfigFile(ofFilePath::join(path, ".ofPackageManager.json"));
	return packageManagerConfigFile.exists() || hiddenPackageManagerConfigFile.exists();
}
