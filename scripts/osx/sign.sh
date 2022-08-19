#!/bin/sh
cd "$(dirname "$0")"
codesign --force --deep --sign "Developer ID Application: Thomas Geissl (DP9DFGMC65)" ../../bin/ofPackageManager.app
codesign -dv --verbose=4 ../../bin/ofPackageManager.app