#!/bin/sh
OFVERSION=0.11.2
GHUSER=thomasgeissl
GHREPO=ofPackageManager

curl -O https://openframeworks.cc/versions/v${OFVERSION}/of_v${OFVERSION}_linux64gcc6_release.tar.gz
tar -zxf of_v${OFVERSION}_linux64gcc6_release.tar.gz
rm of_v${OFVERSION}_linux64gcc6_release.tar.gz
sudo of_v${OFVERSION}_linux64gcc6_release/scripts/linux/ubuntu/install_dependencies.sh -y
sudo of_v${OFVERSION}_linux64gcc6_release/scripts/linux/ubuntu/install_codecs.sh -y
cd of_v${OFVERSION}_linux64gcc6_release/apps
mkdir packageManager
cd packageManager

git clone https://github.com/${GHUSER}/${GHREPO}.git
cd $GHREPO
cd scripts

./install_dependencies.sh
cd ..
make -j4