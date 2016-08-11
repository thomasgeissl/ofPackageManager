#pragma once

#include "ofMain.h"

class ofPackageManager {
	public:
		ofPackageManager(string cwdPath);
		void setCwdPath(string cwdPath);
		void setConfig(string configPath);
		string getOfPath();

		void addPackageToPackageFile(string path, bool all = false, bool global = false);
		void configurePackageManager(bool global = false);
		void doctor();
		void generateDatabaseEntryFile();
		void generateProject();
		void generateReadme();
		void initPackage();
		void installDependencies();
		void installDependenciesFromPackageFile();
		void installDependenciesFromAddonsMakeFile(string destinationPath);
		void installPackageById(string id, string checkout = "", bool addToPackageFileB = true, string destinationPath = "", bool global = false);
		void installPackageByGithub(string github, string checkout = "", bool addToPackageFileB = true, string destinationPath = "", bool global = false);
		void installPackageByUrl(string url, string checkout = "", bool addPackageToPackageFile = true, string destinationPath = "", bool global = false);
		void printManual();
		void printAvailablePackages();
		void printVersion();
		void removePackageFromPackageFile(string path, bool all = false, bool global = false);
		void searchPackageInDatabaseById(string id);
		void searchPackageOnGithubByName(string name);
		void searchPackageOnGithubByUser(string user);
		void updatePackagesDatabase();

		void installDependencies(string path, string destination);
		void installDependenciesFromPackageFile(string path, string destination);
		void installDependenciesFromAddonConfig(string path, string destination);
	protected:
	private:
		ofJson getPackageJson();
		ofJson getPackageManagerJson();
		ofJson getLocalConfigJson();
		ofJson getGlobalConfigJson();
		ofJson getVersionJson();
		string generateGithubUrl(string github);
		string extractRepositoryName(string cloneUrl);
		string getAbsolutePath(string path);

		bool hasConfig(string path);
		bool hasReadme(string path);
		bool isGitRepository(string path);
		bool hasPackageFile(string path);
		bool hasAddonsMakeFile(string path);
		bool hasAddonsConfigFile(string path);
	private:
		string _cwdPath;
		ofJson _configJson;
};
