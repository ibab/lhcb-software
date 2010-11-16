#!/bin/sh

if [ $# -eq 1 ]; then 
    pkill xterm
    echo $1
    . setup.sh && . testWriter.sh && . injectData.sh $1 
fi   



