#include "ofApp.h"

#define TYPE_INSTALLPACKAGESFROMADDONSMAKEFILE "INSTALL"
#define TYPE_INSTALLPACKAGE "INSTALLPACKAGE"
#define TYPE_INSTALLPACKAGEBYID "INSTALLPACKAGEBYID"
#define TYPE_INSTALLPACKAGEBYGITHUB "INSTALLPACKAGEBYGITHUB"
#define TYPE_INSTALLPACKAGEBYURL "INSTALLPACKAGEBYURL"
#define TYPE_ADDPACKAGETOADDONSMAKEFILE "ADDPACKAGETOADDONSMAKEFILE"
#define TYPE_GETPACKAGESLISTEDINADDONSMAKE "GETPACKAGESLISTEDINADDONSMAKE"
#define TYPE_SEARCHPACKAGEINDATABASEBYID "SEARCHPACKAGEINDATABASEBYID"
#define TYPE_SEARCHPACKAGEONGITHUBBYNAME "SEARCHPACKAGEONGITHUBBYNAME"
#define TYPE_SEARCHPACKAGEONGITHUBBYUSER "SEARCHPACKAGEONGITHUBBYUSER"
#define TYPE_GETVERSION "GETVERSION"
#define TYPE_GETAVAILABLEPACKAGES "GETAVAILABLEPACKAGES"
#define TYPE_GETCOREADDONS "GETCOREADDONS"
#define TYPE_GETGLOBALLYINSTALLEDPACKAGES "GETGLOBALLYINSTALLEDPACKAGES"
#define TYPE_GETLOCALLYINSTALLEDPACKAGES "GETLOCALLYINSTALLEDPACKAGES"

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
            result["errors"] = ofJson::array();
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
                result["errors"].push_back("missing config property");
            }
            if (doesKeyExist(payload, "cwd"))
            {
                _app.setCwdPath(payload["cwd"].get<std::string>());
            }
            else
            {
                result["errors"].push_back("missing cwd property");
            }

            if (type == TYPE_INSTALLPACKAGESFROMADDONSMAKEFILE)
            {
                if (_app.installPackagesFromAddonsMakeFile())
                {
                    result["success"] = true;
                }
            }
            else if (type == TYPE_INSTALLPACKAGE)
            {
                std::string destinationPath = "";

                if (doesKeyExist(payload, "destinationPath"))
                {
                    destinationPath = payload["destinationPath"].get<std::string>();
                }
                auto p = _app.installPackage(payload["key"].get<std::string>(), destinationPath);
                if (!p.empty())
                {
                    result["success"] = true;
                    result["payload"]["package"] = ofJson::object();
                    result["payload"]["package"]["url"] = p.getUrl();
                    result["payload"]["package"]["checkout"] = p.getCheckout();
                    result["payload"]["package"]["path"] = p.getPath();
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
                if (!p.empty())
                {
                    result["success"] = true;
                    result["payload"]["package"] = ofJson::object();
                    result["payload"]["package"]["url"] = p.getUrl();
                    result["payload"]["package"]["checkout"] = p.getCheckout();
                    result["payload"]["package"]["path"] = p.getPath();
                }
            }
            else if (type == TYPE_INSTALLPACKAGEBYGITHUB)
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
                auto p = _app.installPackageByGithub(payload["github"].get<std::string>(), checkout, destinationPath);
                if (!p.empty())
                {
                    result["success"] = true;
                    result["payload"]["package"] = ofJson::object();
                    result["payload"]["package"]["url"] = p.getUrl();
                    result["payload"]["package"]["checkout"] = p.getCheckout();
                    result["payload"]["package"]["path"] = p.getPath();
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
                auto p = _app.installPackageByUrl(payload["url"].get<std::string>(), checkout, destinationPath);
                if (!p.empty())
                {
                    result["success"] = true;
                    result["payload"]["package"] = ofJson::object();
                    result["payload"]["package"]["url"] = p.getUrl();
                    result["payload"]["package"]["checkout"] = p.getCheckout();
                    result["payload"]["package"]["path"] = p.getPath();
                }
            }
            else if (type == TYPE_GETPACKAGESLISTEDINADDONSMAKE)
            {
                result["payload"]["data"] = ofJson::array();

                auto packages = _app.getPackagesListedInAddonsMakeFile();
                for (auto &p : packages)
                {
                    auto o = ofJson::object();
                    o["url"] = p.getUrl();
                    o["checkout"] = p.getCheckout();
                    o["path"] = p.getPath();
                    result["payload"]["data"].push_back(o);
                }

                {
                    result["success"] = true;
                }
            }
            else if (type == TYPE_ADDPACKAGETOADDONSMAKEFILE)
            {
                auto p = ofPackage(
                    payload["package"]["path"].get<std::string>(),
                    payload["package"]["url"].get<std::string>(),
                    payload["package"]["checkout"].get<std::string>());
                if (_app.addPackageToAddonsMakeFile(p))
                {
                    result["success"] = true;
                }
            }
            else if (type == TYPE_SEARCHPACKAGEINDATABASEBYID)
            {
                auto data = _app.searchPackageInDatabaseById(payload["id"].get<std::string>());
                // TODO: it maybe makes sense to filter a bit
                // if (data["total_count"].get<int>() > 0)
                {
                    result["success"] = true;
                    result["payload"]["data"] = data;
                }
            }
            else if (type == TYPE_SEARCHPACKAGEONGITHUBBYNAME)
            {
                auto data = _app.searchPackageOnGithubByName(payload["name"].get<std::string>());
                // TODO: it maybe makes sense to filter a bit
                if (data["total_count"].get<int>() > 0)
                {
                    result["success"] = true;
                    result["payload"]["data"] = data;
                }
            }
            else if (type == TYPE_SEARCHPACKAGEONGITHUBBYUSER)
            {
                auto data = _app.searchPackageOnGithubByUser(payload["user"].get<std::string>());
                // TODO: it maybe makes sense to filter a bit
                if (data.size() > 0)
                {
                    result["success"] = true;
                    result["payload"]["data"] = data;
                }
            }
            else if (type == TYPE_GETVERSION)
            {
                auto v = _app.getVersion();
                result["success"] = true;
                result["payload"]["major"] = v.getMajor();
                result["payload"]["minor"] = v.getMinor();
                result["payload"]["patch"] = v.getPatch();
            }
            else if (type == TYPE_GETAVAILABLEPACKAGES)
            {
                auto v = _app.getPackagesDatabase();
                result["success"] = true;
                result["payload"]["data"] = v;
            }
            else if (type == TYPE_GETCOREADDONS)
            {
                auto v = _app.getCorePackages();
                result["success"] = true;
                result["payload"]["data"] = ofJson::array();
                for (auto &coreAddon : v)
                {
                    // todo: return array of objects
                    result["payload"]["data"].push_back(coreAddon.getPath());
                }
            }
            else if (type == TYPE_GETGLOBALLYINSTALLEDPACKAGES)
            {
                auto v = _app.getGloballyInstalledPackages();
                result["success"] = true;
                result["payload"]["data"] = ofJson::array();
                for (auto &package : v)
                {
                    auto p = ofJson::object();
                    p["path"] = package.getPath();
                    p["checkout"] = package.getCheckout();
                    p["url"] = package.getUrl();
                    result["payload"]["data"].push_back(p);
                }
            }
            else if (type == TYPE_GETLOCALLYINSTALLEDPACKAGES)
            {
                auto v = _app.getLocallyInstalledPackages();
                result["success"] = true;
                result["payload"]["data"] = ofJson::array();
                for (auto &package : v)
                {
                    auto p = ofJson::object();
                    p["path"] = package.getPath();
                    p["checkout"] = package.getCheckout();
                    p["url"] = package.getUrl();
                    result["payload"]["data"].push_back(p);
                }
            }
            return result;
        }
        ofJson result;
        result["success"] = false;
        result["errors"] = ofJson::array();
        result["errors"].push_back("could not parse json");
        return result;
    }

private:
    ofPackageManager _app;
};