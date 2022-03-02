#include "gui.h"
#include "./Theme.h"
#include "./helpers.h"
static int footerHeight = 64;
static int consoleHeight = 200;
static int buttonWidth = 200;

gui::gui(ofPackageManager app) : ofBaseApp(), _app(app),
                                 _homeState(ofxState::create("home")),
                                 _installState(ofxState::create("manage addons")),
                                 _createState(ofxState::create("create")),
                                 _updateState(ofxState::create("update")),
                                 _updateMultipleState(ofxState::create("updateMultiple")),
                                 _configureProjectState(ofxState::create("configureProject")),
                                 _fullscreen(false),
                                 _showAdvancedOptions(false),
                                 _showConsole(false),
                                 _showMetricsWindow(false),
                                 _showStyleEditor(false),
                                 _showDemoWindow(false),
                                 _searchModalOpened(false),
                                 _projectDirectoryPath(app.getMyAppsPath())
{
    _stateMachine.setInitialState(_homeState);
    _stateMachine.addTransition(_homeState, "create", _createState);
    _stateMachine.addTransition(_homeState, "install", _installState);
    _stateMachine.addTransition(_homeState, "update", _updateState);
    _stateMachine.addTransition(_homeState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_installState, "back", _homeState);
    _stateMachine.addTransition(_installState, "create", _createState);
    _stateMachine.addTransition(_installState, "update", _updateState);
    _stateMachine.addTransition(_installState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_createState, "configure", _configureProjectState);
    _stateMachine.addTransition(_createState, "back", _homeState);
    _stateMachine.addTransition(_createState, "install", _installState);
    _stateMachine.addTransition(_createState, "update", _updateState);
    _stateMachine.addTransition(_createState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_updateState, "configure", _configureProjectState);
    _stateMachine.addTransition(_updateState, "back", _homeState);
    _stateMachine.addTransition(_updateState, "install", _installState);
    _stateMachine.addTransition(_updateState, "create", _createState);
    _stateMachine.addTransition(_updateState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_configureProjectState, "back", _homeState);
    _stateMachine.addTransition(_configureProjectState, "install", _installState);
    _stateMachine.addTransition(_configureProjectState, "create", _createState);
    _stateMachine.addTransition(_configureProjectState, "update", _updateState);
    _stateMachine.addTransition(_configureProjectState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_updateMultipleState, "back", _homeState);
    _stateMachine.addTransition(_updateMultipleState, "install", _installState);
    _stateMachine.addTransition(_updateMultipleState, "create", _createState);
    _stateMachine.addTransition(_updateMultipleState, "update", _updateState);
    _stateMachine.addTransition(_updateMultipleState, "updateMultiple", _updateMultipleState);

    // TODO: add support for lambda functions to ofxStateMachine
    // _homeState->addEnteredListener([&]{ofLogNotice() << "home entered";});
    _homeState->addEnteredListener(this, &gui::onHomeStateEntered);
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
        _templates[target._target] = getTargetProject(target._target)->listAvailableTemplates(getTargetString(target._target));
    }
}
void gui::setup()
{
    ofSetWindowTitle("ofPackageManager");
    ofSetFrameRate(60);
    ofSetBackgroundColor(16, 16, 16);
    _gui.setup(nullptr, false);
    _gui.setTheme(new Theme());
    _originalBuffer = std::cout.rdbuf(_consoleBuffer.rdbuf());
}

void gui::exit()
{
    std::cout.rdbuf(_originalBuffer);
}

void gui::update()
{
    auto frameNum = ofGetFrameNum();
    // update packages list every 60 frames
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
}

