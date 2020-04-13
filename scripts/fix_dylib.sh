#!/bin/sh
cwd=$(pwd)
cd "$(dirname "$0")"

cp ../local_addons/ofxGit2/libs/libgit2/lib/osx/libgit2.27.dylib ../bin/ofPackageManager.app/Contents/MacOS/
install_name_tool -change @rpath/libgit2.27.dylib @executable_path/libgit2.27.dylib ../bin/ofPackageManager.app/Contents/MacOS/ofPackageManager

cd $pwd
