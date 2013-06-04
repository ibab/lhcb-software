#!/bin/bash

export CASTORD=$CASTOR_HOME/ProtoParticlePIDtuples/MC12-Binc-nu2.5

export FILELIST="castor-list.txt"
rm -rf $FILELIST
touch $FILELIST

nsrm -r $CASTORD
nsmkdir $CASTORD

export i=0
for tuple in `find ~/gangadir/workspace/jonrob/LocalXML -name ProtoPIDANN.MC.tuples.root | perl -MList::Util=shuffle -e"print shuffle<>"`; do
 export CASTORF=${CASTORD}"/Reco14-"${i}".root"
 echo "Copying "$tuple" to castor as "$CASTORF
 echo $CASTORF >> $FILELIST
 rfcp $tuple $CASTORF
 i=`expr $i + 1`
done

echo "Copied "`cat $FILELIST | wc -l`" files to castor. Listed in '"$FILELIST"'"

exit 0
