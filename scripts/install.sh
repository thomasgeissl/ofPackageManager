#!/bin/sh
# TODO: automatically get os
VERSION=0.11.0
echo "What OS are you using? (0): OSX, (1): LINUX, (any other): WINDOWS"
read -r OS
if [ $OS = 0 ]
then 
    curl -O https://openframeworks.cc/versions/v${VERSION}/of_v${VERSION}_osx_release.zip
    unzip -a -qq of_v${VERSION}_osx_release.zip
    rm of_v${VERSION}_osx_release.zip
    cd of_v${VERSION}_osx_release/apps
    mkdir packageManager
    cd packageManager
elif [ $OS = 1 ]
then
        curl -O https://openframeworks.cc/versions/v${VERSION}/of_v${VERSION}_linux64gcc6_release.tar.gz
        tar -zxf of_v${VERSION}_linux64gcc6_release.tar.gz
        rm of_v${VERSION}_linux64gcc6_release.tar.gz
        of_v${VERSION}_linux64gcc6_release/scripts/linux/ubuntu/install_dependencies.sh
        cd of_v${VERSION}_linux64gcc6_release/apps
        mkdir packageManager
        cd packageManager
else echo "windows support is not yet implemented" && exit
fi

git clone https://github.com/thomasgeissl/ofPackageManager.git
cd ofPackageManager/scripts
./install_dependencies.sh
cd .. && make -j4
if [ $OS = 0 ]
then 
cd scripts && ./fix_dylib.sh
fi