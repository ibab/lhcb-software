#!/bin/bash

# This setup script is to be used with the LHCb online environment to
# start the CAMERA web server when the computer boots.
# It is a simple script setting up the enviroment to use then
# starting the executable with the exec command. This allows the LHCb
# online scripts to maintain control over the server.

# Define a couple of variables that will point to the CAMERA executable
# and config files.
version=v4r23
Camera_release=/sw/lib/lhcb/ONLINE/ONLINE_$version/Online/Camera/

# The official LHCb recommended mechanism for setting up a project.
# Setting up the online project gives us access to the CAMERA executable.
source `which SetupProject.sh` Online $version

# Start the executable.
exec -a CameraWebServer `which CameraWebServer.exe` -C Camera_release/scripts/etc/CameraWebServer.conf
