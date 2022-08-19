#pragma once

#include "ofMain.h"
#include "ofxGit2.h"
#include "ofxCommandLineUtils.h"

#include "defines.h"
#include "ofPackage.h"
#include "ofVersion.h"
#include "ghRepo.h"
#include "./generators/projectGenerator/ofProjectGenerator.h"

class ofPackageManager
{
public:
	class config {
	};
	ofPackageManager(std::string cwdPath);

	bool addPackageToAddonsMakeFile(ofPackage package);
	bool addPackageToAddonsMakeFile(std::string path);
	bool addPackagesToAddonsMakeFile(std::string path);
	bool addPackagesToAddonsMakeFile(std::vector<std::string> paths);

	bool isNewerVersionAvailable();
	ofVersion getNewestAvailableVersion();

	bool installPackagesFromAddonsMakeFile();
	bool installDependenciesFromAddonConfig(std::string path, std::string destination);

	bool installPackage(ofPackage package);
	ofPackage installPackage(std::string key, std::string destinationPath = "");
	ofPackage installPackageById(std::string id, std::string checkout = "", std::string destinationPath = "");
	ofPackage installPackageByGithub(std::string github, std::string checkout = "", std::string destinationPath = "");
	ofPackage installPackageByUrl(std::string url, std::string checkout = "", std::string destinationPath = "");
	ofPackage maybeInstallOneOfThesePackages(ofJson packages, std::string destinationPath);

	std::vector<ofPackage> getCorePackages();
	std::vector<ofPackage> getGloballyInstalledPackages();
	std::vector<ofPackage> getLocallyInstalledPackages();
	std::vector<ofPackage> getPackagesListedInAddonsMakeFile();

	ofJson searchPackageInDatabaseById(std::string id);
	ofJson searchPackageOnGithubByName(std::string name);
	ofJson searchPackageOnGithubByUser(std::string user);

	std::vector<ghRepo> searchPackageOnGithubByName2(std::string name);
	bool installPackagesDatabase(bool force = false);
	bool updatePackagesDatabase();
	bool hasPackagesDatabase();
	bool isCorePackage(std::string id);

	std::string getCwdPath();
	std::string getProjectPath();
	std::string getOfPath();
	std::string getAddonsPath();
	std::string getMyAppsPath();
	std::string getLocalAddonsPath();
	std::string getPackagesDatabasePath();
	ofJson getPackagesDatabase();
	ofVersion getVersion();
	std::vector<ofPackage> getMissingPackages();

	void setProjectPath(std::string path);
	void setSilent(bool value = true);
	std::string getAbsolutePath(std::string path);
	std::pair<std::string, std::string> getPathAndName(std::string name);

	bool hasAddonsMakeFile(std::string path);
	bool hasAddonConfigFile(std::string path);
	bool isLocatedInsideOfDirectory(std::string path);
	bool isProject(std::string path);

	std::string findOfPathInwardly(std::string path, int depth);
	std::string findOfPathOutwardly(std::string path, int maxLevel = 8);

	bool generateProject(std::string path, std::vector<ofPackage> packages = {}, std::vector<ofTargetPlatform> platforms = {}, baseProject::Template = baseProject::Template(), std::vector<std::string> additionalSources = {});
	bool recursivelyGenerateProjects(std::string path, std::vector<ofTargetPlatform> platforms = {}, baseProject::Template = baseProject::Template());
	ofJson generateSingleFileProject(std::string path);
	ofJson directoryToJson(std::string path);
	bool jsonToDirectory(ofJson data, std::string path);
	bool createFromSingleFileProject(ofJson sfp, std::string path);

	std::vector<ofTargetPlatform> getPlatforms();
	std::map<ofTargetPlatform, std::vector<baseProject::Template>> getTemplates();


private:
	bool _silent;
	std::string _cwdPath;
	std::string _projectPath;
	std::string _appDataDirectoryPath;
	std::string _ofPackagesUrl;
	std::string _globalConfigPath;
	std::string _localAddonsPath;
	ofxCommandLineUtils _clu;
};
