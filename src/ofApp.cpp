#include "ofApp.h"

#include "ofxCommandLineUtils.h"
#include "ofxHTTP.h"
//#include "ofxGit.h"

ofPackageManager::ofPackageManager(string cwdPath) :
    _cwdPath(cwdPath),
    _configJson(getPackageManagerJson())
{
    ofLog::setAutoSpace(true);
}
void ofPackageManager::setCwdPath(string cwdPath)
{
    _cwdPath = cwdPath;
}

void ofPackageManager::setConfig(string configPath)
{


}

void ofPackageManager::addPackageToPackageFile(string path, bool all, bool global)
{
    path = getAbsolutePath(path);
    ofDirectory directory(path);
    directory.listDir();
    if(all)
    {
        for(auto file : directory.getFiles())
        {
            if(file.isDirectory())
            {
                if(isGitRepository(file.getAbsolutePath()))
                {
                    addPackageToPackageFile(file.getAbsolutePath(), false, global);
                }
                else
                {

                }
            }
        }
    }
    else
    {
        if(isGitRepository(path))
        {
            ofJson packageJson;
            ofFile packageFile;
            packageFile.open(ofFilePath::join(_cwdPath, "ofPackage.json"), ofFile::ReadWrite);
            packageJson << packageFile;
            ofJson dependency;

            //TODO: use libgit instead of system
            string urlResult = ofSystem("cd "+directory.getAbsolutePath()+" && git config --get remote.origin.url && cd ..");
            dependency["url"] = urlResult.substr(0,urlResult.size()-1);
            string checkoutResult = ofSystem("cd "+directory.getAbsolutePath()+" && git rev-parse HEAD && cd ..");
            dependency["checkout"] = checkoutResult.substr(0,checkoutResult.size()-1);
            if(global)
            {
                dependency["path"] = ofFilePath::getEnclosingDirectory(ofFilePath::makeRelative(_configJson["ofPath"], path), false);
            }
            else
            {
                dependency["path"] = ofFilePath::getEnclosingDirectory(ofFilePath::makeRelative(_cwdPath, path), false);
            }

            bool alreadyAdded = false;
            string globalOrLocal = "local";
            if(global) globalOrLocal = "global";
            for(int i = 0; i < packageJson["dependencies"][globalOrLocal].size(); i++)
            {
                if((packageJson["dependencies"][globalOrLocal][i]["path"] == dependency["path"]) && (packageJson["dependencies"][globalOrLocal][i]["url"] == dependency["url"]))
                {
//                    ofLogNotice("add")<<"package already added as a dependency"<<"TODO"<<"update commit hash";
                    packageJson["dependencies"][globalOrLocal][i]["checkout"] = dependency["checkout"];
                    alreadyAdded = true;
                }
            }
            if(!alreadyAdded)
            {
                packageJson["dependencies"][globalOrLocal].push_back(dependency);
            }

            packageFile.open(ofFilePath::join(_cwdPath, "ofPackage.json"), ofFile::ReadWrite);
            packageFile.clear();
            packageFile << packageJson.dump(4);
            packageFile.close();

        }
        else
        {
            ofLogError("add")<<"Directory"<<path<<"is not a valid git repository";
        }
    }

}

void ofPackageManager::configurePackageManager(bool globally)
{
    auto configPath = ofFilePath::join(_cwdPath, "ofPackageManager.json");
    string relativeOrAbsolute = "relative";
    if(globally)
    {
        configPath = ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json");
        relativeOrAbsolute = "absolute";
    }
    ofFile configFile(configPath);
    if(configFile.exists())
    {
        ofLogWarning("config")<<"Config file already exits.";
        if(!getBoolAnswer("Do you want to override it?"))
        {
            return;
        }
    }

    ofJson configJson;
    configJson["ofPath"] = getStringAnswer(relativeOrAbsolute+" path to openFrameworks?", ofFilePath::getAbsolutePath(getAbsolutePath("../../.."),false));;
    configJson["pgPath"] = getStringAnswer(relativeOrAbsolute+" path to the executable of projectGenerator?", ofFilePath::join(configJson["ofPath"], "apps/projectGenerator/commandLine/bin/projectGenerator.app/Contents/MacOS/projectGenerator"));
    configJson["packagesPath"] = getStringAnswer("absolute path to packages directory?", ofToDataPath("ofPackages", true));
    configJson["localAddonsPath"] = getStringAnswer("local addons directory?", "local_addons");


    configFile.create();
    configFile.open(configPath, ofFile::WriteOnly);
    configFile << configJson.dump(4);
    configFile.close();
}

