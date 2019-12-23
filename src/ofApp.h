#pragma once

#include "ofMain.h"
#include "ofxGit2.h"

#include "ofPackage.h"
#include "defines.h"
#include "version.h"

class ofPackageManager
{
public:
	ofPackageManager(std::string cwdPath);

	bool addPackageToAddonsMakeFile(ofPackage package);
	bool addPackageToAddonsMakeFile(std::string path);
	bool addPackagesToAddonsMakeFile(std::string path);
	bool addPackagesToAddonsMakeFile(std::vector<std::string> paths);
	void configure(bool global = false);
	bool isNewerVersionAvailable();
	void generateDatabaseEntryFile();

	bool installPackagesFromAddonsMakeFile();
	void installDependenciesFromAddonConfig(std::string path, std::string destination);

	ofPackage installPackage(std::string key, std::string destinationPath = "");
	ofPackage installPackageById(std::string id, std::string checkout = "", std::string destinationPath = "");
	ofPackage installPackageByGithub(std::string github, std::string checkout = "", std::string destinationPath = "");
	ofPackage installPackageByUrl(std::string url, std::string checkout = "", std::string destinationPath = "");
	ofPackage maybeInstallOneOfThePackages(ofJson packages, std::string destinationPath);
	ofJson getAvailablePackages();
	std::vector<std::string> getCoreAddons();
	std::vector<ofPackage> getGloballyInstalledPackages();
	std::vector<ofPackage> getLocallyInstalledPackages();

	void generateProject();

	ofJson searchPackageInDatabaseById(std::string id);
	ofJson searchPackageOnGithubByName(std::string name);
	ofJson searchPackageOnGithubByUser(std::string user);
	void updatePackagesDatabase();
	bool isCoreAddon(std::string id);

	std::string getOfPath();
	ofJson getConfig();
	version getVersion();
	void setConfig(ofJson config);
	void setCwdPath(std::string path);
	void setSilent(bool value = true);
	std::string generateGithubUrl(std::string github);
	std::string getAbsolutePath(std::string path);
	std::pair<std::string, std::string> getPathAndName(std::string name);

	bool isGitUrl(std::string path);
	bool isGithubPair(std::string path);
	bool hasAddonsMakeFile(std::string path);
	bool hasAddonConfigFile(std::string path);
	bool hasPackageManagerConfig(std::string path);

	bool isConfigured();

	std::string _cwdPath;
	ofJson _configJson;
	bool _silent;
};
