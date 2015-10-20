#!/bin/bash
#(
#SetupProject_tmpfile=`runpy LbConfiguration.SetupProject --shell=sh --mktemp Moore $1`
#SetupProjectStatus="$?"
#if [ "$SetupProjectStatus" = 0 -a -n "$SetupProject_tmpfile" ]; then
#    . $SetupProject_tmpfile
#fi
#rm -f $SetupProject_tmpfile
#shift
python ${INDEPENDENCETESTSROOT}/scripts/Moore-Lines.py "$@"
#)