void ofPackageManager::doctor()
{
    ofLogError("TODO")<<"doctor:"<<"check if there is a newer ofPackageManager version and check if there is a newer version ofPackages";
    printVersion();
}

void ofPackageManager::generateDatabaseEntryFile()
{
    ofJson dataBaseEntryJson;

    if(hasPackageFile(getAbsolutePath(_cwdPath)))
    {
        ofFile packageFile(getAbsolutePath(_cwdPath));
        ofJson packageJson;
        packageJson << packageFile;

        ofLogNotice("marker");
        dataBaseEntryJson["name"] = packageJson["name"];
        dataBaseEntryJson["author"] = packageJson["author"];
        dataBaseEntryJson["website"] = packageJson["website"];
        dataBaseEntryJson["cloneUrl"] = packageJson["cloneUrl"];
        dataBaseEntryJson["license"] = packageJson["license"];
        dataBaseEntryJson["type"] = packageJson["type"];

    }
    else
    {
        dataBaseEntryJson["name"] = getStringAnswer("package name?");
        dataBaseEntryJson["author"] = getStringAnswer("author?");
        dataBaseEntryJson["website"] = getStringAnswer("website?");
        dataBaseEntryJson["cloneUrl"] = getStringAnswer("cloneUrl?");
        dataBaseEntryJson["license"] = getStringAnswer("license?");
        dataBaseEntryJson["type"] = getOptionAnswer("type", {"app", "addon"});


    }

    string name = dataBaseEntryJson["name"];
    ofFile dataBaseEntryFile(ofFilePath::addTrailingSlash(getAbsolutePath(_cwdPath))+name+".json", ofFile::ReadWrite);
    if(!dataBaseEntryFile.exists())
    {
        dataBaseEntryFile.create();
    }
    dataBaseEntryFile << dataBaseEntryJson.dump(4);
}

void ofPackageManager::initPackage()
{
    auto packagePath = getAbsolutePath("ofPackage.json");
    ofFile packageFile(packagePath);
    ofJson packageJson;

    if(packageFile.exists())
    {
        ofLogWarning("init")<<"This package is already initialised.";
        if(!getBoolAnswer("Do you want to reinitialise the package?"))
        {
            return;
        }
    }
    else
    {
        ofFile defaultPackageFile(ofToDataPath("ofPackage.json"));
        defaultPackageFile.copyTo(packageFile.getAbsolutePath());
        defaultPackageFile.close();
    }

    packageFile.open(packageFile.getAbsolutePath(), ofFile::ReadWrite);
    packageJson << packageFile;

    packageJson["name"] = getStringAnswer("package name?", packageJson["name"]);
    packageJson["author"] = getStringAnswer("author?", packageJson["author"]);
    packageJson["version"] = getStringAnswer("version?", packageJson["version"]);
    packageJson["website"] = getStringAnswer("website?", packageJson["website"]);
    packageJson["cloneUrl"] = getStringAnswer("cloneUrl?", packageJson["cloneUrl"]);
    packageJson["license"] = getStringAnswer("license?", packageJson["license"]);
    packageJson["visible"] = getBoolAnswer("visible?", packageJson["visible"]);
    packageJson["type"] = getOptionAnswer("type", {"app", "addon"});

    packageJson["dependencies"]["global"] = ofJson::array();
    packageJson["dependencies"]["local"] = ofJson::array();



    packageFile.open(packagePath, ofFile::WriteOnly);
    packageFile << packageJson.dump(4);
    packageFile.close();


    ofDirectory cwd(_cwdPath);
    cwd.listDir();

    if(cwd.doesDirectoryExist(getAbsolutePath(_configJson["localAddonsPath"])))
    {
        ofLogNotice("init")<<"Found local addons";
        if(getBoolAnswer("Do you want to add it as a dependency?"))
        {
            addPackageToPackageFile(getAbsolutePath(_configJson["localAddonsPath"]), true);
        }
    }

    if(!hasReadme(_cwdPath) && getBoolAnswer("Do you want to generate a readme file?", true))
    {
        generateReadme();
    }
//    if(!hasConfig(_cwdPath) && getBoolAnswer("Do you want to configure the package manager locally?"), true)
//    {
//        configurePackageManager();
//    }
}


