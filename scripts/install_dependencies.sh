#!/bin/sh

cd "$(dirname "$0")"
cd ..
bash -c "$(curl -sSL https://raw.githubusercontent.com/thomasgeissl/ofPackageManager/master/scripts/ofPackageManager.sh)" install