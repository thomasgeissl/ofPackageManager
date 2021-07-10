#include "gui.h"

gui::gui(ofPackageManager app) : ofBaseApp(), _app(app),
                                 _welcomeState(ofxState::create("welcome")),
                                 _newState(ofxState::create("new")),
                                 _updateState(ofxState::create("update")),
                                 _updateMultipleState(ofxState::create("updateMultiple")),
                                 _configureProjectState(ofxState::create("configureProject"))
{
    _stateMachine.setInitialState(_welcomeState);
    _stateMachine.addTransition(_welcomeState, "new", _newState);
    _stateMachine.addTransition(_newState, "next", _configureProjectState);
    _stateMachine.addTransition(_welcomeState, "update", _updateState);
    _stateMachine.addTransition(_updateState, "next", _configureProjectState);
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
}
void gui::setup()
{
    ofSetBackgroundColor(16, 16, 16);
    _gui.setup(nullptr, false);
}

void gui::exit()
{
}

void gui::update()
{
    if (ofGetFrameNum() % 60 == 0)
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

        if (_stateMachine.isCurrentState(_welcomeState))
        {

            ImGui::Button("install addons");
            ImGui::SameLine();
            if (ImGui::Button("new project"))
            {
                _stateMachine.transit("new");
            }
            ImGui::SameLine();
            if (ImGui::Button("update project"))
            {
                _stateMachine.transit("update");
            }
            ImGui::SameLine();
            ImGui::Button("update multiple projects");
            drawRecentProjects();
            ImGui::ShowStyleEditor();
        }
        else if (_stateMachine.isCurrentState(_newState))
        {
            if (ImGui::Button("path"))
            {
                auto result = ofSystemLoadDialog("where should your project live", true, _app.getMyAppsPath());
                if (result.bSuccess)
                {
                    _projectDirectoryPath = result.getPath();
                }
            }
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
                        ofLogNotice() << _app.findOfPathOutwardly(dataPath, 6);
                        ofLogNotice() << _app.getOfPath();
                        ofLogNotice() << ofFilePath::getAbsolutePath(".");
                        ofLogNotice() << _app.getCwdPath();
                        ofLogNotice() << _projectPath;
                        _app.generateProject(_projectPath);
                        _stateMachine.transit("next");
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
                    _stateMachine.transit("next");
                }
            }
        }
        else if (_stateMachine.isCurrentState(_configureProjectState))
        {
            ImGui::Text("TODO:  configure project");
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

            if (ImGui::Button("generate"))
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
                _stateMachine.transit("new");
            }
            if (ImGui::MenuItem("Open"))
            {
                _stateMachine.transit("open");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            // if (ImGui::MenuItem("Fullscreen", NULL, _fullscreen))
            // {
            //     // _fullscreen = !_fullscreen;
            //     ofSetFullscreen(_fullscreen);
            // }
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