#include "gui.h"
#include "./Theme.h"

gui::gui(ofPackageManager app) : ofBaseApp(), _app(app),
                                 _homeState(ofxState::create("home")),
                                 _installState(ofxState::create("manage addons")),
                                 _newState(ofxState::create("new")),
                                 _updateState(ofxState::create("update")),
                                 _updateMultipleState(ofxState::create("updateMultiple")),
                                 _configureProjectState(ofxState::create("configureProject")),
                                 _showStyleEditor(false),
                                 _showDemoWindow(false),
                                 _projectDirectoryPath(app.getMyAppsPath())
{
    _stateMachine.setInitialState(_homeState);
    // _projectPath = "/Users/thomasgeissl/libs/of_v0.11.2_osx_release/apps/myApps/project4";
    // _stateMachine.setInitialState(_configureProjectState);
    _stateMachine.addTransition(_homeState, "new", _newState);
    _stateMachine.addTransition(_homeState, "install", _installState);
    _stateMachine.addTransition(_homeState, "update", _updateState);
    _stateMachine.addTransition(_homeState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_installState, "back", _homeState);

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
    bool open = true;
    if (ImGui::Begin("Main Window", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
    {
        drawHeader();

        if (_stateMachine.isCurrentState(_homeState))
        {
            drawHomeState();
        }
        if (_stateMachine.isCurrentState(_installState))
        {
            drawInstallState();
        }
        else if (_stateMachine.isCurrentState(_newState))
        {
            drawNewState();
        }
        else if (_stateMachine.isCurrentState(_updateState))
        {
            drawUpdateState();
        }
        else if (_stateMachine.isCurrentState(_configureProjectState))
        {
            drawConfigureState();
        }
        else if (_stateMachine.isCurrentState(_updateMultipleState))
        {
            drawUpdateMultipleState();
        }

        if (_showStyleEditor)
        {
            ImGui::ShowStyleEditor();
        }
        if (_showDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }

        drawModals();
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
                ImGui::OpenPopup("About##modal");
            }
            if (ImGui::MenuItem("Preferences"))
            {
                ImGui::OpenPopup("Preferences");
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
            ImGui::MenuItem("demo window", NULL, &_showDemoWindow);
            ImGui::EndMenu();
        }
        size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
    return size;
}
void gui::drawHeader()
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
    if (!_stateMachine.isCurrentState(_homeState))
    {
        if (Button("back"))
        {
            _stateMachine.trigger("back");
        }
        ImGui::SameLine();
    }
    ImGui::Text(ofToUpper(_stateMachine.getCurrentState()->getName()).c_str());
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
}
void gui::drawModals()
{
    if (ImGui::BeginPopupModal("about", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::EndPopup();
    }
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth() - 100, ofGetHeight() - 100));
    ImGui::SetNextWindowPos(ImVec2(50, 50));
    if (ImGui::BeginPopupModal("search", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        char name[128] = "";
        if (ImGui::InputText("querytext", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            _searchResults = _app.searchPackageOnGithubByName2(name);
            _selectedSearchResult = ghRepo();
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table1", 3, flags))
        {
            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("stars", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("forks", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            auto i = 0;
            for (auto &repo : _searchResults)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(repo._name.c_str(), _selectedSearchResult._name == repo._name, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups))
                {
                    _selectedSearchResult = repo;
                }
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(ofToString(repo._stars).c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text(ofToString(repo._forks).c_str());
                i++;
            }
            ImGui::EndTable();
        }
        if (!_selectedSearchResult._name.empty())
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);

            ImGui::Text(_selectedSearchResult._name.c_str());
            ImGui::Text(_selectedSearchResult._url.c_str());
            ImGui::Text(_selectedSearchResult._cloneUrl.c_str());
            ImGui::Text(ofToString(_selectedSearchResult._stars).c_str());
            ImGui::Text(ofToString(_selectedSearchResult._forks).c_str());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);

            if (ImGui::Button("install"))
            {
            }
        }
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        // {
        //     for (auto &tag : repo._tags)
        //     {
        //         const bool is_selected = true;
        //         if (ImGui::Selectable(tag.c_str(), is_selected))
        //         {
        //             // TODO: setselected
        //         }

        //         if (is_selected)
        //         {
        //             ImGui::SetItemDefaultFocus();
        //         }
        //     }

        //     ImGui::EndCombo();
        // }

        // std::string buttonText = "install##";
        // buttonText += repo._url;
        // // repo.toString()
        ImGui::EndPopup();
    }
}
void gui::drawRecentProjects()
{
    if (ImGui::Begin("recent projects"))
    {
        ImGui::Text("recent projects");
        ImGui::End();
    }
}

void gui::drawHomeState()
{
    if (Button("manage addons", OFPACKAGEMANAGER_GUI_SIZE::BIG))
    {
        _stateMachine.trigger("install");
    }
    ImGui::SameLine();
    if (Button("new project", OFPACKAGEMANAGER_GUI_SIZE::BIG))
    {
        _stateMachine.trigger("new");
    }

    if (Button("update project", OFPACKAGEMANAGER_GUI_SIZE::BIG))
    {
        _stateMachine.trigger("update");
    }
    ImGui::SameLine();
    if (Button("update multiple projects", OFPACKAGEMANAGER_GUI_SIZE::BIG))
    {
        _stateMachine.trigger("updateMultiple");
    }
}
void gui::drawInstallState()
{
    ImGui::Text("list core addons");
    ImGui::Text("list globally installed addons, delete");

    for (auto corePackage : _corePackages)
    {
        ImGui::Text(corePackage.second._package.getPath().c_str());
    }
    for (auto &package : _globalPackages)
    {
        ImGui::Checkbox(package.second._package.toString().c_str(), &package.second._selected);
    }

// TODO: only if at least one is selected
    if (Button("delete"))
    {
        ofLogNotice() << "delete selected packages";
    }
    ImGui::SameLine();
    if (Button("add"))
    {
        ImGui::OpenPopup("search");
    }
}
void gui::drawNewState()
{
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
            if (Button("generate and configure", OFPACKAGEMANAGER_GUI_SIZE::MEDIUM, true))
            {
                auto dataPath = ofFilePath::getAbsolutePath(".");
                _app.generateProject(_projectPath);
                _stateMachine.trigger("next");
            }
        }
    }
}
void gui::drawUpdateState()
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
void gui::drawUpdateMultipleState()
{
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
void gui::drawConfigureState()
{
    if (ImGui::CollapsingHeader("core addons"))
    {
        ImGui::Indent();
        for (auto &package : _corePackages)
        {
            ImGui::Checkbox((package.second._package.getPath() + "##corepackage").c_str(), &package.second._selected);
        }
        ImGui::Unindent();
    }
    if (ImGui::CollapsingHeader("global addons"))
    {
        ImGui::Indent();
        for (auto &package : _globalPackages)
        {
            ImGui::Checkbox(package.second._package.toString().c_str(), &package.second._selected);
        }
        ImGui::Unindent();
    }
    if (_localPackages.size() > 0)
    {
        if (ImGui::CollapsingHeader("local addons"))
        {
            ImGui::Indent();
            for (auto &package : _localPackages)
            {
                ImGui::Checkbox(package.second._package.toString().c_str(), &package.second._selected);
            }
            ImGui::Unindent();
        }
    }

    if (ImGui::CollapsingHeader("platforms, templates"))
    {
        ImGui::Indent();
        ImGui::BeginChild("platforms");
        ImGui::Text("platforms");
        for (auto &target : _targets)
        {
            ImGui::Checkbox(getTargetString(target._target).c_str(), &target._selected);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("templates");
        ImGui::Text("templates");
        for (auto &template_ : _templates)
        {
            // TODO: filter only available templates for selected platforms
        }
        ImGui::EndChild();
        ImGui::Unindent();
    }

    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 200);
    if (Button("install"))
    {
        ImGui::OpenPopup("search");
    }
    ImGui::SameLine();
    if (Button("generate", OFPACKAGEMANAGER_GUI_SIZE::SMALL, true))
    {
        ofLogNotice() << "generating project";
        // set of root, required by project manager
        setOFRoot(_app.getOfPath());
        for (auto &target : _targets)
        {
            if (target._selected)
            {
                auto project = getTargetProject(target._target);
                auto template_ = "";
                std::string addonsMakeText = "";
                // TODO: get selected template
                project->create(_app.getCwdPath(), template_);

                // add selected core packages
                for (auto &corePackage : _corePackages)
                {
                    auto package = corePackage.second;
                    if (package._selected)
                    {
                        project->addAddon(package._package.getPath());
                        addonsMakeText += package._package.toString();
                        addonsMakeText += "\n";
                    }
                }
                // add selected global packages
                for (auto &globalPackage : _globalPackages)
                {
                    auto package = globalPackage.second;
                    if (package._selected)
                    {
                        project->addAddon(package._package.getPath());
                        addonsMakeText += package._package.toString();
                        addonsMakeText += "\n";
                    }
                }
                // add selected local packages
                for (auto &localPackage : _localPackages)
                {
                    auto package = localPackage.second;
                    if (package._selected)
                    {
                        project->addAddon(package._package.getPath());
                        addonsMakeText += package._package.toString();
                        addonsMakeText += "\n";
                    }
                }
                // finally generate the project
                project->save();
                ofLogNotice() << addonsMakeText;

                // TODO: override addons.make with commit hash comments

                // ofLogNotice() << "Templates for target " << getTargetString(target._target);
                // auto templates = getTargetProject(target._target)->listAvailableTemplates(getTargetString(target._target));
                // for (auto &templateConfig : templates)
                // {
                //     ofLogNotice() << templateConfig.dir.path() << "\t\t" << templateConfig.description;
                // }
            }
        }
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

bool gui::Button(std::string text, OFPACKAGEMANAGER_GUI_SIZE size, bool primary)
{
    text = ofToUpper(text);
    auto framePaddingHeight = 8;
    if (size == OFPACKAGEMANAGER_GUI_SIZE::MEDIUM)
    {
        framePaddingHeight = 32;
    }
    else if (size == OFPACKAGEMANAGER_GUI_SIZE::BIG)
    {
        framePaddingHeight = 50;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, framePaddingHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
    if (primary)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(148.0 / 255, 36.0 / 255, 81.0 / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    auto pressed = false;
    if (ImGui::Button(text.c_str()))
    {
        pressed = true;
    }
    ImGui::PopStyleVar(2);
    if (primary)
    {
        ImGui::PopStyleColor(3);
    }
    return pressed;
}

void gui::Path(std::string &path)
{
    ImGui::Text(path.c_str());
    ImGui::SameLine();
    if (ImGui::Button("choose"))
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
}