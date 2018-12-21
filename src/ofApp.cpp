#include "ofApp.h"
#include "ofxCommandLineUtils.h"

ofPackageManager::ofPackageManager(std::string cwdPath) :
	_cwdPath(cwdPath),
	_configJson(getPackageManagerJson()){
	ofLog::setAutoSpace(true);
}

void ofPackageManager::setCwdPath(std::string cwdPath){
	_cwdPath = cwdPath;
}


void ofPackageManager::addPackageToAddonsMakeFile(ofPackage package) {
	ofFile addonConfigFile(getAbsolutePath("addons.make"), ofFile::ReadOnly);
	if(!addonConfigFile.exists()){
		addonConfigFile.create();
	}
	auto stringToAdd = package._path+" #"+package._url+"@"+package._checkout;
	ofBuffer fileBuffer = addonConfigFile.readToBuffer();
	std::string content;
	auto foundPackage = false;
	for(auto line : fileBuffer.getLines()){
		auto words = ofSplitString(ofTrim(line), "#");
		// if path addon already listed in addon.make
		// then update it
		// else append a new line
		if (words[0] == package._path) {
			foundPackage = true;
		} else {
			content += line+"\n";
		}
		// ofLogNotice() << content;
	}
	if (!foundPackage) {
		content += stringToAdd;
	}
	fileBuffer.set(content.c_str(), content.size());
	addonConfigFile.close();
	ofFile newAddonsMakeFile(getAbsolutePath("addons.make"), ofFile::ReadWrite);
	if(newAddonsMakeFile.writeFromBuffer(fileBuffer)) {
		ofLogNotice()<<"successfully updated addons.make";
	} else {
		ofLogError()<<"Could not update addons.make";
	}
}

void ofPackageManager::addPackageToAddonsMakeFile(std::string path) {
	ofxGit::repository repo(path);
	auto url = repo.getRemoteUrl();
	auto checkout = repo.getCommitHash();
	ofLogNotice()<<path<<url<<checkout;
	addPackageToAddonsMakeFile(ofPackage(path, url, checkout));
}

void ofPackageManager::addPackagesToAddonsMakeFile(std::string path) {
	ofDirectory directory(ofFilePath::join(_cwdPath, path));
	if(!directory.exists()){
		ofLogError() << "directory does not exit";
		return;
	}
	directory.listDir();
	for(auto file : directory.getFiles()){
		if(file.isDirectory()){
			addPackageToAddonsMakeFile(ofFilePath::join(path, file.getFileName()));
		}
	}
}
void ofPackageManager::addPackagesToAddonsMakeFile(std::vector<std::string> paths) {
	for(auto path : paths){
		addPackageToAddonsMakeFile(ofFilePath::join(_cwdPath, path));
	}
}
void ofPackageManager::configurePackageManager(bool global){
	auto configPath = ofFilePath::join(_cwdPath, "ofPackageManager.json");
	std::string relativeOrAbsolute = "relative";
	if(global){
		configPath = ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json");
		relativeOrAbsolute = "absolute";
	}
	ofFile configFile(configPath);
	if(configFile.exists()){
		ofLogWarning("config") << "Config file already exits.";
		if(!getBoolAnswer("Do you want to override it?")){
			return;
		}
	}

	ofJson configJson;
	configJson["ofPath"] = getStringAnswer(relativeOrAbsolute + " path to openFrameworks?", ofFilePath::getAbsolutePath(getAbsolutePath("../../.."), false));
	// configJson["pgPath"] = getStringAnswer(relativeOrAbsolute + " path to the executable of projectGenerator?", ofFilePath::join(configJson["ofPath"], "apps/projectGenerator/commandLine/bin/projectGenerator.app/Contents/MacOS/projectGenerator"));
	configJson["packagesPath"] = getStringAnswer("absolute path to packages directory?", ofToDataPath("ofPackages", true));
	configJson["localAddonsPath"] = getStringAnswer("local addons directory?", "local_addons");

	configFile.create();
	configFile.open(configPath, ofFile::WriteOnly);
	configFile << configJson.dump(4);
	configFile.close();
}

