#!/bin/bash
X=0
S1=0
S2=0
S3=0
S4=0
echo "Log file for the 2b2c runs" > 2b2coutfile
while [ $X -le 9 ]
do
	S1=$((X+10000))
	S2=$((X+11000))
	S3=$((X+12000))
	S4=$((X+13000))
	echo "1" > 2b2cinitfile
	echo "2b2crun"$X >> 2b2cinitfile
	echo "0" >> 2b2cinitfile
	echo "4000" >> 2b2cinitfile
	echo "4" >> 2b2cinitfile
	echo "40000" >> 2b2cinitfile
	echo "ih2 1" >> 2b2cinitfile
	echo "ihvy 5" >> 2b2cinitfile
	echo "ihvy2 4" >> 2b2cinitfile
	echo "ebeam 7000.0" >> 2b2cinitfile
	echo "mb 4.8" >> 2b2cinitfile
	echo "ptbmin" 5.0 >> 2b2cinitfile
	echo "ptcmin" 5.0 >> 2b2cinitfile
	echo "etabmax" 5.0 >> 2b2cinitfile
	echo "etacmax" 5.0 >> 2b2cinitfile
	echo "drbmin" 0.0 >> 2b2cinitfile
	echo "drcmin" 0.0 >> 2b2cinitfile
	echo "iseed1" $S1 >> 2b2cinitfile
	echo "iseed2" $S2 >> 2b2cinitfile
	echo "iseed3" $S3 >> 2b2cinitfile
	echo "iseed4" $S4 >> 2b2cinitfile
	echo "print 1" >> 2b2cinitfile
	./4Qgen < 2b2cinitfile >> 2b2coutfile
	echo "2" > 2b2cinitfile
	echo "2b2crun"$X >> 2b2cinitfile
	echo "print 1" >> 2b2cinitfile
	./4Qgen < 2b2cinitfile >> 2b2coutfile
	X=$((X+1))
done
