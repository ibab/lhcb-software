#!/bin/bash

# This is an example if that can be used to prevent against accidental starting
# of CAMERA servers from the package.

#if [ "$HOST" = "localhost" ]; then
killall -9 CameraWebServ.exe
killall -9 CameraServ.exe


# The executables will look for their config files in the directory etc from where
# they were executed. Make sure we are in that directory.
cd $CAMERAROOT/scripts/etc

# This is where we save the log files.
export CAMDIR=$HOME/Camera
# Ensures the CAMERA data directory exists
mkdir -p $CAMDIR/data

# Start the Camera server and the web server.
CameraWebServ.exe > $CAMDIR/CameraWebServ.log 2>&1 &
CameraServ.exe > $CAMDIR/CameraServ.log 2>&1 &

# Creates a new warnings.out file when the camera server is started.
$CAMERAROOT/scripts/newCameraLog.sh

#fi
#else
#    echo "camserv must be run on localhost. camserv not started."
#fi
