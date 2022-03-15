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
static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

gui::gui(ofPackageManager app) : ofBaseApp(), _app(app),
                                 _homeState(ofxState::create("home")),
                                 _manageGlobalPackagesState(ofxState::create("manage addons")),
                                 _createState(ofxState::create("create")),
                                 _updateState(ofxState::create("update")),
                                 _updateMultipleState(ofxState::create("updateMultiple")),
                                 _configureProjectState(ofxState::create("configureProject")),
                                 _fullscreen(false),
                                 _showAdvancedOptions(false),
                                 _showConsole(false),
                                 _showAvailablePackages(false),
                                 _showMetricsWindow(false),
                                 _showStyleEditor(false),
                                 _showDemoWindow(false),
                                 _aboutModalOpened(false),
                                 _preferencesModalOpened(false),
                                 _searchModalOpened(false),
                                 _deletePackageModalOpened(false),
                                 _projectDirectoryPath(app.getMyAppsPath()),
                                 _version(_app.getVersion()),
                                 _mostRecentVersion(_app.getNewestAvailableVersion()),
                                 _templates(_app.getTemplates()),
                                 _packagesDatabase(_app.getPackagesDatabase())
{
    _stateMachine.setInitialState(_homeState);
    _stateMachine.addTransition(_homeState, "create", _createState);
    _stateMachine.addTransition(_homeState, "manageGlobalPackages", _manageGlobalPackagesState);
    _stateMachine.addTransition(_homeState, "update", _updateState);
    _stateMachine.addTransition(_homeState, "updateMultiple", _updateMultipleState);
    _stateMachine.addTransition(_homeState, "configure", _configureProjectState);

    _stateMachine.addTransition(_manageGlobalPackagesState, "home", _homeState);
    _stateMachine.addTransition(_manageGlobalPackagesState, "create", _createState);
    _stateMachine.addTransition(_manageGlobalPackagesState, "update", _updateState);
    _stateMachine.addTransition(_manageGlobalPackagesState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_createState, "configure", _configureProjectState);
    _stateMachine.addTransition(_createState, "home", _homeState);
    _stateMachine.addTransition(_createState, "manageGlobalPackages", _manageGlobalPackagesState);
    _stateMachine.addTransition(_createState, "update", _updateState);
    _stateMachine.addTransition(_createState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_updateState, "configure", _configureProjectState);
    _stateMachine.addTransition(_updateState, "home", _homeState);
    _stateMachine.addTransition(_updateState, "manageGlobalPackages", _manageGlobalPackagesState);
    _stateMachine.addTransition(_updateState, "create", _createState);
    _stateMachine.addTransition(_updateState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_configureProjectState, "home", _homeState);
    _stateMachine.addTransition(_configureProjectState, "manageGlobalPackages", _manageGlobalPackagesState);
    _stateMachine.addTransition(_configureProjectState, "create", _createState);
    _stateMachine.addTransition(_configureProjectState, "update", _updateState);
    _stateMachine.addTransition(_configureProjectState, "updateMultiple", _updateMultipleState);

    _stateMachine.addTransition(_updateMultipleState, "home", _homeState);
    _stateMachine.addTransition(_updateMultipleState, "manageGlobalPackages", _manageGlobalPackagesState);
    _stateMachine.addTransition(_updateMultipleState, "create", _createState);
    _stateMachine.addTransition(_updateMultipleState, "update", _updateState);
    _stateMachine.addTransition(_updateMultipleState, "updateMultiple", _updateMultipleState);

    // TODO: add support for lambda functions to ofxStateMachine
    // _homeState->addEnteredListener([&]{ofLogNotice() << "home entered";});
    _homeState->addEnteredListener(this, &gui::onHomeStateEntered);
    _manageGlobalPackagesState->addEnteredListener(this, &gui::onManageGlobalPackagesEntered);
    _updateState->addEnteredListener(this, &gui::onUpdateProjectStateEntered);
    _configureProjectState->addEnteredListener(this, &gui::onConfigureStateEntered);
    _stateMachine.start();

    for (auto platform : _app.getPlatforms())
    {
        _platforms.push_back(selectableTarget(platform, ofGetTargetPlatform() == platform));
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

    _originalBuffer = std::cout.rdbuf(_consoleBuffer.rdbuf());
    updatePackagesLists();
    updateRecentProjectsList();
}

void gui::exit()
{
    std::cout.rdbuf(_originalBuffer);
}

void gui::update()
{
    auto frameNum = ofGetFrameNum();
    _notifications.update();

    if (_stateMachine.isCurrentState(_homeState))
    {
        _animations.update();
    }
    else if ((_stateMachine.isCurrentState(_manageGlobalPackagesState) || _stateMachine.isCurrentState(_configureProjectState)) && frameNum % 60 * 10 == 0)
    {
        updatePackagesLists();
    }
}

void gui::draw()
{
    _gui.begin();
    auto menuHeight = drawMainMenu().y;
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
                if (_stateMachine.isCurrentState(_manageGlobalPackagesState))
                {
                    drawManageGlobalPackages();
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
                    drawConfigureProject();
                }
                else if (_stateMachine.isCurrentState(_updateMultipleState))
                {
                    drawUpdateMultipleProjects();
                }

                if (_showStyleEditor)
                {
                    auto open = true;
                    if (ImGui::Begin("styleEditor", &open))
                    {
                        ImGui::ShowStyleEditor();
                    }
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
        if (_deletePackageModalOpened)
        {
            _deletePackageModalOpened = false;
            ImGui::OpenPopup("deletePackage");
        }
        drawModals();
        drawNotifications();
        ImGui::End();
    }

    _gui.end();
    _gui.draw();
}

ImVec2 gui::drawMainMenu()
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
            if (ImGui::MenuItem("Preferences", getShortCutLabel(",").c_str()))
            {
                _preferencesModalOpened = true;
            }
            if (ImGui::MenuItem("Quit", getShortCutLabel("q").c_str()))
            {
                ofExit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", getShortCutLabel("n").c_str()))
            {
                _stateMachine.trigger("create");
            }
            if (ImGui::MenuItem("Open", getShortCutLabel("o").c_str()))
            {
                _stateMachine.trigger("update");
            }
            if (ImGui::BeginMenu("Open Recent"))
            {
                for (auto p : _recentProjects)
                {
                    if (ImGui::MenuItem(p._path.c_str()))
                    {
                        _projectPath = p._path;
                        _projectName = ofFilePath::getBaseName(p._path);
                        _stateMachine.trigger("configure");
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("fullscreen", getShortCutLabel("f").c_str(), &_fullscreen))
            {
                ofToggleFullscreen();
            }
            ImGui::MenuItem("advanced options", NULL, &_showAdvancedOptions);
            ImGui::MenuItem("console", getShortCutLabel("c", true, true).c_str(), &_showConsole);
            if (_showAdvancedOptions)
            {
                ImGui::Separator();
                ImGui::MenuItem("metrics", NULL, &_showMetricsWindow);
                ImGui::MenuItem("style editor", NULL, &_showStyleEditor);
                ImGui::MenuItem("demo", NULL, &_showDemoWindow);
            }
            ImGui::EndMenu();
        }
        size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
    return size;
}
void gui::drawSideBar()
{
    ImGui::BeginChild("sideBar", ImVec2(200, 0));
    auto style = ImGui::GetStyle();
    auto numberOfButtons = _showAdvancedOptions ? 5 : 4;
    auto availableHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() + style.ItemSpacing.y;
    auto buttonSize = ImVec2(ImGui::GetContentRegionAvailWidth(), availableHeight / numberOfButtons - style.ItemSpacing.y);
    // auto buttonSize = ImVec2(ImGui::GetContentRegionAvailWidth(), footerHeight);
    if (MenuButton("home", buttonSize, _stateMachine.isCurrentState(_homeState)))
    {
        _stateMachine.trigger("home");
    }
    if (MenuButton("manage addons", buttonSize, _stateMachine.isCurrentState(_manageGlobalPackagesState)))
    {
        _stateMachine.trigger("manageGlobalPackages");
    }
    if (MenuButton("new project", buttonSize, _stateMachine.isCurrentState(_createState)))
    {
        _stateMachine.trigger("create");
    }
    if (MenuButton("open project", buttonSize, _stateMachine.isCurrentState(_updateState)))
    {
        _stateMachine.trigger("update");
    }
    if (_showAdvancedOptions)
    {
        if (MenuButton("update multiple projects", buttonSize, _stateMachine.isCurrentState(_updateMultipleState)))
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
    ImGui::SetNextWindowSize(ImVec2(ofGetWidth() * 0.5, 128));
    ImGui::SetNextWindowPos(ImVec2(ofGetWidth() * 0.5, 32));
    auto style = ImGui::GetStyle();
    auto open = true;
    if (ImGui::Begin("notifications", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
    {
        // ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(213.0 / 255, 54.0 / 255, 116.0 / 255, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16, 8));
        for (auto notification : _notifications.getData())
        {
            std::string id = "notification_";
            id += notification._id;
            if (ImGui::BeginChild(id.c_str(), ImVec2(0, 32)))
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);
                ImGui::TextWrapped(notification._message.c_str());
                ImGui::EndChild();
            }
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
        ImGui::End();
    }
}
void gui::drawConsole()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1.0f));

    ImGui::BeginChild("console", ImVec2(0, 0), false, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("console"))
        {
            if (ImGui::MenuItem("close"))
            {
                _showConsole = false;
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
        ImGui::EndMenuBar();
    }

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
            ImGui::SameLine();
            if (Button("refresh"))
            {
                _mostRecentVersion = _app.getNewestAvailableVersion();
            }

            std::string pgVersion = "ofxProjectGenerator commit: ";
            pgVersion += OFXPROJECTGENERATOR_COMMIT;
            ImGui::Text(pgVersion.c_str());

            std::string cwdPath = "cwd: ";
            cwdPath += _app.getCwdPath();
            ImGui::TextWrapped(cwdPath.c_str());

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
            ImGui::Text("TODO");
            ImGui::Text(_app.getOfPath().c_str());
            ImGui::EndChild();
        }
        if (BeginActions(1))
        {
            if (Button("close", ImVec2(buttonWidth, -1)))
            {
                ImGui::CloseCurrentPopup();
            }
            EndActions();
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
                if (ImGui::BeginTable("table1", 4, tableFlags))
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

                        std::string id = "open website##";
                        id += repo._url;
                        if (Button(id.c_str()))
                        {
                            ofLaunchBrowser(repo._url);
                        }
                        ImGui::SameLine();
                        if (_stateMachine.isCurrentState(_manageGlobalPackagesState))
                        {
                            id = "install globally##";
                        }
                        else
                        {
                            id = "install##";
                        }
                        auto isMissing = false;
                        for (auto &missingPackage : _missingPackages)
                        {
                            if (missingPackage.getPath() == repo._shortName)
                            {
                                isMissing = true;
                            }
                        }
                        if (isMissing)
                        {
                            ImGui::SameLine();
                            id = "install globally##";
                            id += repo._name;
                        }

                        id += repo._url;
                        if (Button(id.c_str()))
                        {
                            std::string destinationPath = ""; // locally by default
                            if (_stateMachine.isCurrentState(_manageGlobalPackagesState) || isMissing)
                            {
                                destinationPath = _app.getAddonsPath();
                            }
                            auto package = _app.installPackageByUrl(repo._cloneUrl, "latest", destinationPath);
                            updatePackagesLists(true);
                            updateSelectedPackages();
                            updateMissingPackages();
                            std::string message = "successfully installed ";
                            message += repo._name;
                            _notifications.add(message);
                        }
                        if (isMissing)
                        {
                            Tooltip("will install globally, since it is a missing dependency");
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
            EndActions();
        }

        ImGui::EndPopup();
    }
}
void gui::drawRecentProjects()
{
    if (ImGui::BeginTable("recentProjectsTable", 2, tableFlags))
    {
        ImGui::TableSetupColumn("path", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        auto index = 0;
        for (auto p : _recentProjects)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextWrapped(ofFilePath::getBaseName(p._path).c_str());
            Tooltip(p._path.c_str());
            ImGui::TableSetColumnIndex(1);
            std::string removeButtonId = "remove from list##";
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
            ImGui::SameLine();

            std::string buttonId = "open directory##";
            buttonId += p._path;
            if (Button(buttonId.c_str()))
            {
                openViaOfSystem(p._path);
            }

            buttonId = "configure##";
            buttonId += p._path;
            ImGui::SameLine();
            if (Button(buttonId.c_str()))
            {
                _projectPath = p._path;
                _projectName = ofFilePath::getBaseName(p._path);
                _stateMachine.trigger("configure");
            }
            index++;
        }

        ImGui::EndTable();
    }
}

void gui::drawHome()
{
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("home", ImVec2(-1, -footerHeight - padding)))
    {
        _animations.draw();
        ImGui::EndChild();
    }
    if (ImGui::BeginChild("homeActions"))
    {
        auto label = _animations.getCurrentLabel();
        auto url = _animations.getCurrentUrl();
        if (!label.empty() && !url.empty() && MinButton(label, ImGui::GetContentRegionAvail()))
        {
            ofLaunchBrowser(url);
        }
        ImGui::EndChild();
    }
}
void gui::drawManageGlobalPackages()
{
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("globalPackages", ImVec2(-1, -footerHeight - padding)))
    {
        if (ImGui::BeginTable("globalPackagesTable", 3, tableFlags))
        {
            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("author", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("actions", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
            for (auto corePackage : _corePackages)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(corePackage.second._package.getPath().c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("oF");
                ImGui::TableSetColumnIndex(2);
                std::string buttonId = "open directory##";
                buttonId += corePackage.second._package.toString();
                if (Button(buttonId.c_str()))
                {
                    openViaOfSystem(ofFilePath::join(_app.getAddonsPath(), corePackage.second._package.getPath()));
                }
                ImGui::SameLine();
                if (Button("remove##disabled", ImVec2(0, 0), false, true))
                {
                }
            }
            if (_globalPackages.size() > 0)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("---");
            }

            for (auto &package : _globalPackages)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(package.second._package.getPath().c_str());
                Tooltip(package.second._package.toString());
                ImGui::TableSetColumnIndex(1);
                ImGui::TableSetColumnIndex(2);
                std::string buttonId = "open directory##";
                buttonId += package.second._package.toString();
                if (Button(buttonId.c_str()))
                {
                    openViaOfSystem(ofFilePath::join(_app.getAddonsPath(), package.second._package.getPath()));
                }
                // ImGui::SameLine();
                // buttonId = "upgrade##";
                // buttonId += package.second._package.toString();
                // if(ImGui::Button(buttonId.c_str())){

                // }
                ImGui::SameLine();
                buttonId = "remove##";
                buttonId += package.second._package.toString();
                // TODO: probably makes sense to add a confirmation modal
                if (Button(buttonId.c_str()))
                {
                    if (ofDirectory::removeDirectory(ofFilePath::join(_app.getAddonsPath(), package.second._package.getPath()), true, false))
                    {
                        _notifications.add("successfully removed " + package.first);
                        _notifications.add("TODO: list still needs to be updated");
                        // _globalPackages.erase(package.first);
                    }
                }
            }
            if (_showAvailablePackages)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("---");

                for (auto it : ofJson::iterator_wrapper(_packagesDatabase))
                {
                    if (_globalPackages.count(it.key()) == 0)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(it.key().c_str());
                        std::string gitUrl = it.value()["git"];
                        Tooltip(gitUrl.c_str());
                        ImGui::TableSetColumnIndex(1);
                        std::string author = it.value()["author"];
                        ImGui::Text(author.c_str());
                        ImGui::TableSetColumnIndex(2);
                        std::string buttonId = "open website##";
                        buttonId += gitUrl;
                        if (Button(buttonId.c_str()))
                        {
                            std::string website = it.value()["website"];
                            ofLaunchBrowser(website);
                        }
                        ImGui::SameLine();
                        buttonId = "install##";
                        buttonId += gitUrl;
                        if (Button(buttonId.c_str()))
                        {
                            _app.installPackageById(it.key(), "latest", _app.getAddonsPath());
                        }
                    }
                }
            }
            ImGui::EndTable();
            ImGui::EndChild();
        }
    }
    if (BeginActions(2))
    {
        if (!_showAvailablePackages)
        {
            if (Button("show installable addons", ImVec2(buttonWidth, -1)))
            {
                _showAvailablePackages = true;
            }
        }
        else
        {
            if (Button("hide installable addons", ImVec2(buttonWidth, -1)))
            {
                _showAvailablePackages = false;
            }
        }
        ImGui::SameLine();
        if (Button("install additional addons", ImVec2(buttonWidth, -1)))
        {
            _searchModalOpened = true;
        }
        EndActions();
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
        if (Button("create and configure", ImVec2(buttonWidth, -1), true, !projectDirectory.exists() || projectDir.exists()))
        {
            auto dataPath = ofFilePath::getAbsolutePath(".");
            _app.generateProject(_projectPath);
            addToRecentProjects(_projectPath);
            _stateMachine.trigger("configure");
        }
        EndActions();
    }
}
void gui::drawUpdate()
{
    auto style = ImGui::GetStyle();
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("update", ImVec2(-1, -footerHeight - padding)))
    {
        auto indentation = 24;
        // ImGui::Text("choose a project from your local file system");
        // ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        PathChooser(_projectPath, _app.getMyAppsPath());
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 48);

        static char sfp[16 * 1024];
        strcpy(sfp, _sfp.c_str());
        ImGui::Text("git url or sfp (single file project)");
        if (ImGui::InputTextMultiline("##source", sfp, IM_ARRAYSIZE(sfp), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), 0))
        {
            _sfp = sfp;
        }
        if (Button("choose destination and import", ImVec2(0, 0), false, _sfp.empty()))
        {
            std::string defaultName = "sfp_";
            defaultName += ofGetTimestampString();
            if (!ofJson::accept(sfp))
            {
                defaultName = ofFilePath::getBaseName(sfp);
            }
            auto result = ofSystemSaveDialog(defaultName, "project path");
            if (result.bSuccess)
            {
                auto path = result.getPath();
                if (!ofDirectory::doesDirectoryExist(path))
                {
                    std::string projectPath = path;
                    std::string projectName = ofFilePath::getFileName(path);

                    if (ofJson::accept(_sfp))
                    {
                        if (_app.createFromSingleFileProject(ofJson::parse(_sfp), projectPath))
                        {
                            _projectPath = projectPath;
                            _projectName = projectName;
                            _notifications.add("successfully import SFP. it is ready to be configured.");
                        }
                        else
                        {
                            _notifications.add("sorry, could not import SFP.");
                        }
                    }
                    else
                    {
                        // TODO: check if git url
                        ofxGit::repository repo(projectPath);
                        if (repo.clone(ofTrim(_sfp)))
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
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 48);
        ImGui::Text("recent projects");
        drawRecentProjects();
        ImGui::EndChild();
    }
    if (BeginActions(1))
    {
        ofDirectory projectDir = ofDirectory(_projectPath);
        if (Button("configure", ImVec2(buttonWidth, -1), true, _projectPath.empty() || !projectDir.exists()))
        {
            _app.setProjectPath(_projectPath);
            auto dataPath = ofFilePath::getAbsolutePath(".");
            auto packages = _app.getPackagesListedInAddonsMakeFile();
            for (auto package : packages)
            {
                ofLogNotice() << package.toString();
            }
            _app.generateProject(_projectPath, packages);
            _projectName = ofFilePath::getBaseName(_projectPath);

            addToRecentProjects(_projectPath);

            _stateMachine.trigger("configure");
        }
        EndActions();
    }
}
void gui::drawUpdateMultipleProjects()
{
    auto style = ImGui::GetStyle();
    auto padding = ImGui::GetStyle().ItemInnerSpacing.y;
    if (ImGui::BeginChild("update", ImVec2(-1, -footerHeight - padding)))
    {
        ImGui::TextWrapped("This updates existing projects <em>recursively</em> in the selected directory. For example, to generate examples for a specific platform, you'd choose the examples folder as the update path.");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        PathChooser(_multipleProjectsDirectoryPath, _app.getMyAppsPath());
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        drawPlatformAndTemplateChooser();
        ImGui::EndChild();
    }

    if (BeginActions(1))
    {
        if (Button("update multiple projects", ImVec2(buttonWidth, -1), true, !ofDirectory::doesDirectoryExist(_multipleProjectsDirectoryPath, false)))
        {
            if (_app.recursivelyGenerateProjects(_multipleProjectsDirectoryPath))
            {
                _notifications.add("successfully updated projects");
            }
            else
            {
                _notifications.add("sorry, could not update projects. the output in the console might help to figure out what's going wrong.");
            }
        }
        EndActions();
    }
}
void gui::drawConfigureProject()
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
        ImGui::SameLine();
        if (Button("open directory##projectDir"))
        {
            openViaOfSystem(_projectPath);
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);

        if (ImGui::BeginChild("configureContent", ImVec2(0, 0)))
        {
            drawMissingPackages();
            if (_missingPackages.size())
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
            }
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
                    drawPlatformAndTemplateChooser();
                    ImGui::Unindent(indentation);
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
    if (BeginActions(3))
    {
        if (Button("install additional addons", ImVec2(buttonWidth, -1)))
        {
            _searchModalOpened = true;
        }
        ImGui::SameLine();
        if (Button("sfp to clipboard", ImVec2(buttonWidth, -1), false))
        {
            auto sfp = _app.generateSingleFileProject(_projectPath);
            ofSetClipboardString(sfp.dump(4));
        }
        // Tooltip("creates a single file project and copies it to the clipboard.");
        ImGui::SameLine();
        if (Button("generate project", ImVec2(buttonWidth, -1), true))
        {
            std::vector<ofPackage> ofPackages;
            std::vector<ofTargetPlatform> platforms;
            baseProject::Template template_;
            // add selected core packages
            for (auto &corePackage : _corePackages)
            {
                auto package = corePackage.second;
                if (package.isSelected())
                {
                    ofPackages.push_back(package._package);
                }
            }
            // add selected global packages
            for (auto &globalPackage : _globalPackages)
            {
                auto package = globalPackage.second;
                if (package.isSelected())
                {
                    ofPackages.push_back(package._package);
                }
            }
            // add selected local packages
            for (auto &localPackage : _localPackages)
            {
                auto package = localPackage.second;
                if (package.isSelected())
                {
                    ofPackages.push_back(package._package);
                }
            }
            for (auto &target : _platforms)
            {
                if (target.isSelected())
                {
                    platforms.push_back(target._target);
                }
            }

            if (_app.generateProject(_projectPath, ofPackages, platforms))
            {
                _notifications.add("successfully generated project");
            }
            else
            {
                _notifications.add("sorry, could not generate project. the output in the console might help to figure out what's going wrong.");
            }
        }
        EndActions();
    }
}
void gui::drawMissingPackages()
{
    if (_missingPackages.size())
    {
        ImGui::TextWrapped("ooops, there are some missing packages.");
        ImGui::SameLine();
        if (Button("refresh"))
        {
            updateMissingPackages();
        }

        if (ImGui::BeginTable("missingPackages", 2, tableFlags))
        {
            ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("actions", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            for (auto &missingPackage : _missingPackages)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(missingPackage.getPath().c_str());
                Tooltip(missingPackage.toString());
                ImGui::TableSetColumnIndex(1);
                if (!missingPackage.getUrl().empty())
                {
                    std::string buttonId = "install##";
                    buttonId += missingPackage.toString();
                    if (Button(buttonId.c_str()))
                    {
                        _app.installPackageByUrl(missingPackage.getUrl(), missingPackage.getCheckout());
                        updateMissingPackages();
                        updateSelectedPackages();
                    }
                }
                else
                {
                    std::string buttonId = "search##";
                    buttonId += missingPackage.toString();
                    if (Button(buttonId.c_str()))
                    {
                        _queryText = missingPackage.getPath();
                        _searchResults = _app.searchPackageOnGithubByName2(_queryText);
                        _searchModalOpened = true;
                        // _app.installPackageByUrl(missingPackage.getUrl(), missingPackage.getCheckout(), missingPackage.getPath());
                        // _missingPackages = _app.getMissingPackages();
                    }
                }
            }
            ImGui::EndTable();
        }
    }
}
void gui::drawPlatformAndTemplateChooser()
{
    auto selectedTargets = std::vector<ofTargetPlatform>();
    std::vector<std::string> selectedPlatformStrings;
    for (auto &target : _platforms)
    {
        ImGui::Checkbox(getTargetString(target._target).c_str(), &target._selected);
        if (target.isSelected())
        {
            selectedTargets.push_back(target._target);
            selectedPlatformStrings.push_back(getTargetString(target._target));
        }
    }

    // sorry. it was quite late when i wrote this, probably the most inefficent way to find templates which are available for all selcted platforms
    // also, this should not be done every frame
    std::vector<baseProject::Template> filteredTemplates;

    for (auto &selectedPlatform : selectedTargets)
    {
        for (auto templateForSelectedPlatform : _templates[selectedPlatform])
        {
            auto availableForAllSelectedPlatforms = true;
            for (auto &selectedPlatform : selectedTargets)
            {
                if (!ofContains(templateForSelectedPlatform.platforms, getTargetString(selectedPlatform)))
                {
                    availableForAllSelectedPlatforms = false;
                }
            }
            if (availableForAllSelectedPlatforms)
            {
                auto alreadyIncluded = false;
                for (auto filteredTemplate : filteredTemplates)
                {
                    if (filteredTemplate.description == templateForSelectedPlatform.description)
                    {
                        alreadyIncluded = true;
                    }
                }
                if (!alreadyIncluded)
                {
                    filteredTemplates.push_back(templateForSelectedPlatform);
                }
            }
        }
    }
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    if (ImGui::BeginCombo("##template", _selectedTemplate.description.empty() ? "default template" : _selectedTemplate.description.c_str(), 0))
    {
        auto is_selected = _selectedTemplate.description.empty();
        if (ImGui::Selectable("default template", is_selected))
        {
            _selectedTemplate = baseProject::Template();
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        for (auto &selectedTarget : selectedTargets)
        {
            for (auto template_ : filteredTemplates)
            {
                auto is_selected = template_.description == _selectedTemplate.description;
                if (ImGui::Selectable(template_.description.c_str(), is_selected))
                {
                    _selectedTemplate = template_;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }

        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
}

void gui::keyPressed(int key) {}
void gui::keyReleased(int key)
{
    auto commandKeyPressed = false;
    auto shiftKeyPressed = ofGetKeyPressed(OF_KEY_SHIFT);
#ifdef TARGET_OSX
    commandKeyPressed = ofGetKeyPressed(OF_KEY_COMMAND);
#endif
#ifdef TARGET_WIN
    commandKeyPressed = ofGetKeyPressed(OF_KEY_CTRL);
#endif
#ifdef TARGET_LINUX
    commandKeyPressed = ofGetKeyPressed(OF_KEY_CTRL);
#endif
    if (!commandKeyPressed)
    {
        return;
    }
    switch (key)
    {
    case ',':
    {
        _preferencesModalOpened = true;
        break;
    }
    case 'c':
    {
        if (shiftKeyPressed)
        {
            _showConsole = !_showConsole;
        }
        break;
    }
    case 'f':
    {
        _fullscreen = !_fullscreen;
        ofToggleFullscreen();
        break;
    }
    case 'n':
    {
        _stateMachine.trigger("create");
        break;
    }
    case 'o':
    {
        _stateMachine.trigger("update");
        break;
    }
    case 'q':
    {
        ofExit();
        break;
    }
    }
}
void gui::mouseMoved(int x, int y) {}
void gui::mouseDragged(int x, int y, int button) {}
void gui::mousePressed(int x, int y, int button) {}
void gui::mouseReleased(int x, int y, int button) {}
void gui::mouseEntered(int x, int y) {}
void gui::mouseExited(int x, int y) {}
void gui::windowResized(int w, int h) {}
void gui::dragEvent(ofDragInfo dragInfo) {}
void gui::gotMessage(ofMessage msg) {}
void gui::updatePackagesLists(bool clear)
{
    if (clear)
    {
        _corePackages.clear();
        _globalPackages.clear();
        _localPackages.clear();
    }
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
void gui::updateSelectedPackages()
{
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
void gui::updateMissingPackages()
{
    _missingPackages = _app.getMissingPackages();
}
void gui::openViaOfSystem(std::string path)
{
    std::string command;
#ifdef TARGET_OSX
    command = "open ";
#endif
#ifdef TARGET_WIN
    command = "start ";
#endif
#ifdef TARGET_LINUX
    command = "xdg-open ";
#endif
    command += path;
    // TODO: is there another way then ofSystem
    ofSystem(command);
}
void gui::addToRecentProjects(std::string path)
{
    ofJson recentProjects = ofJson::array();
    ofJson o;
    o["path"] = path;
    recentProjects.push_back(o);
    for (auto recentProject : _recentProjects)
    {
        ofJson o;
        o["path"] = recentProject._path;
        recentProjects.push_back(o);
    }
    // TODO: remove duplicates

    auto recentProjectsPath = ofToDataPath("recentProjects.json");
    ofFile file(recentProjectsPath, ofFile::ReadWrite);
    if (!file.exists())
    {
        file.create();
    }
    recentProjects >> file;
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
    // TODO: reset platforms
    _selectedTemplate = baseProject::Template();
}
void gui::onManageGlobalPackagesEntered(ofxStateEnteredEventArgs &args)
{
    _missingPackages.clear();
}
void gui::onUpdateProjectStateEntered(ofxStateEnteredEventArgs &args)
{
    _sfp.clear();
    _projectPath.clear();
    _projectName.clear();
    updateRecentProjectsList();
}

void gui::onConfigureStateEntered(ofxStateEnteredEventArgs &args)
{
    _app.setProjectPath(_projectPath);
    updatePackagesLists(true);
    updateSelectedPackages();
    updateMissingPackages();
}