#pragma once

#include "ofMain.h"
#include "ofxGit2.h"
#include "ofxCommandLineUtils.h"

#include "defines.h"
#include "ofPackage.h"
#include "ofVersion.h"

class ofPackageManager
{
public:
	ofPackageManager(std::string cwdPath);

	bool addPackageToAddonsMakeFile(ofPackage package);
	bool addPackageToAddonsMakeFile(std::string path);
	bool addPackagesToAddonsMakeFile(std::string path);
	bool addPackagesToAddonsMakeFile(std::vector<std::string> paths);
	bool configure(bool global = false);
	bool isNewerVersionAvailable();
	ofVersion getNewestAvailableVersion();
	void generateDatabaseEntryFile();

	bool installPackagesFromAddonsMakeFile();
	bool installDependenciesFromAddonConfig(std::string path, std::string destination);

	bool installPackage(ofPackage package);
	ofPackage installPackage(std::string key, std::string destinationPath = "");
	ofPackage installPackageById(std::string id, std::string checkout = "", std::string destinationPath = "");
	ofPackage installPackageByGithub(std::string github, std::string checkout = "", std::string destinationPath = "");
	ofPackage installPackageByUrl(std::string url, std::string checkout = "", std::string destinationPath = "");
	ofPackage maybeInstallOneOfThePackages(ofJson packages, std::string destinationPath);
	ofJson getAvailablePackages();
	std::vector<std::string> getCorePackages();
	std::vector<ofPackage> getGloballyInstalledPackages();
	std::vector<ofPackage> getLocallyInstalledPackages();
	std::vector<ofPackage> getPackagesListedInAddonsMakeFile();

	bool generateProject();

	ofJson searchPackageInDatabaseById(std::string id);
	ofJson searchPackageOnGithubByName(std::string name);
	ofJson searchPackageOnGithubByUser(std::string user);
	bool installPackagesDatabase();
	bool updatePackagesDatabase();
	bool hasPackagesDatabase();
	bool isCorePackage(std::string id);

	std::string getCwdPath();
	std::string getOfPath();
	std::string getLocalAddonsPath();
	std::string getOfPackagesPath();
	ofJson getConfig();
	ofVersion getVersion();

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
	bool isLocatedInsideOfDirectory(std::string path);
	std::string findOfPathInwardly(std::string path, int depth);
	std::string findOfPathOutwardly(std::string path, int maxLevel = 8);

private:
	bool _silent;
	std::string _cwdPath;
	std::string _configDirPath;
	std::string _packagesPath;
	std::string _globalConfigPath;
	std::string _localAddonsPath;
	ofJson _configJson;
	ofxCommandLineUtils _clu;
};
