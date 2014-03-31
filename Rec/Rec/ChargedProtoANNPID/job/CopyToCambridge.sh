#!/bin/bash

export ROOTD="/r03/lhcb/jonesc/ANNPID/ProtoParticlePIDtuples/Sim08a/Digi13/Reco14a/MC/Binc"

export FILELIST="cambridge-list.txt"
rm -rf $FILELIST
touch $FILELIST

mkdir -p $ROOTD

export i=0
for tuple in `find ~/gangadir/workspace/jonesc/LocalXML -name ProtoPIDANN.MC.tuples.root | perl -MList::Util=shuffle -e"print shuffle<>"`; do
 export TARGET=${ROOTD}"/ProtoPIDANN."${i}".root"
 mv -v $tuple $TARGET
 echo $TARGET >> $FILELIST
 i=`expr $i + 1`
done

echo "Copied "`cat $FILELIST | wc -l`" files. Listed in '"$FILELIST"'"

exit 0
