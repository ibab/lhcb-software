#!/bin/bash
PID=$$;
#
#
cat >/tmp/match_${PID}.txt<<EOF
.svn
setup.
cleanup.
.history
x86_64-slc5-gcc
x86_64-slc5-gcc
.pyc
.ph
.pp
.d differ
.stamp.differ
.make.differ
EOF
#
#
diff -r --brief ONLINE_${1} ONLINE_${2} | grep -v -f /tmp/match_${PID}.txt 
