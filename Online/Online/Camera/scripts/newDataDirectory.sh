#!/bin/bash

# Creates a new data directory for the CAMERA server.
# The first argument is the name of the directory to create.
# Second argument is the server and port in the form server:port.
# The new directory will be created in the root directory given
# in the CAMERA server configuration file.
CameraSwitch.exe $1 $2
