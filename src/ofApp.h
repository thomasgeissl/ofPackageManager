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
	std::vector<ofPackage> getCorePackages();
	std::vector<ofPackage> getGloballyInstalledPackages();
	std::vector<ofPackage> getLocallyInstalledPackages();
	std::vector<ofPackage> getPackagesListedInAddonsMakeFile();
	std::string getAddonsPath();


	ofJson searchPackageInDatabaseById(std::string id);
	ofJson searchPackageOnGithubByName(std::string name);
	ofJson searchPackageOnGithubByUser(std::string user);

	std::vector<ghRepo> searchPackageOnGithubByName2(std::string name);
	bool installPackagesDatabase();
	bool updatePackagesDatabase();
	bool hasPackagesDatabase();
	bool isCorePackage(std::string id);

	std::string getCwdPath();
	std::string getOfPath();
	std::string getMyAppsPath();
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

	bool generateProject(){
		return false;
	}
	bool generateProject(std::string path)
	{
		setCwdPath(path);
		setOFRoot(getOfPath());
		if(ofFile::doesFileExist(ofFilePath::join(path, "addons.make"))){
			ofLogNotice() << "TODO: backup addons.make";
		}

		auto target = ofGetTargetPlatform();
		auto project = getTargetProject(target);
		auto targetS = getTargetString(target);
		auto templateName = "";
		project->create(path, templateName);

		for(auto package : getPackagesListedInAddonsMakeFile()){
			ofLogNotice() << package.toString();
			project->addAddon(package.getPath());
		}
		// project->parseAddons();
		return project->save();
	}

	void updateMultipleProjects(std::string path){

	}

private:
	bool _silent;
	std::string _cwdPath;
	std::string _configDirPath;
	std::string _packagesPath;
	std::string _ofPackagesUrl;
	std::string _globalConfigPath;
	std::string _localAddonsPath;
	ofJson _configJson;
	ofxCommandLineUtils _clu;
};
