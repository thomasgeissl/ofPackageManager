#pragma once
#include "ofUtils.h"

class ofVersion
{
public:
    ofVersion(int major, int minor, int patch) : _major(major), _minor(minor), _patch(patch)
    {
    }
    bool operator<(ofVersion v) const
    {
        if (_major > v.getMajor())
        {
            return false;
        }
        if (_minor > v.getMinor())
        {
            return false;
        }
        if (_patch > v.getPatch())
        {
            return false;
        }
        return true;
    }
    bool operator>(ofVersion v) const
    {
        if (_major > v.getMajor())
        {
            return true;
        }
        if (_minor > v.getMinor())
        {
            return true;
        }
        if (_patch > v.getPatch())
        {
            return true;
        }
        return false;
    }
    int getMajor()
    {
        return _major;
    }
    int getMinor()
    {
        return _minor;
    }
    int getPatch()
    {
        return _patch;
    }
    std::string toString()
    {
        return ofToString(_major) + "." + ofToString(_minor) + "." + ofToString(_patch);
    }
    void setMajor(int major)
    {
        _major = major;
    }
    void setMinor(int minor)
    {
        _minor = minor;
    }
    void setPatch(int patch)
    {
        _patch = patch;
    }

private:
    int _major;
    int _minor;
    int _patch;
};