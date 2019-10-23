#pragma once

class ofPackage
{
public:
	ofPackage(std::string path = "", std::string url = "", std::string checkout = "") : _path(path), _url(url), _checkout(checkout)
	{
	}
	bool isEmpty()
	{
		return _path == "" && _url == "" && _checkout == "";
	}
	std::string _path;
	std::string _url;
	std::string _checkout;
};