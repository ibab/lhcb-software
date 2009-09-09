#!/bin/bash

killall -9 CameraWebServer.exe
killall -9 CameraServer.exe

# This is where we save the log files.
export CAMDIR=$HOME/Camera
# Ensures the CAMERA data directory exists.
mkdir -p $CAMDIR/data
# Ensure the CAMERA log directory exists.
mkdir -p $CAMDIR/log

# Start the Camera server and the web server.
CameraWebServer.exe -C $CAMERAROOT/scripts/etc/CameraWebServer.conf > $CAMDIR/log/CameraWebServer.log 2>&1 &
CameraServer.exe -C $CAMERAROOT/scripts/etc/CameraServer.conf > $CAMDIR/log/CameraServer.log 2>&1 &

# Creates a new directory within the data 
source $CAMERAROOT/scripts/newDataDirectory.sh `date +%y%m%d%H%M`