void ofPackageManager::doctor(){
	printVersion();

	// check version of ofPackageManager
	ofHttpResponse request = ofLoadURL("https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/bin/data/version.json");
	ofJson mostRecentVersionJson;
	mostRecentVersionJson = ofJson::parse(request.data.getText());
	ofJson currentVersion = getVersionJson();
	if(
		mostRecentVersionJson["major"].get <int>() > currentVersion["major"].get <int>() ||
		mostRecentVersionJson["minor"].get <int>() > currentVersion["minor"].get <int>() ||
		mostRecentVersionJson["patch"].get <int>() > currentVersion["patch"].get <int>()
		){
		ofLogNotice("doctor") << "there is a new version of ofPackageManager available";
		ofLog::setAutoSpace(false);
		ofLogNotice("doctor") << "the most recent version is " << mostRecentVersionJson["major"] << "." << mostRecentVersionJson["minor"] << "." << mostRecentVersionJson["patch"];
		ofLog::setAutoSpace(true);
	}else{
		ofLogNotice("doctor") << "there is no new version of ofPackageManager available";
	}

	// check version of ofPackages
}

void ofPackageManager::generateDatabaseEntryFile(){
	ofJson dataBaseEntryJson;

	if (hasPackageFile(getAbsolutePath(_cwdPath))) {
		ofFile packageFile(getAbsolutePath(_cwdPath));
		ofJson packageJson;
		packageJson << packageFile;

		dataBaseEntryJson["name"] = packageJson["name"];
		dataBaseEntryJson["author"] = packageJson["author"];
		dataBaseEntryJson["url"] = packageJson["url"];
		dataBaseEntryJson["cloneUrl"] = packageJson["cloneUrl"];
		dataBaseEntryJson["license"] = packageJson["license"];
		dataBaseEntryJson["type"] = packageJson["type"];

	} else {
		dataBaseEntryJson["name"] = getStringAnswer("package name?");
		dataBaseEntryJson["author"] = getStringAnswer("author?");
		dataBaseEntryJson["url"] = getStringAnswer("url?");
		dataBaseEntryJson["cloneUrl"] = getStringAnswer("cloneUrl?");
		dataBaseEntryJson["license"] = getStringAnswer("license?");
		dataBaseEntryJson["type"] = getOptionAnswer("type", {"app", "addon"});
	}

	std::string name = dataBaseEntryJson["name"];
	ofFile dataBaseEntryFile(ofFilePath::addTrailingSlash(getAbsolutePath(_cwdPath)) + name + ".json", ofFile::ReadWrite);
	if(!dataBaseEntryFile.exists()){
		dataBaseEntryFile.create();
	}
	dataBaseEntryFile << dataBaseEntryJson.dump(4);
}

void ofPackageManager::initPackage(){
	auto packagePath = getAbsolutePath("ofPackage.json");
	ofFile packageFile(packagePath);
	ofJson packageJson;

	if (packageFile.exists()) {
		ofLogWarning("init") << "This package is already initialised.";
		if(!getBoolAnswer("Do you want to reinitialise the package?")){
			return;
		}
	}else{
		ofFile defaultPackageFile(ofToDataPath("ofPackage.json"));
		defaultPackageFile.copyTo(packageFile.getAbsolutePath());
		defaultPackageFile.close();
	}

	packageFile.open(packageFile.getAbsolutePath(), ofFile::ReadWrite);
	packageJson << packageFile;

	packageJson["name"] = getStringAnswer("package name?", ofFilePath::getBaseName(_cwdPath)); // packageJson["name"]);
	packageJson["author"] = getStringAnswer("author?", packageJson["author"]);
	packageJson["version"] = getStringAnswer("version?", packageJson["version"]);
	packageJson["url"] = getStringAnswer("url?", packageJson["url"]);
	packageJson["cloneUrl"] = getStringAnswer("cloneUrl?", packageJson["cloneUrl"]);
	packageJson["license"] = getStringAnswer("license?", packageJson["license"]);
	packageJson["visible"] = getBoolAnswer("visible?", packageJson["visible"]);
	packageJson["type"] = getOptionAnswer("type", {"app", "addon"});

	packageFile.open(packagePath, ofFile::WriteOnly);
	packageFile << packageJson.dump(4);
	packageFile.close();

	ofDirectory cwd(_cwdPath);
	cwd.listDir();

	if(!hasReadme(_cwdPath) && getBoolAnswer("Do you want to generate a readme file?", true)){
		generateReadme();
	}
	// if(!hasConfig(_cwdPath) && getBoolAnswer("Do you want to configure the package manager locally?"), true) {
	// 		configurePackageManager();
	// }
}


