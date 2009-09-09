#!/bin/bash
#
# Creates a new warnings.out file to uses.
#
# $1 optional argument with give host:port, where :port is optional.
#
# The script will use the localhost by default but if a second
# command line argument then that will be taken as the host.
#
if [ -z "$1" ]; then
  SERVER=localhost:45123
  CameraCommand.exe $SERVER COMMAND/0/switch\ warnings_`date +%y%m%d%H%M`.out
  $CAMERAROOT/scripts/commentCamera.sh "New logfile opened: warnings_`date +%y%m%d%H%M`.out" $SERVER
else
  CameraCommand.exe $1 COMMAND/0/switch\ warnings_`date +%y%m%d%H%M`.out
  $CAMERAROOT/scripts/commentCamera.sh "New logfile opened: warnings_`date +%y%m%d%H%M`.out" $1
fi
