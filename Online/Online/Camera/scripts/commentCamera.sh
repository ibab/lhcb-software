#!/bin/bash
#
# Sends a comment to the CAMERA servers.
#
# $1 the comment to send. Must be in "" if spaces are used e.g. "My comment" not My comment.
# $2 optional argument with give host:port, where :port is optional. To send commet to more 
#    than one server the servers must be separated by a space and must be enclosed in "" or '',
#     Ex.: ./commentCamera.sh comment "host1:port1 host2:port2" 
#
# The comment will use the user name as its group and it will have
# the info level. The script will use the localhost by default
# but if a second command line argument then that will be taken
# as the host.
#
if [ -z "$2" ]; then
  SERVER=localhost:45123
  CameraCommand.exe $USER/0/"$1" $SERVER 
else
  CameraCommand.exe $USER/0/"$1" $2
fi