void ofPackageManager::searchPackageOnGithubByName(string name){
	std::string url = "https://api.github.com/search/repositories?q=" + name;
	ofHttpRequest request(url, "TODO: check name");
	request.headers["User-Agent"] = "ofPackageManager";
	ofURLFileLoader loader;
	auto response = loader.handleRequest(request);
	auto resultJson = ofJson::parse(response.data.getText());
	std::string outputString;
	outputString += "repositories containing " + name + ":\n";
	for(auto repo : resultJson["items"]){
		std::string name = repo["full_name"];
		outputString += name;
		outputString += "\n";
	}
	ofLogNotice("search") << outputString;
}

void ofPackageManager::searchPackageOnGithubByUser(std::string user){
	std::string url = "https://api.github.com/users/" + user + "/repos?per_page=100";
	ofHttpRequest request(url, "TODO: check name");
	request.headers["User-Agent"] = "ofPackageManager";
	ofURLFileLoader loader;
	auto response = loader.handleRequest(request);
	auto resultJson = ofJson::parse(response.data.getText());
	std::string outputString;
	outputString += "repositories by" + user + ":\n";
	for(auto repo : resultJson["items"]){
		std::string name = repo["full_name"];
		outputString += name;
		outputString += "\n";
	}
	ofLogNotice("search") << outputString;
}

ofPackage ofPackageManager::installPackageByGithub(std::string github, std::string checkout, std::string destinationPath){
	return installPackageByUrl(generateGithubUrl(github), checkout, destinationPath);
}

ofPackage ofPackageManager::installPackageByUrl(std::string url, std::string checkout, std::string destinationPath){
	if(destinationPath.empty()){
		destinationPath = _configJson["localAddonsPath"];
	}
	destinationPath = getAbsolutePath(destinationPath);
	auto name = ofSplitString(url, "/").back();
	name = name.substr(0, name.size() - 4);

	ofDirectory destinationDirectory(destinationPath);
	if(!destinationDirectory.exists()){
		destinationDirectory.create();
	}

	ofxGit::repository repo(ofFilePath::join(destinationPath, name));
	if(repo.clone(url)) {
		ofLogNotice("ofPackageManager") << "successfully cloned repo" << url << "via libgit";
	}
	if(checkout != "latest"){
		// checkout the version
		repo.checkout(checkout);
	} else {
		// get commit hash
		checkout = repo.getCommitHash();
	}

	if(hasAddonsConfigFile(ofFilePath::join(destinationPath, name))){
		installDependenciesFromAddonConfig(ofFilePath::join(destinationPath, name), destinationPath);
	}

	return ofPackage(ofFilePath::join(ofFilePath::makeRelative(_cwdPath, destinationPath), name), url, checkout);
}


void ofPackageManager::searchPackageInDatabaseById(std::string name){
	ofDirectory ofPackagesDirectory(ofToDataPath("ofPackages"));
	ofPackagesDirectory.listDir();
	auto foundPackage = false;
	for(auto file : ofPackagesDirectory.getFiles()){
		if(file.getExtension() == "json"){
			ofJson packageJson;
			file.open(file.getAbsolutePath());
			file >> packageJson;

			std::size_t found = ofToLower(file.getFileName()).find(ofToLower(name));
			if(found != std::string::npos){
				foundPackage = true;
				ofLogNotice("search") << packageJson["cloneUrl"];
			}else{
				//            TODO: check tags
				//            ofLogNotice("tags")<<packageJson["tags"];
			}
			file.close();
		}
	}
	if(!foundPackage){
		ofLogError("search") << "No package found";
		if(getBoolAnswer("Do you want to search on github?")){
			searchPackageOnGithubByName(name);
		}
	}
}

