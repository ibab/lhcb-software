#!/bin/bash

#This is the location that the CAMERA client uses to store the extra
#data received from the CAMERA server.
export CAMCACHE=/tmp/cam/camcache
mkdir -p $CAMCACHE/data

# ssh -L 45124:127.0.0.1:45124 lxplus.cern.ch ssh -L 45124:plus:45124 lbgw

#Takes two arguments:
#$1 - name(s) of the computer running the CAMERA server. In the form
#       host:port[,host:port,host:port]. If no name is given then
#       localhost:45124 is used.
#$2 - the CAMERA warning file to open. This is optional and if no server
#       is specified then the warning file must not be specified either.
if [ -z "$1" ]; then
  SERVER=localhost:45124
  CameraGui.exe $SERVER $2 &
else
  CameraGui.exe $1 $2 &
fi