void ofPackageManager::searchPackageOnGithubByName(string name)
{
    ofLogNotice("ofPackageManager")<<"search";
    string url = "https://api.github.com/search/repositories?q="+name;

    ofx::HTTP::DefaultClient client;
    ofx::HTTP::Context context;
    ofx::HTTP::BaseResponse response;
    ofx::HTTP::GetRequest request(url, Poco::Net::HTTPMessage::HTTP_1_1);

     try
     {
         std::istream& responseStream = client.execute(request, response, context);
         ofJson resultJson;
         resultJson << responseStream;
         string outputString;
         outputString += "repositories containing " + name + ":\n";
         for(auto repo : resultJson["items"])
         {
             outputString += repo["full_name"];
             outputString += "\n";
         }
         ofLogNotice("search")<<outputString;

     }
     catch (const Poco::Exception& exc)
     {
         ofLogError("search") << "Got Exception " << exc.displayText() << " " << exc.code();
     }
}
void ofPackageManager::searchPackageOnGithubByUser(string user)
{
    std::string url = "https://api.github.com/users/"+user+"/repos";

    ofx::HTTP::DefaultClient client;
    ofx::HTTP::Context context;
    ofx::HTTP::BaseResponse response;
    ofx::HTTP::GetRequest request(url, Poco::Net::HTTPMessage::HTTP_1_1);

     try
     {
         std::istream& responseStream = client.execute(request, response, context);
         ofJson resultJson;
         resultJson << responseStream;
         string outputString;
         outputString += "repositories by " + user + ":\n";
         for(auto repo : resultJson)
         {
             outputString += repo["full_name"];
             outputString += "\n";
         }
         ofLogNotice("search")<<outputString;
     }
     catch (const Poco::Exception& exc)
     {
         ofLogError("search") << "Got Exception " << exc.displayText() << " " << exc.code();
    }
}
void ofPackageManager::installPackageByGithub(string github, string checkout, bool addToPackageFileB, string destinationPath, bool global)
{
    installPackageByUrl(generateGithubUrl(github), checkout, addToPackageFileB, destinationPath);
}

void ofPackageManager::installPackageByUrl(string url, string checkout, bool addToPackageFileB, string destinationPath, bool global)
{
    if(destinationPath.empty())
    {
        destinationPath = _configJson["localAddonsPath"];
    }
    destinationPath = getAbsolutePath(destinationPath);
    auto name = ofSplitString(url, "/").back(); name = name.substr(0, name.size()-4);

    ofDirectory destinationDirectory(destinationPath);
    if(!destinationDirectory.exists())
    {
        destinationDirectory.create();
    }

    ofSystem("cd "+destinationPath+" && git clone "+url);
    if(!checkout.empty())
    {
        ofSystem("cd "+ofFilePath::join(destinationPath, name)+" && git checkout "+checkout);
    }

    if(hasPackageFile(ofFilePath::join(destinationPath, name)))
    {
        installDependenciesFromPackageFile(ofFilePath::join(destinationPath, name), destinationPath);
    }
    else if(hasAddonsConfigFile(ofFilePath::join(destinationPath, name)))
    {
        installDependenciesFromAddonConfig(ofFilePath::join(destinationPath, name), destinationPath);
    }

    if(addToPackageFileB)
    {
        addPackageToPackageFile(ofFilePath::join(destinationPath, name),false, global);
    }
}

