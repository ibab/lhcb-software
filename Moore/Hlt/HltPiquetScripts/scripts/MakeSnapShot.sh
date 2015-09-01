#!/bin/bash
# argument 1=Tag
# argument 2=output db filename

if test -n "$1" ; then export TAG=$1
   else echo "Script cannot be called without explicit tag. Call MakeSnapshot.py.";exit 1
fi

#. $LBSCRIPTS_HOME/InstallArea/scripts/SetupProject.sh LHCb

DATE=`date +%Y"-"%m"-"%d`

#export SQLITEDBPATH="/cvmfs/lhcb.cern.ch/lib/lhcb/SQLite/SQLDDDB/db"

for partition in LHCBCOND DDDB SIMCOND; do
   if test -n "$2"; then export DB=$2
      else export DB=${partition}_$TAG.db
   fi   
   echo " request snapshot of partition " $partition " using tag " $TAG " for date "   $DATE
   CondDBAdmin_MakeSnapshot.py -T $TAG $partition sqlite_file:$DB/$partition
done      
 




