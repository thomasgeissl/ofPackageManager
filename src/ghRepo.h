#pragma once
#include "ofJson.h"

class ghRepo
{
public:
    ghRepo() : _name(""){

    }
    ghRepo(ofJson data) : _data(data)
    {
        _name = data["full_name"];
        _shortName = data["name"];
        _cloneUrl = data["clone_url"];
        _url = data["html_url"];
        _updatedAt = data["updated_at"];
        _stars = data["stargazers_count"];
        _forks = data["forks_count"];
        _isFork = data["fork"].get<bool>();
    }
    std::string toString()
    {
        return "";
    }
    void fetchTags()
    {
        ofHttpRequest request(_data["tags_url"], "ofPackageManager");
        request.headers["User-Agent"] = "ofPackageManager";
        ofURLFileLoader loader;
        auto response = loader.handleRequest(request);
        auto resultJson = ofJson::parse(response.data.getText());
        ofLogNotice() << resultJson.dump(2);
        for(auto & tag : resultJson){
            // _tags.push_back(tag["name"]);
        }
    }
    ofJson _data;
    std::string _name;
    std::string _shortName;
    std::string _cloneUrl;
    std::string _url;
    std::string _updatedAt;
    int _stars;
    int _forks;
    bool _isFork;
    std::vector<std::string> _tags;
};