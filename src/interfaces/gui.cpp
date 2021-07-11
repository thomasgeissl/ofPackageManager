#include "gui.h"
#include "./Theme.h"

gui::gui(ofPackageManager app) : ofBaseApp(), _app(app),
                                 _homeState(ofxState::create("welcome")),
                                 _newState(ofxState::create("new")),
                                 _updateState(ofxState::create("update")),
                                 _updateMultipleState(ofxState::create("updateMultiple")),
                                 _configureProjectState(ofxState::create("configureProject")),
                                 _showStyleEditor(false)
{
    _stateMachine.setInitialState(_homeState);
    _stateMachine.addTransition(_homeState, "new", _newState);
    _stateMachine.addTransition(_homeState, "update", _updateState);
    _stateMachine.addTransition(_homeState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_newState, "next", _configureProjectState);
    _stateMachine.addTransition(_newState, "back", _homeState);

    _stateMachine.addTransition(_updateState, "next", _configureProjectState);
    _stateMachine.addTransition(_updateState, "back", _homeState);

    _stateMachine.addTransition(_configureProjectState, "back", _homeState);

    _stateMachine.addTransition(_updateMultipleState, "back", _homeState);

    _stateMachine.start();

    _targets.push_back(selectableTarget(OF_TARGET_LINUX, ofGetTargetPlatform() == OF_TARGET_LINUX));
    _targets.push_back(selectableTarget(OF_TARGET_LINUX64, ofGetTargetPlatform() == OF_TARGET_LINUX64));
    _targets.push_back(selectableTarget(OF_TARGET_LINUXARMV6L, ofGetTargetPlatform() == OF_TARGET_LINUXARMV6L));
    _targets.push_back(selectableTarget(OF_TARGET_LINUXARMV7L, ofGetTargetPlatform() == OF_TARGET_LINUXARMV7L));
    _targets.push_back(selectableTarget(OF_TARGET_MINGW, ofGetTargetPlatform() == OF_TARGET_MINGW));
    _targets.push_back(selectableTarget(OF_TARGET_WINVS, ofGetTargetPlatform() == OF_TARGET_WINVS));
    _targets.push_back(selectableTarget(OF_TARGET_OSX, ofGetTargetPlatform() == OF_TARGET_OSX));
    _targets.push_back(selectableTarget(OF_TARGET_IOS, ofGetTargetPlatform() == OF_TARGET_IOS));
    _targets.push_back(selectableTarget(OF_TARGET_ANDROID, ofGetTargetPlatform() == OF_TARGET_ANDROID));

    for (auto &target : _targets)
    {
        _templates[target._target] = std::vector<selectableTemplate>();
    }
}
void gui::setup()
{
    ofSetBackgroundColor(16, 16, 16);
    _gui.setup(nullptr, false);
    _gui.setTheme(new Theme());
}

void gui::exit()
{
}

void gui::update()
{
    auto frameNum = ofGetFrameNum();
    if (frameNum % 60 == 0)
    {
        for (auto package : _app.getCorePackages())
        {
            if (_corePackages.find(package.getPath()) == _corePackages.end())
            {
                // TODO: check value
                _corePackages[package.getPath()] = selectablePackage(package, false);
            }
        }
        for (auto package : _app.getGloballyInstalledPackages())
        {
            if (_globalPackages.find(package.getPath()) == _globalPackages.end())
            {
                // TODO: check value
                _globalPackages[package.getPath()] = selectablePackage(package, false);
            }
        }
        for (auto package : _app.getLocallyInstalledPackages())
        {
            if (_localPackages.find(package.getPath()) == _localPackages.end())
            {
                // TODO: check value
                _localPackages[package.getPath()] = selectablePackage(package, false);
            }
        }
    }

    if (frameNum % 300 == 0 && !_projectPath.empty() && ofDirectory::doesDirectoryExist(_projectPath))
    {
        for (auto &target : _targets)
        {
            auto templates = getTargetProject(target._target)->listAvailableTemplates(getTargetString(target._target));
            for (auto &template_ : templates)
            {
                bool found = false;
                for (auto &selectableTemplate_ : _templates.at(target._target))
                {
                    if (selectableTemplate_._template.dir.path() == template_.dir.path())
                    {
                        found = true;
                    }
                }
                if (!found)
                {
                    _templates[target._target].push_back(selectableTemplate(template_, false));
                }
            }
        }
    }
}

