#!/bin/bash
echo $*
export SERVER=localhost:12346
CameraCommand.exe $SERVER $USER/0/"$*"
