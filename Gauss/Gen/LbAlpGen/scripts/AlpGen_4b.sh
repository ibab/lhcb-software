#!/bin/bash
X=0
S1=0
S2=0
S3=0
S4=0
echo "Log file for the 4b runs" > 4boutfile
while [ $X -le 3 ]
do
	S1=$((X+10000))
	S2=$((X+11000))
	S3=$((X+12000))
	S4=$((X+13000))
	echo "1" > 4binitfile
	echo "4brun"$X >> 4binitfile
	echo "0"       >> 4binitfile
	echo "10000"    >> 4binitfile
	echo "4"        >> 4binitfile
	echo "100000"   >> 4binitfile
	echo "ih2 1"   >> 4binitfile
	echo "ihvy 5"  >> 4binitfile
	echo "ihvy2 5"      >> 4binitfile
	echo "ebeam 7000.0" >> 4binitfile
	echo "mb 4.8" >> 4binitfile
	echo "ptbmin" 5.0 >> 4binitfile
	echo "ptcmin" 5.0 >> 4binitfile
	echo "etabmax" 5.0 >> 4binitfile
	echo "etacmax" 5.0 >> 4binitfile
	echo "drbmin" 0.0 >> 4binitfile
	echo "drcmin" 0.0 >> 4binitfile
	echo "iseed1" $S1 >> 4binitfile
	echo "iseed2" $S2 >> 4binitfile
	echo "iseed3" $S3 >> 4binitfile
	echo "iseed4" $S4 >> 4binitfile
	echo "print 1" >> 4binitfile
	AlpGen4Q.exe < 4binitfile >> 4boutfile
	echo "2" > 4binitfile2
	echo "4brun"$X >> 4binitfile2
	echo "print 1" >> 4binitfile2
	AlpGen4Q.exe < 4binitfile2 >> 4boutfile
	X=$((X+1))
done
