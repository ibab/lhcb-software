#!/bin/sh

mkdir $TMPDIR/$1
pushd $TMPDIR/$1

find ~/gangadir/workspace/akozlins/LocalXML/$1 -name "lfns.lst" -exec cat {} \; \
| grep "clbr_hists.root" \
| gawk '{print "echo " $1 "; dirac-dms-get-file " $1 "; mv -v clbr_hists.root clbr_hists_" NR ".root"}' \
| sh; hadd clbr_hists.root `ls -1 clbr_hists_*.root | tr "\n" " "`

popd
cp $TMPDIR/$1/clbr_hists.root .
