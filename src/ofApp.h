#pragma once

#include "defines.h"
#include "ofMain.h"
#include "ofPackage.h"
#include "ofxGit2.h"

class ofPackageManager {
	public:
		ofPackageManager(std::string cwdPath);
		void setCwdPath(std::string cwdPath);

		void addPackageToAddonsMakeFile(ofPackage package);
		void addPackageToAddonsMakeFile(std::string path);
		void addPackagesToAddonsMakeFile(std::string path);
		void addPackagesToAddonsMakeFile(std::vector<std::string> paths);
		void configurePackageManager(bool global = false);
		void doctor();
		void generateDatabaseEntryFile();
		void generateProject();
		void generateReadme();
		void installPackagesFromAddonsMakeFile();
		ofPackage installPackage(std::string key, std::string destinationPath = "");
		ofPackage installPackageById(std::string id, std::string checkout = "", std::string destinationPath = "");
		ofPackage installPackageByGithub(std::string github, std::string checkout = "", std::string destinationPath = "");
		ofPackage installPackageByUrl(std::string url, std::string checkout = "", std::string destinationPath = "");
		ofPackage maybeInstallPackage(ofJson package);
		void printInfo();
		void printPaths();
		void printManual();
		void printAvailablePackages();
		void printVersion();
		void searchPackageInDatabaseById(std::string id);
		ofJson searchPackageOnGithubByName(std::string name);
		ofJson searchPackageOnGithubByUser(std::string user);
		void updatePackagesDatabase();
		void installDependenciesFromAddonConfig(std::string path, std::string destination);

		std::string getOfPath();
		ofJson getPackageJson();
		ofJson getPackageManagerJson();
		ofJson getLocalConfigJson();
		ofJson getGlobalConfigJson();
		ofJson getVersionJson();
		std::string generateGithubUrl(std::string github);
		std::string extractRepositoryName(std::string cloneUrl);
		std::string getAbsolutePath(std::string path);
		std::pair<std::string, std::string> getPathAndName(std::string name);

		bool hasConfig(std::string path);
		bool hasReadme(std::string path);
		bool isGitRepository(std::string path);
		bool isGitUrl(std::string path);
		bool isGithubPair(std::string path);
		bool hasPackageFile(std::string path);
		bool hasAddonsMakeFile(std::string path);
		bool hasAddonsConfigFile(std::string path);

		std::string _cwdPath;
		ofJson _configJson;
};
