#!/bin/bash
cd `dirname ${0}`
alias cmt='${CMTROOT}/${CMTBIN}/cmt.exe'
args="${1} ${2} ${3} ${4} ${5} ${6}"
shift;
shift;
shift;
shift;
shift;
shift;
if test ! -e setup.sh;
then 
cmt config 2&> /dev/null
fi;
rm -rf /dev/shm/* 2&>/dev/null
. setup.sh 2&> /dev/null
. preamble.sh 2&> /dev/null
#  echo "Executing:   $args "
eval ${args}

