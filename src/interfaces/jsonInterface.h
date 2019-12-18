#include "ofApp.h"

class jsonInterface
{
public:
    jsonInterface(ofPackageManager app) : _app(app)
    {
    }
    bool doesKeyExist(ofJson payload, std::string key)
    {
        auto search = payload.find(key);
        return search != payload.end();
    }
    bool exec(int argc, char **argv)
    {
        std::string args = "";
        for (auto i = 1; i < argc; i++)
        {
            args += argv[i];
        }
        if (ofJson::accept(args))
        {
            auto request = ofJson::parse(args);
            auto payload = request["payload"];
            auto type = request["type"].get<std::string>();
            if (doesKeyExist(payload, "config"))
            {
                _app.setConfig(payload["config"]);
            }
            else
            {
                ofLogWarning("ofPackageManager") << "missing config property";
            }
            if (doesKeyExist(payload, "cwd"))
            {
                _app.setCwdPath(payload["cwd"].get<std::string>());
            }

            if (type == "INSTALL")
            {
                _app.installPackagesFromAddonsMakeFile();
            }
            if (type == "INSTALLPACKAGE")
            {
                std::string destinationPath = "";

                if (doesKeyExist(payload, "destinationPath"))
                {
                    destinationPath = payload["destinationPath"].get<std::string>();
                }
                _app.installPackage(payload["key"].get<std::string>(), destinationPath);
            }
            if (type == "INSTALLPACKAGEBYID")
            {
                std::string destinationPath = "";
                std::string checkout = "";
                if (doesKeyExist(payload, "destinationPath"))
                {
                    destinationPath = payload["destinationPath"].get<std::string>();
                }
                if (doesKeyExist(payload, "checkout"))
                {
                    checkout = payload["checkout"].get<std::string>();
                }
                _app.installPackageById(payload["id"].get<std::string>(), checkout, destinationPath);
            }
            if (type == "INSTALLPACKAGEBYGITHUB")
            {
                std::string destinationPath = "";
                std::string checkout = "";
                if (doesKeyExist(payload, "destinationPath"))
                {
                    destinationPath = payload["destinationPath"].get<std::string>();
                }
                if (doesKeyExist(payload, "checkout"))
                {
                    checkout = payload["checkout"].get<std::string>();
                }
                _app.installPackageById(payload["github"].get<std::string>(), checkout, destinationPath);
            }
            if (type == "INSTALLPACKAGEBYURL")
            {
                std::string destinationPath = "";
                std::string checkout = "";
                if (doesKeyExist(payload, "destinationPath"))
                {
                    destinationPath = payload["destinationPath"].get<std::string>();
                }
                if (doesKeyExist(payload, "checkout"))
                {
                    checkout = payload["checkout"].get<std::string>();
                }
                _app.installPackageById(payload["url"].get<std::string>(), checkout, destinationPath);
            }
            return true;
        }
        return false;
    }
    ofPackageManager _app;
};