#pragma once

class ofPackage
{
public:
	ofPackage(std::string path = "", std::string url = "", std::string checkout = "") : _path(path), _url(url), _checkout(checkout)
	{
	}
	bool empty()
	{
		return _path == "" && _url == "" && _checkout == "";
	}
	std::string toString()
	{
		if(_checkout.empty() || _url.empty()) return _path;
		return _path + "#" + _url + "@" + _checkout;
	}
	std::string getPath()
	{
		return _path;
	}
	std::string getUrl()
	{
		return _url;
	}
	std::string getCheckout()
	{
		return _checkout;
	}

private:
	std::string _path;
	std::string _url;
	std::string _checkout;
};