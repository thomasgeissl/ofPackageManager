#pragma once
#include "ofMain.h"

namespace animation
{
    class base
    {
    public:
    base(){
        _fbo.allocate(1024, 1023, GL_RGBA);
    }
    virtual void update() = 0;
    ofFbo _fbo;
    };
};