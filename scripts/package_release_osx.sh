#!/bin/sh
mkdir ../../tmp && cd ../../tmp
cp -R ../../ofPackageManager ofPackageManager
zip -r ofPackageManager_osx_version.zip ofPackageManager
openssl dgst -sha256 ofPackageManager_osx_version.zip