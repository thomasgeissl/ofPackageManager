#!/bin/sh
cd "$(dirname "$0")"

mkdir test 
cd test
APP=../../bin/ofPackageManager.app/Contents/MacOS/ofPackageManager 
echo "testing info task:"
$APP info
echo ""


echo "testing install task with id and tag"
$APP install ofxMidi@1.1.1 > /dev/null


echo "testing install task with github name"
$APP install arturoc/ofxEasing > /dev/null
echo ""


echo "local addons should now contain two addons:"
ls ./local_addons
COUNTER=$(ls local_addons | wc -l)
echo $COUNTER
if [ "$COUNTER" -ne 2 ]
then
    exit -1
fi
echo ""


echo "after deleting the local_addons dir and reinstalling it should again contain these two addons:"
rm -rf ./local_addons
$APP install > /dev/null
ls ./local_addons
COUNTER=$(ls local_addons | wc -l)
echo $COUNTER
if [ "$COUNTER" -ne 2 ]
then
    exit -1
fi
echo ""


echo "clear addons.make and readding already installed packages, it should now contain the two addons with their urls and hashes"
>./addons.make
$APP add -A local_addons > /dev/null
cat addons.make
echo ""


echo "testing install task from addons.make, it should again contain two addons"
rm -rf local_addons
$APP install > /dev/null
COUNTER=$(ls local_addons | wc -l)
echo $COUNTER
if [ "$COUNTER" -ne 2 ]
then
    exit -1
fi
echo ""


echo "so far, so good. the next task will be interactive and will write quite a lot on your console"


echo "testing install task with addon with depedencies, it should again contain six addons"
sleep 5
$APP install https://github.com/thomasgeissl/ofxPackageManageableExampleAddon.git
COUNTER=$(ls local_addons | wc -l)
echo $COUNTER
if [ "$COUNTER" -ne 6 ]
then
    exit -1
fi
sleep 2
echo ""
echo ""

echo "congratulations. successfully run the test."

# TODO: search tasks, global install