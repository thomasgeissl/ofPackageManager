#pragma once
#include "ofMain.h"

namespace animation
{
    class base
    {
    public:
        base(std::string author, std::string url = "") : _author(author), _url(url)
        {
            ofFbo::Settings fboSettings;
            fboSettings.width = 1024;
            fboSettings.height = 1024;
            fboSettings.internalformat = GL_RGBA;
            fboSettings.textureTarget = GL_TEXTURE_2D;
            _fbo.allocate(fboSettings);
        }
        void begin(bool clear = true)
        {
            _fbo.begin();
            if (clear)
            {
                ofClear(ofColor(0, 0, 0, 0));
            }
            ofPushStyle();
        }
        void end()
        {
            ofPopStyle();
            _fbo.end();
        }
        virtual void update() = 0;
        std::string getAuthor()
        {
            return _author;
        }
        std::string getUrl()
        {
            return _url;
        }
        ofFbo _fbo;
        std::string _author;
        std::string _url;
    };
};