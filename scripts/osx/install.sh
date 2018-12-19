#!/bin/sh

curl -O https://openframeworks.cc/versions/v0.10.1/of_v0.10.1_osx_release.zip
unzip -a -qq of_v0.10.1_osx_release.zip
rm of_v0.10.1_osx_release.zip
cd of_v0.10.1_osx_release/apps
mkdir ofPackageManager && cd ofPackageManager
git clone https://github.com/thomasgeissl/ofxPackageManager.git
cd ofPackageManager/scripts
./install_dependencies.sh
cd ..
make
cd scripts
./fix_dylib.sh