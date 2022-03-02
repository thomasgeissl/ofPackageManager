#include "ofxImGui.h"

bool Button(std::string text, ImVec2 size = ImVec2(0,0), bool primary = false)
{
    text = ofToUpper(text);
    auto framePaddingHeight = 8;
    // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, framePaddingHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
    if (primary)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(148.0 / 255, 36.0 / 255, 81.0 / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    auto pressed = false;
    if (ImGui::Button(text.c_str(), size))
    {
        pressed = true;
    }
    ImGui::PopStyleVar(1);
    if (primary)
    {
        ImGui::PopStyleColor(3);
    }
    return pressed;
}

void PathChooser(std::string &path, std::string startPath = "")
{
    ImGui::Text(path.empty() ? "please choose" : path.c_str());
    ImGui::SameLine();
    if (ImGui::Button("choose"))
    {
        auto result = ofSystemLoadDialog("path", true, startPath);
        if (result.bSuccess)
        {
            auto path_ = result.getPath();
            // if(!path_.emtpy()){
            path = path_;
            // }
        }
    }
}

void Tooltip(std::string text)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

bool BeginActions(int numberOfButtons, int buttonWidth = 200){
    if (ImGui::BeginChild("actions", ImVec2(0, 0)))
    {
        auto style = ImGui::GetStyle();
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() - numberOfButtons * buttonWidth - (numberOfButtons-1)* style.ItemSpacing.x);
        return true;
    }
    return false;
}