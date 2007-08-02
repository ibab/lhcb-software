#!/bin/bash
X=0
S1=0
S2=0
S3=0
S4=0
echo "Log file for the 4c runs" > 4coutfile
while [ $X -le 9 ]
do
	S1=$((X+10000))
	S2=$((X+11000))
	S3=$((X+12000))
	S4=$((X+13000))
	echo "1" > 4cinitfile
	echo "4crun"$X >> 4cinitfile
	echo "0" >> 4cinitfile
	echo "4000" >> 4cinitfile
	echo "4" >> 4cinitfile
	echo "40000" >> 4cinitfile
	echo "ih2 1" >> 4cinitfile
	echo "ihvy 4" >> 4cinitfile
	echo "ihvy2 4" >> 4cinitfile
	echo "ebeam 7000.0" >> 4cinitfile
	echo "mb 4.8" >> 4cinitfile
	echo "ptbmin" 5.0 >> 4cinitfile
	echo "ptcmin" 5.0 >> 4cinitfile
	echo "etabmax" 5.0 >> 4cinitfile
	echo "etacmax" 5.0 >> 4cinitfile
	echo "drbmin" 0.0 >> 4cinitfile
	echo "drcmin" 0.0 >> 4cinitfile
	echo "iseed1" $S1 >> 4cinitfile
	echo "iseed2" $S2 >> 4cinitfile
	echo "iseed3" $S3 >> 4cinitfile
	echo "iseed4" $S4 >> 4cinitfile
	echo "print 1" >> 4cinitfile
	./4Qgen < 4cinitfile >> 4coutfile
	echo "2" > 4cinitfile
	echo "4crun"$X >> 4cinitfile
	echo "print 1" >> 4cinitfile
	./4Qgen < 4cinitfile >> 4coutfile
	X=$((X+1))
done
