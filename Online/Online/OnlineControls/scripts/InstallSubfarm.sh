#!/bin/bash
cd `dirname $0`/../cmt
source PVSS.setup.sh
unset PVSS_II
start=`date`
echo "Installation starting: `date`"
export PVSS_II=/localdisk/pvss/${1}/config/config
`which python` ${ONLINECONTROLSROOT}/scripts/Install.py COPY  -project ${1} -number ${2}
`which python` ${ONLINECONTROLSROOT}/scripts/Install.py COPY2 -project ${1} -number ${2}
echo "Installation started: ${start}   finished: `date`"
