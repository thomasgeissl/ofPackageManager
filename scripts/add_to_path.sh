#!/bin/sh

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

STRINGTOADD="PATH="$SCRIPTPATH"/../bin/ofPackageManager.app/Contents/MacOS:""$""PATH"
echo $STRINGTOADD >> ~/.bash_profile
echo "export PATH">> ~/.bash_profile


