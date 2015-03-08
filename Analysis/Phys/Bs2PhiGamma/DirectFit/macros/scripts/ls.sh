#!/bin/bash

for fil in `ls *root`; do

    root -n -l -b -q $fil -c ~/Analysis/macros/ls.C

done
