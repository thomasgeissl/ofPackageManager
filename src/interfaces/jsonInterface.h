#include "ofApp.h"

#define TYPE_INSTALLPACKAGEBYID "INSTALLPACKAGEBYID"
#define TYPE_INSTALLPACKAGEBYGITHUB "INSTALLPACKAGEBYGITHUB"
#define TYPE_INSTALLPACKAGEBYURL "INSTALLPACKAGEBYURL"
#define TYPE_ADDPACKAGETOADDONSMAKEFILE "ADDPACKAGETOADDONSMAKEFILE"
#define TYPE_GETVERSION "GETVERSION"

class jsonInterface
{
public:
    jsonInterface(ofPackageManager app) : _app(app)
    {
        _app.setSilent(true);
    }
    bool doesKeyExist(ofJson payload, std::string key)
    {
        auto search = payload.find(key);
        return search != payload.end();
    }
    bool accept(int argc, char **argv)
    {
        std::string args = "";
        for (auto i = 1; i < argc; i++)
        {
            args += argv[i];
        }
        return ofJson::accept(args);
    }
    ofJson exec(int argc, char **argv)
    {
        std::string args = "";
        for (auto i = 1; i < argc; i++)
        {
            args += argv[i];
        }
        if (ofJson::accept(args))
        {
            // response object
            ofJson result;
            result["success"] = false;
            result["payload"] = ofJson::object();

            // request object
            auto request = ofJson::parse(args);
            auto payload = request["payload"];
            auto type = request["type"].get<std::string>();

            if (doesKeyExist(payload, "config"))
            {
                _app.setConfig(payload["config"]);
            }
            else
            {
                // ofLogWarning("ofPackageManager") << "missing config property";
            }
            if (doesKeyExist(payload, "cwd"))
            {
                _app.setCwdPath(payload["cwd"].get<std::string>());
            }

            if (type == "INSTALL")
            {
                if (_app.installPackagesFromAddonsMakeFile())
                {
                    result["success"] = true;
                }
                else
                {
                    result["success"] = false;
                }
            }
            else if (type == "INSTALLPACKAGE")
            {
                std::string destinationPath = "";

                if (doesKeyExist(payload, "destinationPath"))
                {
                    destinationPath = payload["destinationPath"].get<std::string>();
                }
                auto p = _app.installPackage(payload["key"].get<std::string>(), destinationPath);
                if (p.isEmpty())
                {
                    result["success"] = false;
                }
                else
                {
                    result["success"] = true;
                    result["payload"]["url"] = p._url;
                    result["payload"]["checkout"] = p._checkout;
                    result["payload"]["path"] = p._path;
                }
            }
            else if (type == TYPE_INSTALLPACKAGEBYID)
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
                auto p = _app.installPackageById(payload["id"].get<std::string>(), checkout, destinationPath);
                if (p.isEmpty())
                {
                    result["success"] = false;
                }
                else
                {
                    result["success"] = true;
                    result["payload"]["url"] = p._url;
                    result["payload"]["checkout"] = p._checkout;
                    result["payload"]["path"] = p._path;
                }
            }
            else if (type == TYPE_INSTALLPACKAGEBYID)
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
                auto p = _app.installPackageById(payload["github"].get<std::string>(), checkout, destinationPath);
                if (p.isEmpty())
                {
                    result["success"] = false;
                }
                else
                {
                    result["success"] = true;
                    result["payload"]["url"] = p._url;
                    result["payload"]["checkout"] = p._checkout;
                    result["payload"]["path"] = p._path;
                }
            }
            else if (type == TYPE_INSTALLPACKAGEBYURL)
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
                auto p = _app.installPackageById(payload["url"].get<std::string>(), checkout, destinationPath);
                if (p.isEmpty())
                {
                    result["success"] = false;
                }
                else
                {
                    result["success"] = true;
                    result["payload"]["url"] = p._url;
                    result["payload"]["checkout"] = p._checkout;
                    result["payload"]["path"] = p._path;
                }
            }
            else if (type == TYPE_ADDPACKAGETOADDONSMAKEFILE)
            {
                auto p = ofPackage(
                    payload["path"].get<std::string>(),
                    payload["url"].get<std::string>(),
                    payload["checkout"].get<std::string>());
                if (_app.addPackageToAddonsMakeFile(p))
                {
                    result["success"] = true;
                }
                else
                {
                    result["success"] = false;
                }
            }
            else if (type == TYPE_GETVERSION)
            {
                auto v = _app.getVersion();
                result["success"] = true;
                result["payload"]["major"] = v._major;
                result["payload"]["minor"] = v._minor;
                result["payload"]["patch"] = v._patch;
            }
            return result;
        }
        ofJson result;
        result["success"] = false;
        return result;
    }
    ofPackageManager _app;
};