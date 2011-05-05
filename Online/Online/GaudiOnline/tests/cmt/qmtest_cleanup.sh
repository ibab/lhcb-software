#!/bin/bash
echo "....starting cleanup.....";
#
/bin/ls -1 *.dat | sort;
#
/bin/rm -f *.dat;
echo "....cleanup finished.....";
