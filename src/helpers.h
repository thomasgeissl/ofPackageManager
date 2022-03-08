#include "ofUtils.h"

std::string generateGithubUrl(std::string github)
{
	return "https://github.com/" + github + ".git";
}

bool isGitUrl(std::string path)
{
	// TODO: check if string ends with .git
	return path.substr(0, 4) == "git@" || path.substr(0, 4) == "http";
}

bool isGithubPair(std::string path)
{
	return ofSplitString(path, "/").size() > 1;
}