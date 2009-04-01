#!/bin/bash
echo "....starting cleanup....."
/bin/ls -s -1 *.dat
/bin/ls -s -1 *.xml
#
/bin/rm -f *.dat
echo "....cleanup finished....."
