#include "ofxImGui.h"

bool Button(std::string text, ImVec2 size = ImVec2(0, 0), bool primary = false, bool disabled = false)
{
    // text = ofToUpper(text);
    auto framePaddingHeight = 8;
    // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, framePaddingHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
    if (primary && disabled)
    {
        auto opacity = 0.3f;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, opacity));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, opacity));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, opacity));
    }
    else if (primary && !disabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(148.0 / 255, 36.0 / 255, 81.0 / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else if (!primary && disabled)
    {
        auto opacity = 0.3f;
        auto colors = ImGui::GetStyle().Colors;
        auto buttonColor = colors[ImGuiCol_Button];
        auto textColor = colors[ImGuiCol_Text];
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(buttonColor.x, buttonColor.y, buttonColor.z, opacity));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonColor.x, buttonColor.y, buttonColor.z, opacity));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(textColor.x, textColor.y, textColor.z, opacity));
    }
    auto pressed = false;
    if (ImGui::Button(text.c_str(), size))
    {
        pressed = true;
    }
    ImGui::PopStyleVar(1);
    if (primary || disabled)
    {
        ImGui::PopStyleColor(3);
    }
    if (disabled)
    {
        return false;
    }
    return pressed;
}

bool MenuButton(const char* icon, std::string text, ImVec2 size = ImVec2(0, 0), bool active = false)
{
    auto style = ImGui::GetStyle();
    auto actionColor = style.Colors[ImGuiCol_ResizeGripActive];
    text = ofToUpper(text);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
    if (active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, actionColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, actionColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    auto pressed = false;

    // Calculate the combined text with icon
    std::string buttonText = std::string(icon) + " " + text;

    // Get the size of the button content
    ImVec2 textSize = ImGui::CalcTextSize(buttonText.c_str());

    // Adjust the button size to fit the icon and text if a size is not provided
    if (size.x == 0) size.x = textSize.x + style.FramePadding.x * 2.0f;
    if (size.y == 0) size.y = textSize.y + style.FramePadding.y * 2.0f;

    // Render the button with the combined text
    if (ImGui::Button(buttonText.c_str(), size))
    {
        pressed = true;
    }

    ImGui::PopStyleVar(1);
    if (active)
    {
        ImGui::PopStyleColor(4);
    }

    return pressed;
}

bool MinButton(std::string text, ImVec2 size = ImVec2(0, 0))
{
    auto style = ImGui::GetStyle();
    auto actionColor = style.Colors[ImGuiCol_ResizeGripActive];
    text = ofToUpper(text);
    auto framePaddingHeight = 8;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, actionColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    auto pressed = false;
    if (ImGui::Button(text.c_str(), size))
    {
        pressed = true;
    }
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);
    return pressed;
}

void PathChooser(std::string &path, std::string startPath = "", ImVec2 size = ImVec2(0, 0))
{
    ImGui::Text(path.empty() ? "path has not yet been selected" : path.c_str());
    if (Button("open file dialog", size))
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

bool BeginActions(int numberOfButtons, int buttonWidth = 200)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 16));
    if (ImGui::BeginChild("actions", ImVec2(0, 0)))
    {
        auto style = ImGui::GetStyle();
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - numberOfButtons * buttonWidth - (numberOfButtons - 1) * style.ItemSpacing.x);
        return true;
    }
    return false;
}
void EndActions()
{
    ImGui::EndChild();
    ImGui::PopStyleVar();
}

bool BeginModal(std::string title)
{
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth() * 0.7, ofGetHeight() * 0.7));
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() * .15, ofGetHeight() * .15));
    if (ImGui::BeginPopupModal(title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    {
        return true;
    }
    return false;
}
void EndModal(bool withCloseButton, int buttonWidth = -1)
{
    if (withCloseButton && BeginActions(1))
    {
        if (Button("close", ImVec2(buttonWidth, -1)))
        {
            ImGui::CloseCurrentPopup();
        }
        EndActions();
    }
    ImGui::EndPopup();
}

std::string getShortCutLabel(std::string key, bool cmdOrCtrlKey = true, bool shiftKey = false)
{
    std::string label;
#ifdef TARGET_OSX
    label = "cmd+";
#endif
#ifdef TARGET_WIN
    label = "ctrl+";
#endif
#ifdef TARGET_LINUX
    label = "ctrl+";
#endif
if(shiftKey){
    label += "shift+";
}
    return label + key;
}