#include "../defines.h"
#include "./gui.h"
#include "./Theme.h"
#include "./helpers.h"
#include "./fonts/tahoma.h"
#include "./fonts/fa_solid_900.h"
#include "./fonts/font_awesome_5.h"
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
                                 _aboutModalOpened(false),
                                 _preferencesModalOpened(false),
                                 _searchModalOpened(false),
                                 _projectDirectoryPath(app.getMyAppsPath()),
                                 _version(_app.getVersion()),
                                 _mostRecentVersion(_app.getNewestAvailableVersion())
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
    _configureProjectState->addEnteredListener(this, &gui::onConfigureStateEntered);
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

    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.GlyphMinAdvanceX = 13.0f;
    // io.Fonts->AddFontDefault();
    config.MergeMode = false;
    io.Fonts->AddFontFromMemoryTTF((void *)tahoma, sizeof(tahoma), 13.f, &config);
    config.MergeMode = true;
    io.Fonts->AddFontFromMemoryTTF((void *)fa_solid_900, sizeof(fa_solid_900), 13.f, &config, icon_ranges);

    // _originalBuffer = std::cout.rdbuf(_consoleBuffer.rdbuf());
}

void gui::exit()
{
    std::cout.rdbuf(_originalBuffer);
}

void gui::update()
{
    _notifications.update();
    _animations.update();
    auto frameNum = ofGetFrameNum();
    if (frameNum % 60 == 0)
    {
        updatePackagesLists();
    }
    if (frameNum % 60 * 5 == 0)
    {
        updateRecentProjectsList();
    }
}

