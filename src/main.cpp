#include "ofMain.h"
#include "ofxUnitTests.h"
#include "ofApp.h"

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

bool doesKeyExist(ofJson payload, std::string key)
{
	auto search = payload.find(key);
	return search != payload.end();
}
int main(int argc, char **argv)
{
	ofInit();

#if defined(TARGET_OSX) || defined(TARGET_LINUX)
	std::shared_ptr<ofColorsLoggerChannel> logger{
		new ofColorsLoggerChannel};
	ofSetLoggerChannel(logger);
#endif
	//    ofSetLogLevel(OF_LOG_VERBOSE);
	filesystem::path cwdPath = filesystem::current_path();

	ofPackageManager app(cwdPath.string());
	std::string args = "";
	for (auto i = 1; i < argc; i++)
	{
		args += argv[i];
	}

	if (ofJson::accept(args))
	{
		auto request = ofJson::parse(args);
		auto type = request["type"].get<std::string>();
		if (type == "INSTALL")
		{
			auto payload = request["payload"];
			if (doesKeyExist(payload, "config"))
			{
				app.setConfig(payload["config"]);
			}
			app.installPackagesFromAddonsMakeFile();
		}
		if (type == "INSTALLPACKAGE")
		{
			auto payload = request["payload"];
			std::string destinationPath = "";
			if (doesKeyExist(payload, "config"))
			{
				app.setConfig(payload["config"]);
			}
			if (doesKeyExist(payload, "destinationPath"))
			{
				destinationPath = payload["destinationPath"].get<std::string>();
			}
			app.installPackage(payload["key"].get<std::string>(), destinationPath);
		}
		if (type == "INSTALLPACKAGEBYID")
		{
			auto payload = request["payload"];
			std::string destinationPath = "";
			std::string checkout = "";
			if (doesKeyExist(payload, "config"))
			{
				app.setConfig(payload["config"]);
			}
			if (doesKeyExist(payload, "destinationPath"))
			{
				destinationPath = payload["destinationPath"].get<std::string>();
			}
			if (doesKeyExist(payload, "checkout"))
			{
				checkout = payload["checkout"].get<std::string>();
			}
			app.installPackageById(payload["id"].get<std::string>(), checkout, destinationPath);
		}
		if (type == "INSTALLPACKAGEBYGITHUB")
		{
			auto payload = request["payload"];
			std::string destinationPath = "";
			std::string checkout = "";
			if (doesKeyExist(payload, "config"))
			{
				app.setConfig(payload["config"]);
			}
			if (doesKeyExist(payload, "destinationPath"))
			{
				destinationPath = payload["destinationPath"].get<std::string>();
			}
			if (doesKeyExist(payload, "checkout"))
			{
				checkout = payload["checkout"].get<std::string>();
			}
			app.installPackageById(payload["github"].get<std::string>(), checkout, destinationPath);
		}
		if (type == "INSTALLPACKAGEBYURL")
		{
			auto payload = request["payload"];
			std::string destinationPath = "";
			std::string checkout = "";
			if (doesKeyExist(payload, "config"))
			{
				app.setConfig(payload["config"]);
			}
			if (doesKeyExist(payload, "destinationPath"))
			{
				destinationPath = payload["destinationPath"].get<std::string>();
			}
			if (doesKeyExist(payload, "checkout"))
			{
				checkout = payload["checkout"].get<std::string>();
			}
			app.installPackageById(payload["url"].get<std::string>(), checkout, destinationPath);
		}
		// {
		// 	type: type,
		// 	payload: {
		// 		config
		// 	}
		// }
	}
	else
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
				app.addPackageToAddonsMakeFile(argv[2]);
			}
			else if (argc == 4)
			{
				string option = argv[2];
				if (option == "-A" || option == "--all")
				{
					app.addPackagesToAddonsMakeFile(argv[3]);
				}
			}
			else
			{
				ofLogError("ofPackageManager") << "invalid syntax";
			}
		}
		else if (task == "config")
		{
			if (argc == 2)
			{
				app.configure();
			}
			if (argc == 3)
			{
				std::string option = argv[2];
				if (option == "-g" || option == "--global")
				{
					app.configure(true);
				}
			}
		}
		else if (task == "doctor")
		{
			app.doctor();
		}
		else if (task == "update")
		{
			app.updatePackagesDatabase();
		}
		else if (task == "database")
		{
			app.generateDatabaseEntryFile();
		}
		else if (task == "help" || task == "man")
		{
			app.printManual();
		}
		else if (task == "info")
		{
			app.printInfo();
		}
		else if (task == "version")
		{
			app.printVersion();
		}
		else if (task == "install")
		{
			if (argc == 2)
			{
				app.installPackagesFromAddonsMakeFile();
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
						destinationPath = ofFilePath::join(app.getOfPath(), "addons");
						global = true;
					}
					package = argv[3];
				}
				else
				{
					printInputError(argc, argv);
					app.printManual();
					return -1;
				}

				auto installedPackage = app.installPackage(package, destinationPath);
				// TODO: move to app
				if (!global && !installedPackage._path.empty())
				{
					app.addPackageToAddonsMakeFile(installedPackage);
				}
			}
		}
		else if (task == "search")
		{
			if (argc == 3)
			{
				app.searchPackageInDatabaseById(argv[2]);
			}
			else if (argc == 4)
			{
				std::string subtask = argv[2];
				if (subtask == "github")
				{
					app.searchPackageOnGithubByName(argv[3]);
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
						app.searchPackageOnGithubByUser(argv[4]);
					}
				}
			}
			else
			{
				printInputError(argc, argv);
				app.printManual();
				return -1;
			}
		}
		else
		{
			ofLogError("ofPackageManager") << "unkown task" << task;
			app.printManual();
		}

		ofLogNotice() << "Thanks for using ofPackageManager. If you find a bug then please report it on the github issue tracker (https://github.com/thomasgeissl/ofPackageManager/issues). See you soon.";
	}

	return 0;
}