void ofPackageManager::removePackageFromPackageFile(string path, bool all, bool global)
{
    //TODO: remove package from the dependency list, ask if user wants to remove it from the filesystem too
}

void ofPackageManager::searchPackageById(string name)
{
    ofDirectory ofPackagesDirectory(ofToDataPath("ofPackages"));
    ofPackagesDirectory.listDir();
    bool foundPackage = false;
    for(auto file : ofPackagesDirectory.getFiles())
    {
        if(file.getExtension() == "json")
        {
            ofJson packageJson;
            file.open(file.getAbsolutePath());
            file >> packageJson;

            std::size_t found = ofToLower(file.getFileName()).find(ofToLower(name));
            if(found!=std::string::npos)
            {
                foundPackage = true;
                ofLogNotice("search")<<packageJson["github"];
            }
            else{
    //            TODO: check tags
    //            ofLogNotice("tags")<<packageJson["tags"];
            }
            file.close();
        }
    }
    if(!foundPackage)
    {
        ofLogError("search")<<"No package found";
        if(getBoolAnswer("Do you want to search on github?"))
        {
            searchPackageOnGithubByName(name);
        }
    }


}

void ofPackageManager::generateProject()
{
    ofLogWarning("TODO")<<"generate project";
    string pgPath = _configJson["pgPath"];
    string ofPath = _configJson["ofPath"];
    ofSystem(pgPath+" --ofPath "+ofPath+" --addons ofxMidi");
}

void ofPackageManager::generateReadme()
{
    ofFile readmeFile(getAbsolutePath("README.md"));
    if(hasReadme(_cwdPath))
    {
        ofLogWarning("generate")<<"readme already exists.";
        if(!getBoolAnswer("Do you want to override it?", false))
        {
            return;
        }
        else
        {
            readmeFile.open(readmeFile.getAbsolutePath(), ofFile::ReadWrite);
            readmeFile.clear();
        }
    }
    else
    {
        readmeFile.create();
    }

    readmeFile.open(getAbsolutePath("README.md"), ofFile::ReadWrite);
    {
//        TODO: remove surrounding ""
        ofJson packageJson = getPackageJson();
        readmeFile << "# "<< packageJson["name"] << std::endl;
        readmeFile << "## "<< "author" << std::endl;
        readmeFile << packageJson["author"] << std::endl;
        readmeFile << "## "<< "description" << std::endl;
        readmeFile << packageJson["description"] << std::endl;
        readmeFile << "## "<< "dependencies" << std::endl;
//        readmeFile << packageJson["dependencies"].dump(4) << std::endl;
        readmeFile << "## "<< "license" << std::endl;
        readmeFile << packageJson["license"] << std::endl;
        readmeFile << "## "<< "changelog" << std::endl;
    }
}
void ofPackageManager::printManual()
{
    ofLogWarning("TODO")<<"print manual";
}

void ofPackageManager::printAvailablePackages()
{
    ofLogError("TODO")<<"print available packages";
}

void ofPackageManager::installDependenciesFromPackageFile()
{
    if(hasConfig(_cwdPath))
    {
        ofLogNotice("install")<<"found local config file";
    }
    auto packagePath = ofFilePath::join(_cwdPath, "ofPackage.json");
    ofFile packageFile(packagePath);

    ofJson packageJson;
    if(packageFile.exists()){
        packageFile >> packageJson;
//        string ofPath = packageJson["of"]["path"];

        for (auto package : packageJson["dependencies"]["local"]) {
            string url;
            string github;
            string checkout;
            string path;

            if(!package["github"].empty())
            {
                github = package["github"];
            }
            if(!package["url"].empty())
            {
                url = package["url"];
            }
            if(!package["path"].empty())
            {
                path = package["path"];
            }
            if(!package["checkout"].empty())
            {
                checkout = package["checkout"];
            }
            if(!github.empty())
            {
                url = generateGithubUrl(github);
            }
          installPackageByUrl(url, checkout, false, path);
        }
//        TODO: global depenencies
    }
}
void ofPackageManager::installDependencies()
{
    if(hasPackageFile(_cwdPath))
    {
        installDependenciesFromPackageFile();
    }
    else if(hasAddonsMakeFile(_cwdPath))
    {
        installDependenciesFromAddonsMakeFile(_configJson["localAddonsPath"]);
    }
    else
    {
        ofLogError("install")<<"Could not find ofPackage.json or addons.make file";
    }

}
void ofPackageManager::installDependenciesFromAddonsMakeFile(string destinationPath)
{
    ofFile addonConfigFile(getAbsolutePath("addons.make"));
    if(addonConfigFile.exists())
    {
        ofBuffer fileBuffer = addonConfigFile.readToBuffer();
        for(auto line :fileBuffer.getLines())
        {
            installPackageById(ofTrim(line), "", false, destinationPath);
        }
    }
    else
    {
        ofLogError("install")<<"Package does not contain an addons.make file";
    }

}

