#!/bin/bash
# argument 1=Tag
# argument 2=output db filename

if test -n "$1" ; then export TAG=$1
   else export TAG=head-20090112
fi

. $LHCBSCRIPTS/SetupProject.sh LHCb

for partition in LHCBCOND DDDB; do
   if test -n "$2"; then export DB=$2
      else export DB=/group/online/hlt/conditions/${partition}_$TAG.db
   fi   
   CondDBAdmin_MakeSnapshot.py -T $TAG $partition sqlite_file:$DB/$partition
done      
 




