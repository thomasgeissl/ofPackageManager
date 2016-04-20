#include "ofMain.h"
#include "ofxUnitTests.h"
#include "ofApp.h"

bool isCloneUrl(string url)
{
    ofLogNotice("clone")<<url.substr(0,8);
    return ((url.substr(0,8) == "https://" || url.substr(0,7) == "http://") && url.substr(url.size() - 4, url.size()) == ".git");
}
bool isGithub(string github)
{
    return ofSplitString(github, "/").size() == 2;
}

int main(int argc, char** argv)
{
#if defined(TARGET_OSX) || defined(TARGET_LINUX)
    std::shared_ptr<ofColorsLoggerChannel> logger{new ofColorsLoggerChannel};
    ofSetLoggerChannel(logger);
#endif
//    ofSetLogLevel(OF_LOG_VERBOSE);
    char* cwdPath;
    char buff[PATH_MAX + 1];
    cwdPath = getcwd( buff, PATH_MAX + 1 );
    ofPackageManager app(cwdPath);

    string task;
    if(argc > 1)
    {
        task = argv[1];
        ofLogVerbose("main")<<argc;
    }
    else
    {
        app.searchPackageInDatabaseById("ofxHTTP");
//        app.printManual();
    }

    if(task == "add")
    {
        if(argc == 3)
        {
            app.addPackageToPackageFile(argv[2]);
        }
        else if(argc == 4)
        {
            string option = argv[2];
            if(option == "-A" || option == "--all")
            {
                app.addPackageToPackageFile(argv[3], true);
            }
        }
        else
        {
            ofLogError("main")<<"invalid syntax";
        }
    }
    else if(task == "config")
    {
        if(argc == 2)
        {
            app.configurePackageManager();
        }
        if(argc == 3)
        {
            string option = argv[2];
            if(option == "-g" || option == "--global")
            {
                app.configurePackageManager(true);
            }
        }

    }
    else if(task == "doctor")
    {
        app.doctor();
    }
    else if(task == "generate")
    {

        string subtask = "project";

        if(argc == 3)
        {
            subtask = argv[2];
        }
        else if(argc > 3)
        {
            ofLogError("TODO")<<" error counter"<<argc;
            return -1;
        }
        if(subtask == "project")
        {
            app.generateProject();
        }
        else if(subtask == "readme")
        {
            app.generateReadme();
        }
        else if(subtask == "database")
        {
            app.generateDatabaseEntryFile();
        }
        else
        {

        }
    }
    else if(task == "help")
    {
        app.printManual();
    }
    else if(task == "init")
    {
        app.initPackage();
    }
    else if(task == "install")
    {
        if(argc == 2)
        {
            app.installDependencies();
        }
        else if(argc >= 3)
        {
            string package;
            string destinationPath = "local_addons";
            bool global = false;
            if(argc == 3)
            {
                package = argv[2];

            }
            else if(argc == 4)
            {
                string option = argv[2];
                if(option == "-g" || option == "--global")
                {
                    destinationPath = ofFilePath::join(app.getOfPath(),"addons");
                    global = true;
                }
                package = argv[3];
            }
            if(isCloneUrl(package))
            {
                app.installPackageByUrl(package, "", true, destinationPath, global);
            }
            else if(isGithub(package))
            {
                app.installPackageByGithub(package, "", true, destinationPath, global);
            }
            else
            {
                app.installPackageById(package, "", true, destinationPath, global);
            }
        }
    }
    else if(task == "print")
    {
        string subtask = "packages";
        if(argc == 3)
        {
            subtask = argv[2];
        }
        if(subtask == "packages")
        {
            app.printAvailablePackages();
        }
        else if(subtask == "manual" || subtask == "man" || subtask == "help")
        {
            app.printManual();
        }
        else if(subtask == "version")
        {
            app.printVersion();
        }

    }
    else if(task == "search")
    {
        if(argc == 3)
        {
            app.searchPackageInDatabaseById(argv[2]);
        }
        else if(argc == 4)
        {
            string subtask = argv[2];
            if(subtask == "github")
            {
                app.searchPackageOnGithubByName(argv[3]);
            }
        }
        else if(argc == 5)
        {
            string subtask = argv[2];
            if(subtask == "github")
            {
                string option = argv[3];
                if(option == "-u" || option=="--user")
                {
                    app.searchPackageOnGithubByUser(argv[4]);
                }
            }
        }
    }

    else if(task == "update")
    {
        app.updatePackagesDatabase();
    }
    else
    {
        ofLogError("ofPackageManager")<<"unkown task"<<task;
        app.printManual();
    }
    return 0;
}
