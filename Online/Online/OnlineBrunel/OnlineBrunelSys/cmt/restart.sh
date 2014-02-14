#!/bin/bash
unset LD_PRELOAD
unset LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
ulimit -d 2097152
ulimit -m 2097152
ulimit -v 2097152
#pkill restore.exe
`which restore.exe` $*
