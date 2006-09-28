#!/bin/bash
# cleanup everything brutally
#
/usr/bin/killall Gaudi.exe
/usr/bin/killall test.exe
sleep 1
/usr/bin/killall -9 Gaudi.exe
/bin/rm -fr /dev/shm/*