void ofPackageManager::installPackageById(string id, string checkout, bool addToPackageFileB, string destinationPath, bool global)
{
    if(destinationPath.empty())
    {
        destinationPath = _configJson["localAddonsPath"];
    }
    destinationPath = getAbsolutePath(destinationPath);
    ofDirectory packagesDirectory("ofPackages");
    packagesDirectory.listDir();
    bool foundPackage = false;
    for(auto file : packagesDirectory.getFiles())
    {
        if(file.getFileName() == id+".json")
        {
            foundPackage = true;
            ofJson packageJson;
            file.open(file.getAbsolutePath());
            file >> packageJson;
            ofDirectory directory(ofFilePath::join(_cwdPath, ofFilePath::addTrailingSlash(destinationPath)+id));
            if(directory.exists())
            {
                directory.remove(true);
            }
//            ofxGit::Repository repository;
//            repository.open(ofFilePath::join(_cwdPath, "local_addons/"+id));
//            repository.clone("http://github.com/danomatika/ofxMidi.git");
            string url;
            if(!packageJson["cloneUrl"].empty())
            {
                url = packageJson["cloneUrl"];
            }
            else if(!packageJson["github"].empty())
            {
                url = generateGithubUrl(packageJson["github"]);
            }
            installPackageByUrl(url, checkout, addToPackageFileB, destinationPath, global);
        }
    }
    if(!foundPackage)
    {
        if(getBoolAnswer("Do you want to search on github?"))
        {
            searchPackageOnGithubByName(id);
        }
    }
}

void ofPackageManager::updatePackagesDatabase()
{
    ofSystem("cd "+ofToDataPath("ofPackages")+ " && git pull origin master && cd "+_cwdPath);
}

void ofPackageManager::printVersion()
{
    ofFile versionFile(ofToDataPath("version.json"));
    ofJson versionJson;
    versionJson << versionFile;
    versionFile.close();
    ofLog::setAutoSpace(false);
    ofLogNotice("version")<<versionJson["major"]<<"."<<versionJson["minor"]<<"."<<versionJson["patch"];
    ofLog::setAutoSpace(true);
}

void ofPackageManager::installDependencies(string path, string destination)
{

}

void ofPackageManager::installDependenciesFromPackageFile(string path, string destination)
{
    if(!ofFilePath::isAbsolute(path))
    {
        path = ofFilePath::join(_cwdPath, path);
    }
    ofFile packageFile(ofFilePath::join(path, "ofPackage.json"));
    if(packageFile.exists())
    {
        ofJson packageJson;
        packageJson << packageFile;
        ofLogNotice("install")<<"clone dependencies";
//        <<packageJson["depen"]
    }
    else
    {
        ofLogError("install")<<"Package does not contain a package file"<<path;
    }

}