void gui::draw()
{
    _gui.begin();
    auto menuHeight = drawMenu().y;
    bool open = true;
    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), ofGetHeight() - menuHeight));
    if (ImGui::Begin("window", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus))
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

        if (_aboutModalOpened)
        {
            _aboutModalOpened = false;
            ImGui::OpenPopup("about");
        }
        if (_preferencesModalOpened)
        {
            _preferencesModalOpened = false;
            ImGui::OpenPopup("preferences");
        }
        if (_searchModalOpened)
        {
            _searchModalOpened = false;
            ImGui::OpenPopup("search");
        }
        drawModals();
        drawNotifications();
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
                _aboutModalOpened = true;
            }
            if (ImGui::MenuItem("Preferences"))
            {
                _preferencesModalOpened = true;
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
    if (Button("new project", buttonSize, _stateMachine.isCurrentState(_createState)))
    {
        _stateMachine.trigger("create");
    }
    if (Button("open project", buttonSize, _stateMachine.isCurrentState(_updateState) || _stateMachine.isCurrentState(_configureProjectState)))
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
void gui::drawNotifications()
{
    if (_notifications.size() == 0)
    {
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth() * 0.5, ofGetHeight() * 0.2));
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() * 0.5, 32));
    auto open = true;
    if (ImGui::Begin("notifications", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
    {
        for (auto notification : _notifications.getData())
        {
            std::string id = "notification_";
            id += notification._id;
            if (ImGui::BeginChild(id.c_str()))
            {
                ImGui::TextWrapped(notification._message.c_str());
                ImGui::EndChild();
            }
        }
        ImGui::End();
    }
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
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (BeginModal("about"))
    {
        if (ImGui::BeginChild("modalContent", ImVec2(-1, -footerHeight - padding)))
        {
            std::string version = "ofPackageManager version: ";
            version += _version.toString();
            ImGui::Text(version.c_str());

            std::string mostRecentVersion = "ofPackageManager most recent version: ";
            mostRecentVersion += _mostRecentVersion.toString();
            ImGui::Text(mostRecentVersion.c_str());

            std::string pgVersion = "ofxProjectGenerator commit: ";
            pgVersion += OFXPROJECTGENERATOR_COMMIT;
            ImGui::Text(pgVersion.c_str());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16);
            ImGui::Text("license:");
            std::string license = "This software is distributed under the MIT License.\n\nCopyright(c) 2019 Thomas Geissl\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";
            ImGui::TextWrapped(license.c_str());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16);
            ImGui::Text("source code:");
            ImGui::TextWrapped("The source can be found on github, feel free to file issues, fork, fix and PR.");
            std::string url = "https://github.com/thomasgeissl/ofPackageManager/";
            (ImGui::TextWrapped(url.c_str()));
            ImGui::SameLine();
            if (Button("open"))
            {
                ofLaunchBrowser(url);
            }
            ImGui::EndChild();
        }

        EndModal(buttonWidth);
    }
    if (BeginModal("preferences"))
    {
        if (ImGui::BeginChild("modalContent", ImVec2(-1, -footerHeight - padding)))
        {
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
        ImGui::EndPopup();
    }
    if (BeginModal("search"))
    {
        if (ImGui::BeginChild("modalContent", ImVec2(-1, -footerHeight - padding)))
        {
            char name[128] = "";
            strcpy(name, _queryText.c_str());
            ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 16));
            if (ImGui::InputText("##querytext", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                _queryText = name;
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
                            std::string destinationPath = ""; // locally by default
                            if (_stateMachine.isCurrentState(_installState))
                            {
                                destinationPath = _app.getAddonsPath();
                            }
                            auto package = _app.installPackageByUrl(repo._cloneUrl, "latest", destinationPath);
                            std::string message = "successfully installed ";
                            message += repo._name;
                            _notifications.add(message);
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
        // TODO: tag selector
        if (BeginActions(1))
        {
            if (Button("close", ImVec2(buttonWidth, -1)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndChild();
        }

        ImGui::EndPopup();
    }
}
void gui::drawRecentProjects()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    if (ImGui::BeginTable("recentProjectsTable", 2, flags))
    {
        ImGui::TableSetupColumn("path", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        auto index = 0;
        for (auto p : _recentProjects)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(p._path.c_str());
            ImGui::TableSetColumnIndex(1);
            std::string removeButtonId = "remove##";
            removeButtonId += p._path;
            if (Button(removeButtonId.c_str()))
            {
                _recentProjects.erase(_recentProjects.begin() + index);
                ofJson recentProjects = ofJson::array();
                for (auto recentProject : _recentProjects)
                {
                    ofJson o;
                    o["path"] = recentProject._path;
                    recentProjects.push_back(o);
                }
                auto path = ofToDataPath("recentProjects.json");
                ofFile file(path, ofFile::ReadWrite);
                recentProjects >> file;
            }
            std::string buttonId = "configure##";
            buttonId += p._path;
            ImGui::SameLine();
            if (Button(buttonId.c_str()))
            {
                _projectPath = p._path;
                _stateMachine.trigger("configure");
            }
            index++;
        }

        ImGui::EndTable();
    }
}

void gui::drawHome()
{
    ImGui::TextWrapped("Welcome to ofPackageManager - a package manager for openFrameworks.\n");

    _animations.draw();

    //     ImTextureID textureID = (ImTextureID)(uintptr_t)_preview.getTexture().getTextureData().textureID;
    // auto size = ImGui::GetContentRegionAvail(); // for example
    // ImGui::Image(textureID, glm::vec2(_preview.getWidth(), _preview.getHeight()));
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
        ImGui::Text("choose a project from your local file system");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        PathChooser(_projectPath, _app.getMyAppsPath());
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 48);
        ImGui::Text("or paste a public git url in here to clone the project");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        char name[256] = "";
        strcpy(name, _openFromWebText.c_str());
        if (ImGui::InputText("##cloneUrl", name, IM_ARRAYSIZE(name)))
        {
            _openFromWebText = name;
        }
        if (!_openFromWebText.empty() &&  Button("choose destination and clone", ImVec2(0, 0), true))
        {
            auto result = ofSystemLoadDialog("path to projects", true, _app.getMyAppsPath());
            if (result.bSuccess)
            {
                auto path = result.getPath();
                if (ofDirectory::doesDirectoryExist(path))
                {
                    std::string projectName = ofFilePath::getBaseName(_openFromWebText);
                    std::string projectPath = ofFilePath::join(path, projectName);
                    ofxGit::repository repo(projectPath);
                    if (repo.clone(_openFromWebText))
                    {
                        _projectName = projectName;
                        _projectPath = projectPath;
                        _notifications.add("successfully cloned project. it is ready to be configured.");
                    }
                    else
                    {
                        _notifications.add("sorry, could not clone project.");
                    }
                }
            }
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 48);
        ImGui::Text("or select one of your recent projects");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        drawRecentProjects();
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
                ofJson recentProjects = ofJson::array();
                ofJson o;
                o["path"] = _projectPath;
                recentProjects.push_back(o);
                for (auto recentProject : _recentProjects)
                {
                    ofJson o;
                    o["path"] = recentProject._path;
                    recentProjects.push_back(o);
                }

                auto path = ofToDataPath("recentProjects.json");
                ofFile file(path, ofFile::ReadWrite);
                recentProjects >> file;
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
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("configure", ImVec2(-1, -footerHeight - padding)))
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        auto font = ImGui::GetFont();
        auto originalScale = font->Scale;
        font->Scale = 1.5f;
        ImGui::PushFont(font);
        ImGui::TextColored(ImGui::GetStyle().Colors[ImGuiCol_ButtonActive], _projectName.c_str());
        font->Scale = originalScale;
        ImGui::PopFont();
        ImGui::Text(_projectPath.c_str());
        ImGui::PopStyleColor();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);

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
                    // ofLogNotice() << addonsMakeText;
                    _notifications.add(
                        "successfully generated project");

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
void gui::updatePackagesLists()
{
    for (auto package : _app.getCorePackages())
    {
        if (_corePackages.count(package.getPath()) == 0)
        {
            _corePackages[package.getPath()] = selectablePackage(package, false);
        }
    }
    for (auto package : _app.getGloballyInstalledPackages())
    {
        if (_globalPackages.find(package.getPath()) == _globalPackages.end())
        {
            _globalPackages[package.getPath()] = selectablePackage(package, false);
        }
    }
    for (auto package : _app.getLocallyInstalledPackages())
    {
        if (_localPackages.find(package.getPath()) == _localPackages.end())
        {
            _localPackages[package.getPath()] = selectablePackage(package, false);
        }
    }
}
void gui::updateRecentProjectsList()
{
    auto path = ofToDataPath("recentProjects.json");
    ofFile file(path, ofFile::ReadOnly);
    ofJson data;
    if (file.exists())
    {
        file >> data;
        _recentProjects.clear();
        for (auto p : data)
        {
            _recentProjects.push_back(project(p["path"]));
        }
    }
}

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

void gui::onConfigureStateEntered(ofxStateEnteredEventArgs &args)
{
    _corePackages.clear();
    _globalPackages.clear();
    _localPackages.clear();
    updatePackagesLists();
    auto selectedPackages = _app.getPackagesListedInAddonsMakeFile();
    for (auto package : selectedPackages)
    {
        if (_corePackages.count(package.getPath()) > 0)
        {
            _corePackages[package.getPath()]._selected = true;
        }
        if (_globalPackages.count(package.getPath()) > 0)
        {
            _globalPackages[package.getPath()]._selected = true;
        }
        if (_localPackages.count(package.getPath()) > 0)
        {
            _localPackages[package.getPath()]._selected = true;
        }
    }
}