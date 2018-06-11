#pragma once

#include "ofMain.h"
#include "ofPackage.h"

class ofPackageManager {
	public:
		ofPackageManager(std::string cwdPath);
		void setCwdPath(std::string cwdPath);
		void setConfig(std::string configPath);
		std::string getOfPath();

		void addPackageToAddonsMakeFile(ofPackage package);
		void addPackageToAddonsMakeFile(std::string path);
		void addPackagesToAddonsMakeFile(std::string path);
		void configurePackageManager(bool global = false);
		void doctor();
		void generateDatabaseEntryFile();
		void generateProject();
		void generateReadme();
		void initPackage();
		void installPackagesFromAddonsMakeFile();
		ofPackage installPackageById(std::string id, std::string checkout = "", std::string destinationPath = "");
		ofPackage installPackageByGithub(std::string github, std::string checkout = "", std::string destinationPath = "");
		ofPackage installPackageByUrl(std::string url, std::string checkout = "", std::string destinationPath = "");
		void printManual();
		void printAvailablePackages();
		void printVersion();
		void searchPackageInDatabaseById(std::string id);
		void searchPackageOnGithubByName(std::string name);
		void searchPackageOnGithubByUser(std::string user);
		void updatePackagesDatabase();

		void installDependenciesFromAddonConfig(std::string path, std::string destination);

	protected:
	private:
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

	private:
		std::string _cwdPath;
		ofJson _configJson;
};
