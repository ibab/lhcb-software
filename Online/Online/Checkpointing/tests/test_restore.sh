#!/bin/bash
unset LD_PRELOAD;
unset LD_LIBRARY_PATH;
`which restore.exe` $* 2>&1;