void ofPackageManager::installDependenciesFromAddonConfig(string path, string destination)
{
    if(!ofFilePath::isAbsolute(path))
    {
        path = ofFilePath::join(_cwdPath, path);
    }
    ofFile addonConfigFile(ofFilePath::join(path, "addon_config.mk"));
    if(addonConfigFile.exists())
    {
        ofBuffer fileBuffer = addonConfigFile.readToBuffer();
        for(auto line :fileBuffer.getLines())
        {
            if(ofTrim(ofSplitString(ofTrim(line), "=").front()) == "ADDON_DEPENDENCIES")
            {
                for(auto id : ofSplitString(ofTrim(ofSplitString(ofTrim(line), "=").back()), " "))
                {
                    ofLogNotice("install")<<"TODO"<<id;
                    installPackageById(id, "", false, destination);
                }

            }
        }
    }
    else
    {
        ofLogError("install")<<"Package does not contain a addon_config file"<<path;
    }
}

ofJson ofPackageManager::getPackageJson()
{
    ofJson packageJson;
    ofFile packageFile(getAbsolutePath("ofPackage.json"));
    if(packageFile.exists())
    {
        packageJson << packageFile;
    }
    return packageJson;
}

ofJson ofPackageManager::getPackageManagerJson()
{
    ofJson packageManagerJson;
    ofFile packageFile(getAbsolutePath("ofPackageManager.json"));
    if(packageFile.exists())
    {
        packageManagerJson << packageFile;
    }
    else
    {
        packageFile.open(ofFilePath::join(ofFilePath::getUserHomeDir(), ".ofPackageManager.json"));
        if(packageFile.exists())
        {
            packageManagerJson << packageFile;
        }
    }
    return packageManagerJson;
}

ofJson ofPackageManager::getLocalConfigJson()
{
    ofJson packageManagerConfigJson;
    ofFile packageManagerConfigFile(getAbsolutePath("ofPackageManager.json"));
    if(packageManagerConfigFile.exists())
    {
        packageManagerConfigJson << packageManagerConfigFile;
    }
    return packageManagerConfigJson;
}
ofJson ofPackageManager::getGlobalConfigJson()
{
    ofJson packageManagerConfigJson;
    ofFile packageManagerConfigFile(ofFilePath::join(ofFilePath::getUserHomeDir(),".ofPackageManager.json"));
    if(packageManagerConfigFile.exists())
    {
        packageManagerConfigJson << packageManagerConfigFile;
    }
    return packageManagerConfigJson;
}

string ofPackageManager::generateGithubUrl(string github)
{
    return "http://github.com/"+github+".git";
}

string ofPackageManager::getAbsolutePath(string path)
{
    if(ofFilePath::isAbsolute(path))
    {
        return path;
    }
    return ofFilePath::join(_cwdPath, path);
}

bool ofPackageManager::hasConfig(string path){
    ofFile localConfig(ofFilePath::join(getAbsolutePath(path), "ofPackageMananager.json"));
    return localConfig.exists();
}

bool ofPackageManager::hasReadme(string path){
    path = getAbsolutePath(path);
    ofFile readmeFile(ofFilePath::join(path, "README.md"));
    return readmeFile.exists();
}

bool ofPackageManager::isGitRepository(string path)
{
    if(!ofFilePath::isAbsolute(path))
    {
        path = ofFilePath::join(_cwdPath, path);
    }
    ofDirectory gitDirectory(ofFilePath::join(path, ".git"));
    return gitDirectory.exists();
}

bool ofPackageManager::hasPackageFile(string path)
{
    if(!ofFilePath::isAbsolute(path))
    {
        path = ofFilePath::join(_cwdPath, path);
    }
    ofFile packageFile(ofFilePath::join(path, "ofPackage.json"));
    return packageFile.exists();
}

bool ofPackageManager::hasAddonsMakeFile(string path)
{
    if(!ofFilePath::isAbsolute(path))
    {
        path = ofFilePath::join(_cwdPath, path);
    }
    ofFile addonsMakeFile(ofFilePath::join(path, "addons.make"));
    return addonsMakeFile.exists();
}

bool ofPackageManager::hasAddonsConfigFile(string path)
{
    if(!ofFilePath::isAbsolute(path))
    {
        path = ofFilePath::join(_cwdPath, path);
    }
    ofFile addonConfigFile(ofFilePath::join(path, "addon_config.mk"));
    return addonConfigFile.exists();
}
