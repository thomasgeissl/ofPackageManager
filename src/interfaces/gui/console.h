#pragma once
#include "ofxImGui.h"
#include "./fonts/font_awesome_5.h"

class console
{
public:
    console() : _autoScroll(true)
    {
        _originalBuffer = std::cout.rdbuf(_consoleBuffer.rdbuf());
    }
    void update()
    {
    }
    void draw(int consoleHeight)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1.0f));

        ImGui::BeginChild("console", ImVec2(0, 0), false, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(ICON_FA_TERMINAL))
            {
                if (ImGui::MenuItem("close"))
                {
                    _closeEvent.notify();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("edit"))
            {
                if (ImGui::MenuItem("clear"))
                {
                    // _consoleBuffer.clear();
                    _consoleBuffer.str("");
                }
                if (ImGui::MenuItem("copy to clipboard"))
                {
                    ofSetClipboardString(_consoleBuffer.str());
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("view"))
            {
                ImGui::MenuItem("autoScroll", nullptr, &_autoScroll);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::TextUnformatted(_consoleBuffer.str().c_str());
        if (_autoScroll)
        {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    void exit()
    {
        std::cout.rdbuf(_originalBuffer);
    }
    std::string execute(std::string command){
        return ofSystem(command);
    }
    std::stringstream _consoleBuffer;
    std::streambuf *_originalBuffer;
    ofEvent<void> _closeEvent;
    bool _autoScroll;
};