void ofPackageManager::generateProject() {
	std::string pgPath = _configJson["pgPath"];
	std::string ofPath = _configJson["ofPath"];
	std::string addonsList;
	auto packageJson = getPackageJson();

	vector <std::string> addonsVec;
	auto command = pgPath + " --ofPath=\"" + ofPath + "\" --addons=\"" + addonsList + "\" .";
	ofSystem(command);
}

void ofPackageManager::generateReadme(){
	ofFile readmeFile(getAbsolutePath("README.md"));
	if(hasReadme(_cwdPath)){
		ofLogWarning("generate") << "readme already exists.";
		if(!getBoolAnswer("Do you want to override it?", false)){
			return;
		}else{
			readmeFile.open(readmeFile.getAbsolutePath(), ofFile::ReadWrite);
			readmeFile.clear();
		}
	}else{
		readmeFile.create();
	}

	readmeFile.open(getAbsolutePath("README.md"), ofFile::ReadWrite);
	ofJson packageJson = getPackageJson();
	std::string name = packageJson["name"];
	std::string url = packageJson["url"];
	std::string author = packageJson["author"];
	std::string description = packageJson["description"];
	std::string license = packageJson["license"];

	readmeFile << "# " << "[" << name << "](" << url << ")" << std::endl;
	readmeFile << "## " << "description" << std::endl;
	readmeFile << description << std::endl;
	readmeFile << std::endl;

	readmeFile << "## " << "author" << std::endl;
	readmeFile << author << std::endl;
	readmeFile << std::endl;

	readmeFile << "## " << "dependencies" << std::endl;
	readmeFile << std::endl;

	readmeFile << "## " << "license" << std::endl;
	readmeFile << license << std::endl;
	readmeFile << std::endl;

	readmeFile << "## " << "changelog" << std::endl;
}
void ofPackageManager::printInfo(){
	ofLogNotice("ofPackageManager") << "info";
	printVersion();
	printPaths();
}
void ofPackageManager::printPaths(){
	ofLogNotice("path") << "TODO";
	ofLogNotice("global config path") << ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json");
}
void ofPackageManager::printManual(){
	ofLogWarning("TODO") << "print manual";
}

