#!/bin/bash
# Creates a new data directory for the CAMERA server.
#
# $1 is the name of the directory to save the CAMERA messages to.
# $2 optional argument with give host:port, where :port is optional.
#
# The new directory will be created in the root directory given
# in the CAMERA server configuration file. The script will use
# the localhost by default but if a second command line argument
# then that will be taken as the host.
#
if [ -z "$2" ]; then
  SERVER=localhost:12345
  CameraSwitch.exe $1 $SERVER
else
  CameraSwitch.exe $1 $2
fi