void gui::draw()
{
    _gui.begin();
    auto menuBarSize = drawMenuGui();
    ImGui::SetNextWindowPos(ImVec2(0, menuBarSize.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), ofGetHeight() - menuBarSize.y));
    // ImGui::ShowDemoWindow();
    bool open = true;
    if (ImGui::Begin("Main Window", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
    {

        if (_stateMachine.isCurrentState(_homeState))
        {

            if (Button("install addons"))
            {
            }
            ImGui::SameLine();
            if (Button("new project"))
            {
                _stateMachine.trigger("new");
            }

            if (Button("update project"))
            {
                _stateMachine.trigger("update");
            }
            ImGui::SameLine();
            if (Button("update multiple projects"))
            {
                _stateMachine.trigger("updateMultiple");
            }

            // drawRecentProjects();
        }
        else if (_stateMachine.isCurrentState(_newState))
        {
            // if (ImGui::Button("path"))
            // {
            //     auto result = ofSystemLoadDialog("where should your project live", true, _app.getMyAppsPath());
            //     if (result.bSuccess)
            //     {
            //         _projectDirectoryPath = result.getPath();
            //     }
            // }
            Path(_projectDirectoryPath);
            char name[128] = "myProject";
            if (ImGui::InputText("project name", name, IM_ARRAYSIZE(name)))
            {
                _projectName = std::string(name);
                _projectPath = ofFilePath::join(_projectDirectoryPath, _projectName);
            }
            ofDirectory projectDirectory = ofDirectory(_projectDirectoryPath);
            ofDirectory projectDir = ofDirectory(_projectPath);
            if (projectDirectory.exists())
            {
                if (!projectDir.exists())
                {

                    if (ImGui::Button("next"))
                    {
                        auto dataPath = ofFilePath::getAbsolutePath(".");
                        _app.generateProject(_projectPath);
                        _stateMachine.trigger("next");
                    }
                }
            }
        }
        else if (_stateMachine.isCurrentState(_updateState))
        {
            ImGui::Text("TODO: update");
            if (ImGui::Button("path"))
            {
                auto result = ofSystemLoadDialog("where should your project live", true, _app.getMyAppsPath());
                if (result.bSuccess)
                {
                    _projectPath = result.getPath();
                }
            }
            ofDirectory projectDir = ofDirectory(_projectPath);
            if (projectDir.exists())
            {
                if (ImGui::Button("next"))
                {
                    auto dataPath = ofFilePath::getAbsolutePath(".");
                    ofLogNotice() << _app.findOfPathOutwardly(dataPath, 6);
                    ofLogNotice() << _app.getOfPath();
                    ofLogNotice() << ofFilePath::getAbsolutePath(".");
                    ofLogNotice() << _app.getCwdPath();
                    ofLogNotice() << _projectPath;
                    _app.generateProject(_projectPath);
                    _stateMachine.trigger("next");
                }
            }
        }
        else if (_stateMachine.isCurrentState(_configureProjectState))
        {
            if (ImGui::CollapsingHeader("core addons"))
            {
                for (auto &package : _corePackages)
                {
                    ImGui::Checkbox((package.second._package.getPath() + "##corepackage").c_str(), &package.second._selected);
                }
            }
            if (ImGui::CollapsingHeader("global addons"))
            {
                for (auto &package : _globalPackages)
                {
                    ImGui::Checkbox(package.second._package.toString().c_str(), &package.second._selected);
                }
            }
            if (ImGui::CollapsingHeader("local addons"))
            {
                for (auto &package : _localPackages)
                {
                    ImGui::Checkbox(package.second._package.toString().c_str(), &package.second._selected);
                }
            }

            if (ImGui::CollapsingHeader("platforms, templates"))
            {
                for (auto &target : _targets)
                {
                    ImGui::Checkbox(getTargetString(target._target).c_str(), &target._selected);
                }
            }

            if (ImGui::Button("install"))
            {
                ImGui::OpenPopup("my_select_popup");
            }
            if (ImGui::BeginPopup("my_select_popup"))
            {
            char name[128] = "";
            if (ImGui::InputText("querytext", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue ))
            {
                ofLogNotice() << name;
                _searchResults = _app.searchPackageOnGithubByName2(name);
            }
                ImGui::Separator();
                for(auto & repo : _searchResults){
                    ImGui::Text(repo._name.c_str());
                    // TODO: dropdown for tag select, latest and tags
                    // TODO: dropdown for destination select
                    // TODO: install button
                }
                ImGui::EndPopup();
            }

            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 300);

            if (Button("back"))
            {
                _stateMachine.trigger("back");
            }
            ImGui::SameLine();
            if (PrimaryButton("generate"))
            {
                setOFRoot(_app.getOfPath());
                for (auto &target : _targets)
                {
                    if (target._selected)
                    {
                        ofLogNotice() << "Templates for target " << getTargetString(target._target);
                        auto templates = getTargetProject(target._target)->listAvailableTemplates(getTargetString(target._target));
                        for (auto &templateConfig : templates)
                        {
                            ofLogNotice() << templateConfig.dir.path() << "\t\t" << templateConfig.description;
                        }
                    }
                }
            }
        }
        else if (_stateMachine.isCurrentState(_updateMultipleState))
        {
            ImGui::Text("TODO: update multiple");
            if (ImGui::Button("path"))
            {
                auto result = ofSystemLoadDialog("path to projects", true, _app.getMyAppsPath());
                if (result.bSuccess)
                {
                    auto path = result.getPath();
                    if (ofDirectory::doesDirectoryExist(path))
                    {
                        _app.updateMultipleProjects(path);
                    }
                }
            }
        }

        if (_showStyleEditor)
        {
            ImGui::ShowStyleEditor();
        }
        ImGui::End();
    }

    _gui.end();
    _gui.draw();
}

