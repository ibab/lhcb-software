#!/bin/bash

#This is the location that the CAMERA client uses to store the extra
#data received from the CAMERA server.
export CAMCACHE=/tmp/cam/camcache
mkdir -p $CAMCACHE/data

#ssh -L 12346:127.0.0.1:12346 lxplus.cern.ch ssh -L 12346:hlte0403:12346 lbgw

#Takes two arguments:
#$1 - name of the computer running the CAMERA server.
#$2 - the CAMERA warning file to open. This is optional and defaults to warnings.out.
CameraGui.exe $1 $2 &#127.0.0.1 #warnings.out 
