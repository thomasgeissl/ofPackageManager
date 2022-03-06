
#pragma once

#include "BaseTheme.h"

class Theme : public ofxImGui::BaseTheme
{
public:
    void setup() override;
    ImVec4 ColorFromBytes(uint8_t r, uint8_t g, uint8_t b)
    {
        return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
    };
};