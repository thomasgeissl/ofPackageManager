#pragma once
#include "ofUtils.h"

class notification
{
public:
    enum type
    {
        INFO,
        SUCCESS,
        WARNING,
        ERROR,
    };
    notification(int id, std::string message, type t, int duration) : _id(id), _message(message), _type(t), _timestamp(ofGetElapsedTimeMillis()), _duration(duration)
    {
    }
    bool timedOut()
    {
        return _timestamp + _duration < ofGetElapsedTimeMillis();
    }

    int _id;
    std::string _message;
    type _type;
    uint64_t _timestamp;
    int _duration;
};

class notifications
{
public:
    notifications()
    {
    }
    void update()
    {
        for (auto iter = _data.begin(); iter != _data.end();)
        {
            if (iter->timedOut())
                iter = _data.erase(iter);
            else
                ++iter;
        }
    }
    void add(std::string message, notification::type t = notification::type::INFO)
    {
        _data.push_back(notification(_counter++, message, t, 5000));
    }
    size_t size()
    {
        return _data.size();
    }
    std::vector<notification> getData()
    {
        return _data;
    }

private:
    std::vector<notification> _data;
    int _counter;
};