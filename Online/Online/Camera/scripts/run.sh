#!/bin/bash

#Takes two arguments:
#$1 - name(s) of the computer running the CAMERA server. In the form
#       host:port[,host:port,host:port]. If no name is given then
#       localhost:45124 is used.
#$2 - the CAMERA warning file to open. This is optional and if no server
#       is specified then the warning file must not be specified either.
# The arugments are forwarded to the runCameraGui.sh as runCameraServer.sh
# does not take any arguments.
./runCameraServers.sh
./runCameraGui.sh $1 $2
