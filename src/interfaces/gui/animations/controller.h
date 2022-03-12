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
            auto timestamp = ofGetElapsedTimeMillis();
            if(timestamp - _timestamp > 10000){
                _index = (int)(ofRandom(0, _animations.size()));
                _timestamp = timestamp;
            }
            _animations[_index]->update();
        }
        void draw()
        {
            if (_animations[_index]->_fbo.isAllocated())
            {
                ImTextureID textureID = (ImTextureID)(uintptr_t)_animations[_index]->_fbo.getTexture().getTextureData().textureID;
                // ofxImGui::AddImage(_animations[_index]->_fbo.getTexture(), glm::vec2(256, 256));
                auto maxWidth = _animations[_index]->_fbo.getWidth();
                auto maxHeight = _animations[_index]->_fbo.getHeight();
                auto availableWidth = ImGui::GetContentRegionAvail().x;
                auto availableHeight = ImGui::GetContentRegionAvail().y;
                auto width = availableWidth;
                auto height = availableHeight;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + width/10);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + height/10);
                ImGui::Image(textureID, glm::vec2(width*0.8, height*0.8));
            }
        }
        std::string getCurrentLabel(){
            std::string label = "animation by ";
            return label + _animations[_index]->getAuthor();
        }
        std::string getCurrentUrl(){
            return _animations[_index]->getUrl();
        }

        std::vector<animation::base *> _animations;
        int _index;
        uint64_t _timestamp;
    };
};