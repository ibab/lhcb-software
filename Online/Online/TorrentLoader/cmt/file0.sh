for row in a b c d e f; do
    for rack in 01 02 03 04 06 07 08 09 10 11; do 
	tmStart -e -o -N hlt${row}${rack} -m hlt${row}${rack}?? `pwd`/get.sh  \
	  /group/online/dataflow/cmtuser/checkpoints/Moore/Moore_v12r0dev/Physics_draft2011/head-20101112/head-20101206/data.torrent -timeout=30 -print=4 &
    done;
done;