void ofPackageManager::printAvailablePackages(){
	ofDirectory ofPackagesDirectory(ofToDataPath("ofPackages"));
	ofPackagesDirectory.listDir();
	for(auto file : ofPackagesDirectory.getFiles()){
		if(file.getExtension() == "json"){
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

void ofPackageManager::installPackagesFromAddonsMakeFile(){
	ofFile addonConfigFile(getAbsolutePath("addons.make"));
	if(addonConfigFile.exists()){
		ofBuffer fileBuffer = addonConfigFile.readToBuffer();
		for(auto line :fileBuffer.getLines()){
			auto words = ofSplitString(ofTrim(line), "#");
			auto path = getPathAndName(words[0]).first;
			auto name = getPathAndName(words[0]).second;

			if (path.empty()) {
				path = ofFilePath::join(getOfPath(), "addons");
			}
			switch (words.size()) {
				case 1: {
					installPackageById(words[0],"latest", path);
					break;
				}
				case 2: {
					std::vector<std::string> packageParts = ofSplitString(words[1], "@");
					std::string checkout = "latest";
					std::string package = packageParts[0];
					if(packageParts.size() > 1){
						checkout = packageParts[1];
					}
					if(isGitUrl(words[1])) {
						installPackageByUrl(package, checkout, path);
					} else if ( isGithubPair(package) ) {
						auto url = "https://github.com/"+package+".git";
						installPackageByUrl(url, checkout, path);
					} else {
						installPackageById(package, checkout, path);
					}
					break;
				}
				default: break;
			}
		}
	}else{
		ofLogError("install") << "Package does not contain an addons.make file";
	}
}
ofPackage ofPackageManager::installPackage(std::string key, std::string destinationPath){
	auto parts = ofSplitString(key, "@");
	std::string checkout = "latest";
	if(parts.size() > 1){
		checkout = parts[1];
	}
	key = parts[0];

	if(isGitUrl(key)){
		return installPackageByUrl(key, checkout, destinationPath);
	}else if(isGithubPair(key)){
		return installPackageByGithub(key, checkout, destinationPath);
	}else{
		return installPackageById(key, checkout, destinationPath);
	}
}
ofPackage ofPackageManager::installPackageById(std::string id, std::string checkout, std::string destinationPath){
	ofLogNotice()<<"install package by id" << id;
	if(destinationPath.empty()){
		destinationPath = _configJson["localAddonsPath"];
	}
	destinationPath = getAbsolutePath(destinationPath);
	ofDirectory packagesDirectory("ofPackages");
	packagesDirectory.listDir();
	bool foundPackage = false;
	for(auto file : packagesDirectory.getFiles()){
		if(file.getFileName() == id + ".json"){
			foundPackage = true;
			ofJson packageJson;
			file.open(file.getAbsolutePath());
			file >> packageJson;
			ofDirectory directory(ofFilePath::join(_cwdPath, ofFilePath::addTrailingSlash(destinationPath) + id));
			if(directory.exists()){
				directory.remove(true);
			}
			std::string url;
			if(!packageJson["cloneUrl"].empty()){
				url = packageJson["cloneUrl"];
			}else if(!packageJson["github"].empty()){
				url = generateGithubUrl(packageJson["github"]);
			}
			return installPackageByUrl(url, checkout, destinationPath);
		}
	}
	if(!foundPackage){
		if(getBoolAnswer("Do you want to search on github?")){
			searchPackageOnGithubByName(id);
		}
	}
	return ofPackage("", "", "");
}

void ofPackageManager::updatePackagesDatabase(){
	ofSystem("cd " + ofToDataPath("ofPackages") + " && git pull origin master && cd " + _cwdPath);
}

void ofPackageManager::printVersion(){
	ofFile versionFile(ofToDataPath("version.json"));
	ofJson versionJson;
	versionJson << versionFile;
	versionFile.close();
	ofLog::setAutoSpace(false);
	ofLogNotice("version") << versionJson["major"] << "." << versionJson["minor"] << "." << versionJson["patch"];
	ofLog::setAutoSpace(true);
}

void ofPackageManager::installDependenciesFromAddonConfig(std::string path, std::string destination){
	if(!ofFilePath::isAbsolute(path)){
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile addonConfigFile(ofFilePath::join(path, "addon_config.mk"));
	if(addonConfigFile.exists()){
		ofBuffer fileBuffer = addonConfigFile.readToBuffer();
		for(auto line :fileBuffer.getLines()){
			if(ofTrim(ofSplitString(ofTrim(line), "=").front()) == "ADDON_DEPENDENCIES"){
				for(auto id : ofSplitString(ofTrim(ofSplitString(ofTrim(line), "=").back()), " ")){
					installPackage(ofTrim(id), destination);
				}
			}else if(ofTrim(ofSplitString(ofTrim(line), "+=").front()) == "ADDON_DEPENDENCIES"){
				auto key  = ofSplitString(ofTrim(line), "+=").back();
				installPackage(ofTrim(key), destination);
			}
		}
	}else{
		ofLogError("install") << "Package does not contain a addon_config file" << path;
	}
}

std::string ofPackageManager::getOfPath(){
	return _configJson["ofPath"];
}

ofJson ofPackageManager::getPackageJson(){
	ofJson packageJson;
	ofFile packageFile(getAbsolutePath("ofPackage.json"));
	if(packageFile.exists()){
		packageJson << packageFile;
	}
	return packageJson;
}

ofJson ofPackageManager::getPackageManagerJson(){
	ofJson packageManagerJson;
	ofFile packageFile(getAbsolutePath("ofPackageManager.json"));
	if(packageFile.exists()){
		packageManagerJson << packageFile;
	}else{
		packageFile.open(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json"));
		if(packageFile.exists()){
			packageManagerJson << packageFile;
		}
	}
	return packageManagerJson;
}

ofJson ofPackageManager::getLocalConfigJson(){
	ofJson packageManagerConfigJson;
	ofFile packageManagerConfigFile(getAbsolutePath("ofPackageManager.json"));
	if(packageManagerConfigFile.exists()){
		packageManagerConfigJson << packageManagerConfigFile;
	}
	return packageManagerConfigJson;
}

ofJson ofPackageManager::getGlobalConfigJson(){
	ofJson packageManagerConfigJson;
	ofFile packageManagerConfigFile(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json"));
	if(packageManagerConfigFile.exists()){
		packageManagerConfigJson << packageManagerConfigFile;
	}
	return packageManagerConfigJson;
}

ofJson ofPackageManager::getVersionJson(){
	ofFile versionFile(ofToDataPath("version.json"));
	ofJson versionJson;
	versionJson << versionFile;
	versionFile.close();
	return versionJson;
}

std::string ofPackageManager::generateGithubUrl(std::string github){
	return "http://github.com/" + github + ".git";
}

std::string ofPackageManager::extractRepositoryName(std::string cloneUrl){
	auto name = ofSplitString(cloneUrl, "/").back();
	name = name.substr(0, name.size() - 4);
	return name;
}

std::string ofPackageManager::getAbsolutePath(std::string path){
	if(ofFilePath::isAbsolute(path)){
		return path;
	}
	return ofFilePath::join(_cwdPath, path);
}

std::pair<std::string, std::string> ofPackageManager::getPathAndName(std::string name) {
	auto words = ofSplitString(name, "/");
	if (words.size() > 1) {
		auto path = words[0];
		for (auto i = 1; i < words.size() - 2; i++) {
			path+"/"+words[i];
		}
		return std::make_pair(path, words[words.size() - 1]);
	}
	return std::make_pair("", name);
}

bool ofPackageManager::hasConfig(std::string path){
	ofFile localConfig(ofFilePath::join(getAbsolutePath(path), "ofPackageMananager.json"));
	return localConfig.exists();
}

bool ofPackageManager::hasReadme(std::string path){
	path = getAbsolutePath(path);
	ofFile readmeFile(ofFilePath::join(path, "README.md"));
	return readmeFile.exists();
}

bool ofPackageManager::isGitRepository(std::string path){
	if(!ofFilePath::isAbsolute(path)){
		path = ofFilePath::join(_cwdPath, path);
	}
	ofDirectory gitDirectory(ofFilePath::join(path, ".git"));
	return gitDirectory.exists();
}

bool ofPackageManager::isGitUrl(std::string path){
	// TODO: check if string ends with .git
	return path.substr(0, 4) == "git@" || path.substr(0, 4) == "http";
}

bool ofPackageManager::isGithubPair(std::string path) {
	return ofSplitString(path, "/").size() > 1;
}

bool ofPackageManager::hasPackageFile(std::string path){
	if(!ofFilePath::isAbsolute(path)){
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile packageFile(ofFilePath::join(path, "ofPackage.json"));
	return packageFile.exists();
}

bool ofPackageManager::hasAddonsMakeFile(std::string path){
	if(!ofFilePath::isAbsolute(path)){
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile addonsMakeFile(ofFilePath::join(path, "addons.make"));
	return addonsMakeFile.exists();
}

bool ofPackageManager::hasAddonsConfigFile(std::string path){
	if(!ofFilePath::isAbsolute(path)){
		path = ofFilePath::join(_cwdPath, path);
	}
	ofFile addonConfigFile(ofFilePath::join(path, "addon_config.mk"));
	return addonConfigFile.exists();
}
