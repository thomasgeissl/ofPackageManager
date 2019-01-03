#!/bin/bash

pwd=$(pwd)
TASK="install"

if [ $# -gt 0 ]; then
    TASK=$1
fi

if [ $TASK="install" ]; then
    # readarray addons < addons.make
    IFS=$'\n' ADDONS=($(cat addons.make))
    for i in "${ADDONS[@]}"
    do
        IFS=$'#,@' ADDON=($i)
        if [ ${#ADDON[@]} -eq 3 ]; then
            cd $pwd
            git clone ${ADDON[1]} ${ADDON[0]}
            cd ${ADDON[0]} && git checkout ${ADDON[2]}
            # TODO: parse addon dependencies, neccessary?
        fi
    done
fi