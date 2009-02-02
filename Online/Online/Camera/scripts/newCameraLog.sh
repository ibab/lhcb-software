#!/bin/bash
echo $*
export SERVER=localhost
CameraCommand.exe $SERVER COMMAND/0/switch\ warnings_`date +%y%m%d%H%M`.out
$CAMERAROOT/scripts/CommentCamera.sh New logfile opened: warnings_`date +%y%m%d%H%M`.out
