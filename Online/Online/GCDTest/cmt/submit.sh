#
#
#
export PGM=/home/frankm/cmtuser/Gaudi_v22r2/Online/GCDTest/cmt/run.sh
#
#
for i in $(seq -w 1 10); 
  do tmStart -e -o -n frankm -u GCDTest_$i -N hltf01 -m hltf01${i} ${PGM} ${i} 0;
done;
#
#
for i in $(seq -w 1 10); 
  do tmStart -e -o -n frankm -u GCDTest_$i -N hltf02 -m hltf02${i} ${PGM} ${i} 10000;
done;
#
#
for i in $(seq -w 1 10); 
  do tmStart -e -o -n frankm -u GCDTest_$i -N hltf03 -m hltf03${i} ${PGM} ${i} 25000;
done;
#
#
for i in $(seq -w 1 10); 
  do tmStart -e -o -n frankm -u GCDTest_$i -N hltf04 -m hltf04${i} ${PGM} ${i} 50000;
done
#
#
