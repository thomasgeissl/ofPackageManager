#!/bin/sh
cd "$(dirname "$0")"
cd ..
make
cd scripts
./fix_dylib.sh

# TODO: run tests

mkdir ../../tmp
cd ../../tmp
rm -rf *
cp -R ../ofPackageManager ofPackageManager
zip -r ofPackageManager_osx_version.zip ofPackageManager
openssl dgst -sha256 ofPackageManager_osx_version.zip

# TODO: get version from defines.h
MAJOR=0
MINOR=0
PATCH=0
mv ofPackageManager_osx_version.zip ofPackageManager_osx_$MAJOR.$MINOR.$PATCH.zip