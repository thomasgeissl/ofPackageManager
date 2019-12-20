#include "ofApp.h"

class cli
{
public:
    cli(ofPackageManager app) : _app(app)
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
    void printManual()
    {
        cout << "ofPackageManager version " << OFAPP_MAJOR_VERSION << "." << OFAPP_MINOR_VERSION << "." << OFAPP_PATCH_VERSION << endl
             << endl;

        cout << "Configuration" << endl;
        cout << "The package manager needs to know where to find the openFrameworks installation and the packages database. It can be configured globally or locally. If there is no local (per project) config, the package manager will use the global configuration." << endl;
        cout << "* Configure the package manager globally after the installation: ofPackageManager config -g" << endl;
        cout << "* Configure the package manager locally: ofPackageManager config" << endl
             << endl;

        cout << "Installation of packages" << endl;
        cout << "Packages can be installed via an id (if available in the database), github name or git url. It is also possible to specify the version by appending the commit hash or tag. Packages can be installed locally or globally. It is recommended to install them locally. The package manager will also install the packages' dependencies." << endl;
        cout << "The package manager will add git url and commit hash to the addons.make file." << endl;
        cout << "* Install by id: ofPackageManager install ofxMidi" << endl;
        cout << "* Install by github name: ofPackageManager install danomatika/ofxMidi" << endl;
        cout << "* Install by git url: ofPackageManager install https://github.com/danomatika/ofxMidi.git" << endl;
        cout << "* Install a specific version: ofPackageManager install ofxMidi@1.1.1" << endl
             << endl;

        cout << "Installation of a project's dependencies" << endl;
        cout << "It will clone all dependencies and check out the specified version in the addons.make file." << endl;
        cout << "* ofPackageManager install" << endl
             << endl;

        cout << "Adding manually cloned addons" << endl;
        cout << "It will clone all dependencies and check out the specified version in the addons.make file." << endl;
        cout << "* Add a single package: ofPackageManager add local_addons/ofxMidi" << endl;
        cout << "* Add all packages inside one directory: ofPackageManager add -A local_addons" << endl
             << endl;

        cout << "Further information can be found on github (https://github.com/thomasgeissl/ofPackageManager/)." << endl;
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
            auto v = _app.getVersion();
            ofLogNotice("version") << v._major << "." << v._minor << "." << v._patch;
            if (_app.isNewerVersionAvailable())
            {
                ofLogNotice("doctor") << "There is a new version of ofPackageManager available!";
                //   << "\nThe most recent version is " << mostRecentVersion._major << "." << mostRecentVersion._minor << "." << mostRecentVersion._patch;
            }
            else
            {
                ofLogNotice("doctor") << "You are up to date. Currently there is no newer version of ofPackageManager available";
            }
            // TODO: check version of ofPackages
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
            printManual();
        }
        else if (task == "info")
        {
            // _app.printInfo();
        }
        else if (task == "list")
        {
            auto availablePackages = _app.getAvailablePackages();
            // ofLogNotice() << availablePackages.dump(2);
            for (auto &package : availablePackages)
            {
                std::cout << package["name"] << endl;
                std::cout << "\t" << package["description"] << endl;
                std::cout << "\t" << package["author"] << endl;
                std::cout << "\t" << package["license"] << endl;
                std::cout << "\t" << package["cloneUrl"] << endl
                          << endl;
            }
        }
        else if (task == "version")
        {
            auto v = _app.getVersion();
            ofLogNotice("version") << v._major << "." << v._minor << "." << v._patch;
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
                    printManual();
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
                printManual();
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
            printManual();
        }
        ofLogNotice() << "Thanks for using ofPackageManager. If you find a bug then please report it on the github issue tracker (https://github.com/thomasgeissl/ofPackageManager/issues). See you soon.";
        return false;
    }
    ofPackageManager _app;
};