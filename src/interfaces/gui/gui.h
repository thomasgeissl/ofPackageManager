#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxStateMachine.h"
#include "../../ofApp.h"
#include "generators/projectGenerator/ofProjectGenerator.h"
#include "../../ghRepo.h"

enum OFPACKAGEMANAGER_GUI_SIZE {
    SMALL,
    MEDIUM,
    BIG
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
    baseProject::Template _template;
    bool _selected;
    friend bool operator== ( const selectableTemplate &left, const selectableTemplate &right){
        return left._template.dir.path() == right._template.dir.path();
    }
};
class gui : public ofBaseApp
{
public:
    gui(ofPackageManager app);
    void setup();
    void update();
    void draw();
    ImVec2 drawMenuGui();
    void drawHeader();
    void drawFooter();
    void drawModals();
    void drawRecentProjects();
    void drawHomeState();
    void drawInstallState();
    void drawNewState();
    void drawUpdateState();
    void drawUpdateMultipleState();
    void drawConfigureState();


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

    bool Button(std::string text, OFPACKAGEMANAGER_GUI_SIZE size = OFPACKAGEMANAGER_GUI_SIZE::SMALL, bool primary = false);
    void Path(std::string & path);

private:
    ofPackageManager _app;
    ofxImGui::Gui _gui;
    ofxStateMachine _stateMachine;
    ofxState::pointer _homeState;
    ofxState::pointer _installState;
    ofxState::pointer _newState;
    ofxState::pointer _updateState;
    ofxState::pointer _updateMultipleState;
    ofxState::pointer _configureProjectState;

    std::string _projectDirectoryPath;
    std::string _projectName;
    std::string _projectPath;

    std::string _queryText;
    std::vector<ghRepo> _searchResults;
    ghRepo _selectedSearchResult;

    std::map<string, selectablePackage> _corePackages;
    std::map<string, selectablePackage> _globalPackages;
    std::map<string, selectablePackage> _localPackages;


    std::vector<selectableTarget> _targets;
    std::map<ofTargetPlatform, std::vector<selectableTemplate>> _templates;


    bool _showStyleEditor;
    bool _showDemoWindow;
};
