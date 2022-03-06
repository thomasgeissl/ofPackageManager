#pragma once
#include "./demo.h"
#include "ofxImGui.h"

namespace animation
{
    class controller
    {
    public:
        controller() : _index(0)
        {
            _animations.push_back(new demo());
        }
        void update(){
            _animations[_index]->update();
        }
        void draw()
        {
            if (_animations[_index]->_fbo.isAllocated())
            {
                ImTextureID textureID = (ImTextureID)(uintptr_t)_animations[_index]->_fbo.getTexture().getTextureData().textureID;
                ImGui::Image(textureID, glm::vec2(256, 256));
            }
        }
        std::vector<animation::base *> _animations;
        int _index;
    };
};