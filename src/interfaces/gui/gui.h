#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxStateMachine.h"
#include "../../ofApp.h"
#include "generators/projectGenerator/ofProjectGenerator.h"
#include "../../ghRepo.h"
#include "./notifications.h"
#include "./console.h"

class project
{
public:
    project(std::string path) : _path(path)
    {
    }
    std::string _path;
};
class selectablePackage
{
public:
    selectablePackage() {}
    selectablePackage(ofPackage package, bool selected)
    {
        _package = package;
        _selected = selected;
    }
    bool isSelected()
    {
        return _selected;
    }
    ofPackage _package;
    bool _selected;
};
class selectableTarget
{
public:
    selectableTarget() {}
    selectableTarget(ofTargetPlatform target, bool selected)
    {
        _target = target;
        _selected = selected;
    }
    bool isSelected()
    {
        return _selected;
    }
    ofTargetPlatform _target;
    bool _selected;
};
class selectableTemplate
{
public:
    selectableTemplate() {}
    selectableTemplate(baseProject::Template template_, bool selected)
    {
        _template = template_;
        _selected = selected;
    }
    bool isSelected()
    {
        return _selected;
    }
    friend bool operator==(const selectableTemplate &left, const selectableTemplate &right)
    {
        return left._template.dir.path() == right._template.dir.path();
    }
    baseProject::Template _template;
    bool _selected;
};

class gui : public ofBaseApp
{
public:
    gui(ofPackageManager app);
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void updatePackagesLists(bool clear = false);
    void updateSelectedPackages();
    void updateRecentProjectsList();
    void updateMissingPackages();
    void updatePreferences();
    void savePreferences(bool showAdvancedOptions);

    void openViaOfSystem(std::string path);
    void addToRecentProjects(std::string path);

private:
    ofPackageManager _app;
    ofxImGui::Gui _gui;
    notifications _notifications;
    console _console;

    // statemachine
    ofxStateMachine _stateMachine;
    ofxState::pointer _homeState;
    ofxState::pointer _manageGlobalPackagesState;
    ofxState::pointer _createState;
    ofxState::pointer _updateState;
    ofxState::pointer _updateMultipleState;
    ofxState::pointer _configureProjectState;
    void onConfigureProjectStateEntered(ofxStateEnteredEventArgs &args);
    void onManageGlobalPackagesEntered(ofxStateEnteredEventArgs &args);
    void onUpdateProjectStateEntered(ofxStateEnteredEventArgs &args);
    void onConfigureStateEntered(ofxStateEnteredEventArgs &args);
    void onConsoleClose();


    std::string _projectDirectoryPath;
    std::string _multipleProjectsDirectoryPath;
    std::string _projectName;
    std::string _projectPath;

    std::string _sfp;
    std::string _queryText;
    std::vector<ghRepo> _searchResults;
    ghRepo _selectedSearchResult;

    std::map<string, selectablePackage> _corePackages;
    std::map<string, selectablePackage> _globalPackages;
    std::map<string, selectablePackage> _localPackages;
    ofJson _packagesDatabase;

    std::vector<selectableTarget> _platforms;
    baseProject::Template _selectedTemplate;
    std::map<ofTargetPlatform, std::vector<baseProject::Template> > _templates;
    std::vector<project> _recentProjects;
    std::vector<ofPackage> _missingPackages;
    std::vector<std::string> _additionalSources;

    // preferences
    bool _showAdvancedOptionsPreference;

    // view options
    bool _fullscreen;
    bool _showAdvancedOptions;
    bool _showStyleEditor;
    bool _showDemoWindow;
    bool _showMetricsWindow;
    bool _showConsole;
    bool _showAvailablePackages;

    // temp modal state variables
    bool _aboutModalOpened;
    bool _preferencesModalOpened;
    bool _searchModalOpened;
    bool _deletePackageModalOpened;
    bool _closeCurrentModal;
    bool _importModelOpened;


    // versions
    ofVersion _version;
    ofVersion _mostRecentVersion;

    // ui elements
    ImVec2 drawMainMenu();
    void drawNotifications();
    void drawSideBar();
    void drawConsole();
    void drawModals();
    bool drawAboutModal();
    bool drawPreferencesModal();
    bool drawImportModal();
    bool drawSearchModal();
    void drawRecentProjects();
    void drawManageGlobalPackages();
    void drawCreate();
    void drawUpdate();
    void drawUpdateMultipleProjects();
    void drawConfigureProject();
    void drawMissingPackages();
    void drawPlatformAndTemplateChooser();
};
