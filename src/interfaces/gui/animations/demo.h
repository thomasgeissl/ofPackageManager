#pragma once
#include "./base.h"

class demo : public animation::base {
    public:
    demo() : base(){

    }
    void update(){
        _fbo.begin();
        ofClear(ofColor(255,0,0));
        ofSetColor(ofColor(0,255,255));
        ofDrawRectangle(0,0,100, 100);
        _fbo.end();
    }
};