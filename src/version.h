#pragma once

class version
{
public:
    version(int major, int minor, int patch) : _major(major), _minor(minor), _patch(patch)
    {
    }
    int _major;
    int _minor;
    int _patch;
};