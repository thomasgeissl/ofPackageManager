#!/bin/sh
OFVERSION=0.11.0
GHUSER=thomasgeissl
GHREPO=ofPackageManager

curl -O https://openframeworks.cc/versions/v${OFVERSION}/of_v${OFVERSION}_osx_release.zip
unzip -a -qq of_v${OFVERSION}_osx_release.zip
rm of_v${OFVERSION}_osx_release.zip
cd of_v${OFVERSION}_osx_release/apps
mkdir packageManager
cd packageManager

git clone https://github.com/${GHUSER}/${GHREPO}.git
cd $GHREPO
cd scripts

./install_dependencies.sh
cd ..
make
cd scripts
./fix_dylib.sh