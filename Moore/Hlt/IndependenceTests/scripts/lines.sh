#!/bin/bash
(
SetupProject_tmpfile=`runpy LbConfiguration.SetupProject --shell=sh --mktemp Moore`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 -a -n "$SetupProject_tmpfile" ]; then
    . $SetupProject_tmpfile
fi
rm -f $SetupProject_tmpfile
python ./Moore-Lines.py "$@"
)
