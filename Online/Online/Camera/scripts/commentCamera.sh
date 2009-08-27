#!/bin/bash
#
# Sends a comment to the CAMERA server.
#
# $1 the comment to send. Must be in "" if spaces are used e.g. "My comment" not My comment.
# $2 optional argument with give host:port, where :port is optional.
#
# The comment will use the user name as its group and it will have
# the info level. The script will use the localhost by default
# but if a second command line argument then that will be taken
# as the host.
#
if [ -z "$2" ]; then
  SERVER=localhost:12345
  CameraCommand.exe $USER/0/"$1" $SERVER 
else
  CameraCommand.exe $USER/0/"$1" $2
fi