void gui::draw()
{
    _gui.begin();
    auto menuHeight = drawMenu().y;
    bool open = true;
    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), ofGetHeight() - menuHeight));
    if (ImGui::Begin("window", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
    {
        drawSideBar();
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
        if (ImGui::BeginChild("content", ImVec2(0, 0)))
        {
            if (ImGui::BeginChild("main", ImVec2(0, _showConsole ? -consoleHeight : 0)))
            {
                if (_stateMachine.isCurrentState(_homeState))
                {
                    drawHome();
                }
                if (_stateMachine.isCurrentState(_installState))
                {
                    drawInstall();
                }
                else if (_stateMachine.isCurrentState(_createState))
                {
                    drawCreate();
                }
                else if (_stateMachine.isCurrentState(_updateState))
                {
                    drawUpdate();
                }
                else if (_stateMachine.isCurrentState(_configureProjectState))
                {
                    drawConfigure();
                }
                else if (_stateMachine.isCurrentState(_updateMultipleState))
                {
                    drawUpdateMultiple();
                }

                if (_showStyleEditor)
                {
                    ImGui::ShowStyleEditor();
                }
                if (_showDemoWindow)
                {
                    ImGui::SetNextWindowPos(ImVec2(0, 100));
                    ImGui::ShowDemoWindow();
                }
                if (_showMetricsWindow)
                {
                    ImGui::ShowMetricsWindow();
                }

                ImGui::EndChild();

                if (_showConsole)
                {
                    drawConsole();
                }
            }
            ImGui::EndChild();
        }

        if (_searchModalOpened)
        {
            _searchModalOpened = false;
            ImGui::OpenPopup("search");
        }
        drawModals();
        ImGui::End();
    }

    _gui.end();
    _gui.draw();
}

ImVec2 gui::drawMenu()
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
                _stateMachine.trigger("create");
            }
            if (ImGui::MenuItem("Open"))
            {
                _stateMachine.trigger("update");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("fullscreen", NULL, &_fullscreen))
            {
                ofToggleFullscreen();
            }
            ImGui::MenuItem("advanced options", NULL, &_showAdvancedOptions);
            ImGui::MenuItem("console", NULL, &_showConsole);
            ImGui::Separator();
            ImGui::MenuItem("metrics", NULL, &_showMetricsWindow);
            ImGui::MenuItem("style editor", NULL, &_showStyleEditor);
            ImGui::MenuItem("demo", NULL, &_showDemoWindow);
            ImGui::EndMenu();
        }
        size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
    return size;
}
void gui::drawSideBar()
{
    ImGui::BeginChild("sideBar", ImVec2(200, -1));
    auto style = ImGui::GetStyle();
    auto numberOfButtons = _showAdvancedOptions ? 5 : 4;
    auto availableHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - style.ItemSpacing.y;
    auto buttonSize = ImVec2(ImGui::GetContentRegionAvailWidth(), availableHeight / numberOfButtons - style.ItemSpacing.y);
    if (Button("home", buttonSize, _stateMachine.isCurrentState(_homeState)))
    {
        _stateMachine.trigger("back");
    }
    if (Button("manage addons", buttonSize, _stateMachine.isCurrentState(_installState)))
    {
        _stateMachine.trigger("install");
    }
    if (Button("create project", buttonSize, _stateMachine.isCurrentState(_createState)))
    {
        _stateMachine.trigger("create");
    }
    if (Button("update project", buttonSize, _stateMachine.isCurrentState(_updateState) || _stateMachine.isCurrentState(_configureProjectState)))
    {
        _stateMachine.trigger("update");
    }
    if (_showAdvancedOptions)
    {
        if (Button("update multiple projects", buttonSize, _stateMachine.isCurrentState(_updateMultipleState)))
        {
            _stateMachine.trigger("updateMultiple");
        }
    }
    ImGui::EndChild();
}
void gui::drawConsole()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1.0f));
    ImGui::BeginChild("console", ImVec2(0, 0));
    ImGui::TextWrapped(_consoleBuffer.str().c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();
}
void gui::drawModals()
{
    if (ImGui::BeginPopupModal("about", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::EndPopup();
    }
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth() * 0.8, ofGetHeight() * 0.8));
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() * .1, ofGetHeight() * .1));
    if (ImGui::BeginPopupModal("search", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
        if (ImGui::BeginChild("globalPackages", ImVec2(-1, -footerHeight - padding)))
        {
            char name[128] = "";
            ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 16));
            if (ImGui::InputText("##querytext", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                _searchResults = _app.searchPackageOnGithubByName2(name);
                _selectedSearchResult = ghRepo();
            }
            ImGui::PopStyleVar();
            ImGui::PopItemWidth();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);

            if (_searchResults.size() > 0)
            {

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
                if (ImGui::BeginTable("table1", 4, flags))
                {
                    ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("stars", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("forks", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("actions", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
                    auto i = 0;
                    for (auto &repo : _searchResults)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        // if (ImGui::Selectable(repo._name.c_str(), _selectedSearchResult._name == repo._name, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups))
                        // {
                        //     _selectedSearchResult = repo;
                        // }
                        ImGui::Text(repo._name.c_str());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(ofToString(repo._stars).c_str());
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text(ofToString(repo._forks).c_str());
                        ImGui::TableSetColumnIndex(3);
     
                        std::string id = "open##";
                        id += repo._url;
                        if (Button(id.c_str()))
                        {
                            ofLaunchBrowser(repo._url);
                        }
                        ImGui::SameLine();
                        id = "install##";
                        id += repo._url;
                        if (Button(id.c_str()))
                        {
                            _app.installPackageByUrl(repo._cloneUrl, "latest");
                        }
                        i++;
                    }
                    ImGui::EndTable();
                    ImGui::PopStyleVar();
                }
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
            }
            ImGui::EndChild();
        }
        if (BeginActions(1))
        {
            if (Button("close", ImVec2(buttonWidth, -1)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndChild();
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
    // if (ImGui::Begin("recent projects"))
    // {
    //     ImGui::Text("recent projects");
    //     ImGui::End();
    // }
}

void gui::drawHome()
{
    ImGui::TextWrapped("Welcome to ofPackageManager - a package manager for openFrameworks.\n");
}
void gui::drawInstall()
{
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("globalPackages", ImVec2(-1, -footerHeight - padding)))
    {
        for (auto corePackage : _corePackages)
        {
            ImGui::Text(corePackage.second._package.getPath().c_str());
        }
        for (auto &package : _globalPackages)
        {
            ImGui::Checkbox(package.second._package.getPath().c_str(), &package.second._selected);
            Tooltip(package.second._package.toString());
            ImGui::SameLine();
            ImGui::Button("delete");
            ImGui::SameLine();
            ImGui::Button("upgrade");
        }
        ImGui::EndChild();
    }
    if (BeginActions(1))
    {
        if (Button("install additional addons", ImVec2(buttonWidth, -1)))
        {
            _searchModalOpened = true;
        }
        ImGui::EndChild();
    }
}
void gui::drawCreate()
{
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("create", ImVec2(-1, -footerHeight - padding)))
    {
        PathChooser(_projectDirectoryPath, _app.getMyAppsPath());
        char name[128];
        strcpy(name, _projectName.c_str());
        if (ImGui::InputText("project name", name, IM_ARRAYSIZE(name)))
        {
            _projectName = std::string(name);
            _projectPath = ofFilePath::join(_projectDirectoryPath, _projectName);
        }
        ofDirectory projectDirectory = ofDirectory(_projectDirectoryPath);
        ofDirectory projectDir = ofDirectory(_projectPath);
        if (projectDirectory.exists())
        {
            if (projectDir.exists())
            {
                ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), "seems like your project exits already");
            }
        }
        ImGui::EndChild();
    }

    if (BeginActions(1))
    {
        ofDirectory projectDirectory = ofDirectory(_projectDirectoryPath);
        ofDirectory projectDir = ofDirectory(_projectPath);
        if (projectDirectory.exists())
        {
            if (!projectDir.exists())
            {
                if (Button("create and configure", ImVec2(buttonWidth, -1), true))
                {
                    auto dataPath = ofFilePath::getAbsolutePath(".");
                    _app.generateProject(_projectPath);
                    _stateMachine.trigger("configure");
                }
            }
        }
        ImGui::EndChild();
    }
}
void gui::drawUpdate()
{
    auto style = ImGui::GetStyle();
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("update", ImVec2(-1, -footerHeight - padding)))
    {
        PathChooser(_projectPath, _app.getMyAppsPath());
        ImGui::EndChild();
    }
    if (BeginActions(1))
    {
        ofDirectory projectDir = ofDirectory(_projectPath);
        if (!_projectPath.empty() && projectDir.exists())
        {
            if (Button("configure", ImVec2(buttonWidth, -1), true))
            {
                auto dataPath = ofFilePath::getAbsolutePath(".");
                ofLogNotice() << _app.findOfPathOutwardly(dataPath, 6);
                ofLogNotice() << _app.getOfPath();
                ofLogNotice() << ofFilePath::getAbsolutePath(".");
                ofLogNotice() << _app.getCwdPath();
                ofLogNotice() << _projectPath;
                _app.generateProject(_projectPath);
                _projectName = ofFilePath::getBaseName(_projectPath);
                _stateMachine.trigger("configure");
            }
        }
        ImGui::EndChild();
    }
}
void gui::drawUpdateMultiple()
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
void gui::drawConfigure()
{
    // auto availableHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY();
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("configure", ImVec2(-1, -footerHeight - padding)))
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        if (ImGui::BeginChild("configureHeader", ImVec2(0, 32)))
        {
            ImGui::Text(_projectName.c_str());
            ImGui::Text(_projectPath.c_str());
            ImGui::EndChild();
        }
        ImGui::PopStyleColor();

        if (ImGui::BeginChild("configureContent", ImVec2(0, 0)))
        {
            auto indentation = 24;
            if (ImGui::CollapsingHeader("core addons"))
            {
                ImGui::Indent(indentation);
                for (auto &package : _corePackages)
                {
                    ImGui::Checkbox((package.second._package.getPath() + "##corepackage").c_str(), &package.second._selected);
                }
                ImGui::Unindent(indentation);
            }
            if (ImGui::CollapsingHeader("global addons"))
            {
                ImGui::Indent(indentation);
                for (auto &package : _globalPackages)
                {
                    ImGui::Checkbox(package.second._package.getPath().c_str(), &package.second._selected);
                    Tooltip(package.second._package.getUrl() + "@" + package.second._package.getCheckout());
                }
                ImGui::Unindent(indentation);
            }
            if (_localPackages.size() > 0)
            {
                if (ImGui::CollapsingHeader("local addons"))
                {
                    ImGui::Indent(indentation);
                    for (auto &package : _localPackages)
                    {
                        ImGui::Checkbox(package.second._package.getPath().c_str(), &package.second._selected);
                        Tooltip(package.second._package.getUrl() + "@" + package.second._package.getCheckout());
                    }
                    ImGui::Unindent(indentation);
                }
            }

            if (_showAdvancedOptions)
            {

                if (ImGui::CollapsingHeader("platforms, templates"))
                {
                    ImGui::Indent(indentation);
                    auto size = ImVec2(ImGui::GetContentRegionAvail().x / 2, 0);
                    ImGui::BeginChild("platforms", size);
                    ImGui::Text("platforms");

                    auto selectedTargets = std::vector<ofTargetPlatform>();
                    for (auto &target : _targets)
                    {
                        ImGui::Checkbox(getTargetString(target._target).c_str(), &target._selected);
                        if (target.isSelected())
                        {
                            selectedTargets.push_back(target._target);
                        }
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("templates", size);
                    ImGui::Text("templates");
                    // TODO: update common selectable template list
                    for (auto &selectedTarget : selectedTargets)
                    {
                        ImGui::Text(getTargetString(selectedTarget).c_str());
                        // _templates[selectedTarget]
                    }
                    for (auto &selectableTemplate : _commonTemplates)
                    {
                        ImGui::Checkbox(selectableTemplate._template.name.c_str(), &selectableTemplate._selected);
                        Tooltip(selectableTemplate._template.description);
                    }
                    ImGui::EndChild();
                    ImGui::Unindent(indentation);
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
    if (BeginActions(2))
    {
        if (Button("install additional addons", ImVec2(buttonWidth, -1)))
        {
            _searchModalOpened = true;
        }
        ImGui::SameLine();
        if (Button("generate project", ImVec2(buttonWidth, -1), true))
        {
            ofLogNotice() << "generating project";
            // TODO: move to package manager, so it can be called from the cli as well
            // set of root, required by project manager
            setOFRoot(_app.getOfPath());
            for (auto &target : _targets)
            {
                if (target.isSelected())
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
                        if (package.isSelected())
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
                        if (package.isSelected())
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
                        if (package.isSelected())
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
        ImGui::EndChild();
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

void gui::onHomeStateEntered(ofxStateEnteredEventArgs &args)
{
    for (auto &package : _corePackages)
    {
        package.second._selected = false;
    }
    for (auto &package : _globalPackages)
    {
        package.second._selected = false;
    }
    for (auto &package : _localPackages)
    {
        package.second._selected = false;
    }
    // TODO: reset templates and targets
}