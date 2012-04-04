#!/bin/bash
##echo "CMTCONFIG=$CMTCONFIG "
##. setup.sh
unset LD_PRELOAD
unset LD_LIBRARY_PATH
`which restore.exe` $*;
