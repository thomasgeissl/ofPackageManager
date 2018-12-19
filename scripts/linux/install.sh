#!/bin/sh

curl -O https://openframeworks.cc/versions/v0.10.1/of_v0.10.1_linux64gcc6_release.tar.gz
tar -zxf of_v0.10.1_linux64gcc6_release.tar.gz
rm of_v0.10.1_linux64gcc6_release.tar.gz
sudo of_v0.10.1_linux64gcc6_release/scripts/linux/ubuntu/install_dependencies.sh -y
sudo apt install libgit2
cd of_v0.10.1_linux64gcc6_release/apps
mkdir ofPackageManager && cd ofPackageManager
git clone https://github.com/thomasgeissl/ofxPackageManager.git
cd ofPackageManager/scripts
./install_dependencies.sh
cd ..
make