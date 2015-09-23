#!/bin/bash
#echo "Args: $* ";
#cd `dirname ${0}`
#alias cmt='${CMTROOT}/${CMTBIN}/cmt.exe'
args="${1} ${2} ${3} ${4} ${5} ${6}"
for i in $args;do shift;done;
#if test ! -e setup.sh;
#then 
#cmt config 2&> /dev/null
#fi;
rm -rf /dev/shm/bm_* &>/dev/null
#. setup.sh 2&> /dev/null
. ../../cmt/preamble.sh &> /dev/null
#  echo "Executing:   $args "
#
exec ${args}
