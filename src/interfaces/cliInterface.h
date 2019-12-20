#include "ofApp.h"

class cliInterface
{
public:
    cliInterface(ofPackageManager app) : _app(app)
    {
    }
    void printInputError(int argc, char **argv)
    {
        std::string input;
        for (auto i = 1; i < argc - 1; i++)
        {
            input += argv[i];
            input += "";
        }
        ofLogError("ofPackageManager") << "unkown command:" << input;
    }
    bool exec(int argc, char **argv)
    {
        std::string task;
        if (argc > 1)
        {
            task = argv[1];
        }

        if (task == "add")
        {
            if (argc == 3)
            {
                _app.addPackageToAddonsMakeFile(argv[2]);
            }
            else if (argc == 4)
            {
                string option = argv[2];
                if (option == "-A" || option == "--all")
                {
                    _app.addPackagesToAddonsMakeFile(argv[3]);
                }
            }
            else
            {
                ofLogError("ofPackageManager") << "invalid arguments";
            }
        }
        else if (task == "config")
        {
            if (argc == 2)
            {
                _app.configure();
            }
            if (argc == 3)
            {
                std::string option = argv[2];
                if (option == "-g" || option == "--global")
                {
                    _app.configure(true);
                }
            }
        }
        else if (task == "doctor")
        {
            _app.doctor();
        }
        else if (task == "update")
        {
            _app.updatePackagesDatabase();
        }
        else if (task == "database")
        {
            _app.generateDatabaseEntryFile();
        }
        else if (task == "help" || task == "man")
        {
            _app.printManual();
        }
        else if (task == "info")
        {
            _app.printInfo();
        }
        else if (task == "version")
        {
            _app.printVersion();
        }
        else if (task == "install")
        {
            if (!_app.isConfigured())
            {
                ofLogError() << "please run config task, before installing packages.";
                return false;
            }
            if (argc == 2)
            {
                _app.installPackagesFromAddonsMakeFile();
            }
            else if (argc >= 3)
            {
                std::string package;
                std::string destinationPath = "local_addons";
                bool global = false;
                if (argc == 3)
                {
                    package = argv[2];
                }
                else if (argc == 4)
                {
                    std::string option = argv[2];
                    if (option == "-g" || option == "--global")
                    {
                        destinationPath = ofFilePath::join(_app.getOfPath(), "addons");
                        global = true;
                    }
                    package = argv[3];
                }
                else
                {
                    printInputError(argc, argv);
                    _app.printManual();
                    return -1;
                }

                auto installedPackage = _app.installPackage(package, destinationPath);
                // TODO: move to _app
                if (!global && !installedPackage._path.empty())
                {
                    _app.addPackageToAddonsMakeFile(installedPackage);
                }
            }
        }
        else if (task == "search")
        {
            if (!_app.isConfigured())
            {
                ofLogError() << "please run config task, before installing packages.";
                return false;
            }
            if (argc == 3)
            {
                _app.searchPackageInDatabaseById(argv[2]);
            }
            else if (argc == 4)
            {
                std::string subtask = argv[2];
                if (subtask == "github")
                {
                    _app.searchPackageOnGithubByName(argv[3]);
                }
            }
            else if (argc == 5)
            {
                std::string subtask = argv[2];
                if (subtask == "github")
                {
                    string option = argv[3];
                    if (option == "-u" || option == "--user")
                    {
                        _app.searchPackageOnGithubByUser(argv[4]);
                    }
                }
            }
            else
            {
                printInputError(argc, argv);
                _app.printManual();
                return -1;
            }
        }
        else if (task == "generate")
        {
            if (!_app.isConfigured())
            {
                ofLogError() << "please run config task, before installing packages.";
                return false;
            }
            ofLogNotice() << "generate project";
        }
        else
        {
            ofLogError("ofPackageManager") << "unknown task" << task;
            _app.printManual();
        }
        ofLogNotice() << "Thanks for using ofPackageManager. If you find a bug then please report it on the github issue tracker (https://github.com/thomasgeissl/ofPackageManager/issues). See you soon.";
        return false;
    }
    ofPackageManager _app;
};