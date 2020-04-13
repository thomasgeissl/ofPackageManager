#!/bin/bash

pwd=$(pwd)
TASK="install"

if [ $# -gt 0 ]; then
    TASK=$1
fi

if [ $TASK="install" ]; then
    # readarray addons < addons.make
    if [ ! -f "addons.make" ]; then
        cd $pwd
        exit 0
    fi
    IFS=$'\n' ADDONS=($(cat addons.make))
    for i in "${ADDONS[@]}"
    do
        IFS=$'#,@' ADDON=($i)
        if [ ${#ADDON[@]} -eq 3 ]; then
            cd $pwd
            DESTINATION="$(echo -e "${ADDON[0]}" | sed -e 's/[[:space:]]*$//')"
            git clone ${ADDON[1]} ${DESTINATION}
            cd ${DESTINATION} && git checkout ${ADDON[2]}
            # TODO: parse addon dependencies, neccessary?
        fi
    done
fi
cd $pwd