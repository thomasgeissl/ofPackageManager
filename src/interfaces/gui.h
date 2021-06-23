#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxStateMachine.h"
#include "ofApp.h"

class gui : public ofBaseApp
{
public:
    gui(ofPackageManager app);
    void setup();
    void update();
    void draw();
    ImVec2 drawMenuGui();
    void drawRecentProjects();
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

private:
    ofPackageManager _app;
    ofxImGui::Gui _gui;
    ofxStateMachine _stateMachine;
    ofxState::pointer _welcomeState;
    ofxState::pointer _newState;
    ofxState::pointer _updateState;
    ofxState::pointer _updateMultipleState;
};
