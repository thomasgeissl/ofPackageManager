#!/bin/bash

PWD=${pwd}
TASK="install"

# readarray addons < addons.make
IFS=$'\n' ADDONS=($(cat addons.make))
for i in "${ADDONS[@]}"
do
    IFS=$'#,@' ADDON=($i)

    if [ ${#ADDON[@]} -eq 3 ]; then
        echo "size == 3"
        git clone ${ADDON[1]} ${ADDON[0]}
        cd ${ADDON[0]} && git checkout ${ADDON[2]}
        cd $PWD
    fi
done