ImVec2 gui::drawMenuGui()
{
    auto size = ImVec2(-1, -1);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ofPackageManager"))
        {
            if (ImGui::MenuItem("About"))
            {
            }
            if (ImGui::MenuItem("Preferences"))
            {
            }
            if (ImGui::MenuItem("Quit"))
            {
                ofExit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                _stateMachine.trigger("new");
            }
            if (ImGui::MenuItem("Open"))
            {
                _stateMachine.trigger("open");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("style editor", NULL, &_showStyleEditor))
            {
            }
            ImGui::EndMenu();
        }
        size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
    return size;
}
void gui::drawRecentProjects()
{
    if (ImGui::Begin("recent projects"))
    {
        ImGui::Text("recent projects");
        ImGui::End();
    }
}

void gui::keyPressed(int key)
{
}
void gui::keyReleased(int key) {}
void gui::mouseMoved(int x, int y) {}
void gui::mouseDragged(int x, int y, int button) {}
void gui::mousePressed(int x, int y, int button) {}
void gui::mouseReleased(int x, int y, int button) {}
void gui::mouseEntered(int x, int y) {}
void gui::mouseExited(int x, int y) {}
void gui::windowResized(int w, int h) {}
void gui::dragEvent(ofDragInfo dragInfo) {}
void gui::gotMessage(ofMessage msg) {}

bool gui::Button(std::string text)
{
    text = ofToUpper(text);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 50));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
    auto pressed = false;
    if (ImGui::Button(text.c_str()))
    {
        pressed = true;
    }
    ImGui::PopStyleVar(2);
    return pressed;
}
bool gui::PrimaryButton(std::string text)
{
    text = ofToUpper(text);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 50));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(148.0 / 255, 36.0 / 255, 81.0 / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImVec2 size(200, 0.0f);
    auto pressed = false;
    if (ImGui::Button(text.c_str(), size))
    {
        pressed = true;
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    return pressed;
}

void gui::Path(std::string &path)
{
    if (ImGui::Button("open"))
    {
        auto result = ofSystemLoadDialog("path", true, _app.getMyAppsPath());
        if (result.bSuccess)
        {
            auto path_ = result.getPath();
            // if(!path_.emtpy()){
            path = path_;
            // }
        }
    }
    ImGui::Text(path.c_str());
}