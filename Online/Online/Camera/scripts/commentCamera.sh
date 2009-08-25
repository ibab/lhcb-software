#!/bin/bash
echo $*
export SERVER=localhost:12345
CameraCommand.exe $SERVER $USER/0/"$*"
