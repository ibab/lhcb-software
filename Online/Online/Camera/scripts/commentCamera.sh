#!/bin/bash
echo $*
export SERVER=localhost
CameraMessage.exe $SERVER $USER/0/"$*"
