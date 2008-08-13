#!/bin/bash
cd /home/frankm/cmtuser/Gaudi_v19r2/Online/OnlineControls/cmt
source PVSS.setup.sh
unset PVSS_II
start=`date`
echo "Installation starting: `date`"
`which python` ${ONLINECONTROLSROOT}/scripts/Install.py COPY -project $1 -number $2
`which python` ${ONLINECONTROLSROOT}/scripts/Install.py COPY2 -project $1 -number $2
echo "Installation started: ${start}   finished: `date`"
