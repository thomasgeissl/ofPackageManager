#!/bin/sh
mkdir ../../tmp
cd ../../tmp
rm -rf *
cp -R ../../ofPackageManager ofPackageManager
zip -r ofPackageManager_osx_version.zip ofPackageManager
openssl dgst -sha256 ofPackageManager_osx_version.zip