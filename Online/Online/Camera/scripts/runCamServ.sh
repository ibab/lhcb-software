#!/bin/bash

killall -9 CameraWebServ.exe
killall -9 CameraServ.exe

# This is where we save the log files.
export CAMDIR=$HOME/Camera
# Ensures the CAMERA data directory exists
mkdir -p $CAMDIR/data

# Start the Camera server and the web server.
CameraWebServer.exe -C $CAMERAROOT/scripts/etc/CameraWebServer.conf > $CAMDIR/log/CameraWebServer.log 2>&1 &
CameraServer.exe -C $CAMERAROOT/scripts/etc/CameraServer.conf > $CAMDIR/log/CameraServer.log 2>&1 &

# Creates a new warnings.out file when the camera server is started.
$CAMERAROOT/scripts/newCameraLog.sh
