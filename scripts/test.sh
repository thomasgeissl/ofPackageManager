#!/bin/sh

mkdir test && cd test
ofPackageManager info
ofPackageManager install ofxMidi@1.1.1
ofPackageManager install arturoc/ofxEasing
rm -rf local_addons
ofPackageManager install
