#pragma once
#include "./base.h"

class demo : public animation::base {
    public:
    demo() : base("demo collaborator", "https://github.com/thomasgeissl/ofPackageManager/"){

    }
    void update(){
        auto colors = ImGui::GetStyle().Colors;
        auto time = ofGetElapsedTimef();
        begin();
        ofSetRectMode(OF_RECTMODE_CENTER);
        ofNoFill();
        ofSetLineWidth(2);
        for(auto x = 0 ; x < _fbo.getWidth(); x+=4){
            auto y = _fbo.getHeight()/2 + std::sin(x * time/1000)*_fbo.getHeight()/10;
            ofSetColor(ofColor(0,ofMap(std::sin(x*time/200), -1, 1, 0, 255),255));
            ofDrawRectangle(x,y,100, 100);
        }
        end();
    }
};