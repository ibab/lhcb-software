#!/bin/tcsh
 
source ../cmt/setup.csh

set roots = `env | grep ROOT | grep DAVINCI/DAVINCI | grep -v PhysSel | grep -v DAVINCIENV | sort | uniq | awk -F= '{print $1}'`

# now get the rest from brunel
set brunel = `ls -1d /afs/cern.ch/lhcb/software/releases/BRUNEL/* | grep BRUNEL_v | \tail -1`
set bruneldirs = `grep ROOT $brunel/Rec/Brunel/v*/doc/DoxyFile.cfg | grep -v INPUT | awk -F\( '{print $2}' | awk -F\) '{print $1}'`
foreach D ( $bruneldirs )
   set F = `env | grep $D | awk -F\= '{print $1}'`
   set roots = `echo $roots $F`
#  echo added $F
end
touch tmpfile
foreach D ( $roots )
  echo $D >> tmpfile
end
set sorted = `sort tmpfile | uniq`
rm tmpfile
foreach D ( $sorted )
  echo "         \044("$D") \"  
end
