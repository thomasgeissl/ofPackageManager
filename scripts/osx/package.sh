#!/bin/sh
cd "$(dirname "$0")"
cd ../..

mkdir -p ../tmp
cd ../tmp

pwd
rm -rf *
cp -R ../ofPackageManager ofPackageManager
rm -rf ofPackageManager/frontend
zip -r ofPackageManager_osx_version.zip ofPackageManager
openssl dgst -sha256 ofPackageManager_osx_version.zip

MAJOR=0
MINOR=0
PATCH=0

FILE="../ofPackageManager/src/defines.h"
while read line || [ -n "$line" ]; do 
    IFS=', ' read -r -a ARRAY <<< "$line";
    LENGTH="${#ARRAY[@]}"
    if [ $LENGTH -eq 3 ]
    then
        if [ ${ARRAY[1]} = "OFAPP_MAJOR_VERSION" ]
        then
            MAJOR=${ARRAY[2]}
        fi
        if [ ${ARRAY[1]} = "OFAPP_MINOR_VERSION" ]
        then
            MINOR=${ARRAY[2]}
        fi
        if [ ${ARRAY[1]} = "OFAPP_PATCH_VERSION" ]
        then
            PATCH=${ARRAY[2]}
        fi
    fi
done < $FILE
echo $MAJOR.$MINOR.$PATCH


pwd && ls

# rename and move to home
mv ofPackageManager_osx_version.zip $HOME/ofPackageManager_osx_$MAJOR.$MINOR.$PATCH.zip

ls $HOME
