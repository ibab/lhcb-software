#!/bin/bash
echo $*
export SERVER=localhost
CameraComment.exe $SERVER $USER/0/"$*"
