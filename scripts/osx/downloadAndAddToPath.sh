#!/bin/sh
MAJOR=0
MINOR=0
PATCH=0

curl -sSL https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/src/defines.h -o tmp_defines.h
FILE="tmp_defines.h"

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
rm tmp_defines.h
echo ofPackageManager_osx_${MAJOR}.${MINOR}.${PATCH}.zip
mkdir tmp
cd tmp
curl -LOk https://github.com/thomasgeissl/ofPackageManager/releases/download/${MAJOR}.${MINOR}.${PATCH}/ofPackageManager_osx_${MAJOR}.${MINOR}.${PATCH}.zip --output ./ofPackageManager_osx_${MAJOR}.${MINOR}.${PATCH}.zip
unzip -qq ofPackageManager_osx_${MAJOR}.${MINOR}.${PATCH}.zip

read -p "In which directory do you want to install the package manager? ("$HOME"/Applications): " DEST
[ -z "${DEST}" ] && DEST=$HOME'/Applications'

echo $DEST
if [ -d "${DEST}/ofPackageManager.app" ]; then
    echo "There is already a version of ofPackageManager installed in this directory. Please delete it, make a backup and try again."
else
    mkdir ${DEST}/ofPackageManager.app
    cp -R ofPackageManager/bin/ofPackageManager.app/* ${DEST}/ofPackageManager.app
    echo "\n\nSuccessfully downloaded and unpacked ofPackageManager."
    echo "\nPlease manually add "${DEST}"/ofPackageManager.app/Contents/MacOS/ to your PATH variable"
    # echo 'export PATH='${DEST}/ofPackageManager.app/Contents/MacOS/':$PATH' >> ~/.bash_profile
    # echo $PATH
    # source ~/.bash_profile
fi

cd ..
rm -rf tmp
