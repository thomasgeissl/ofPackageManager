#!/bin/sh

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

STRINGTOADD="PATH="$SCRIPTPATH"/../bin/ofPackageManager_debug.app/Contents/MacOS:""$""PATH"
echo $STRINGTOADD >> ~/.bash_profile
echo "export PATH">> ~/.bash_profile


