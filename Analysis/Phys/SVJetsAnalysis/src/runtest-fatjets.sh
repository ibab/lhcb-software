#!/bin/bash

echo ----------------------------------------------------------------------------------------------------
echo 						RUNTEST
echo ----------------------------------------------------------------------------------------------------

if [ -f out ]
then
    rm out
fi

echo cmt make
echo ----------------------------------------------------------------------------------------------------
cmt make >& out 

if [ $? = 0 ]; then
	echo ----------------------------------------------------------------------------------------------------
	echo 						cmt make Done
	echo ----------------------------------------------------------------------------------------------------
	cd $DAVINCIROOT/job/FatJets
	echo changing the folder to:
	pwd
	echo Removing 'out'
	if [ -f out ]
		then
		rm out	
	fi
	echo ----------------------------------------------------------------------------------------------------
	echo executing gaudirun.py PFDaVinci.py
	echo ----------------------------------------------------------------------------------------------------
	gaudirun.py PFDaVinci.py >& out &	
	tail -f out | egrep 'EventSelector' &
	wait %1
	kill %2
	if [ $? = 0 ]; then
		echo ----------------------------------------------------------------------------------------------------
		echo everything is just fine
		echo ----------------------------------------------------------------------------------------------------
	else
		echo ----------------------------------------------------------------------------------------------------
		echo something went wrong execunting gaudirun.py
		echo ----------------------------------------------------------------------------------------------------
		cat $DAVINCIROOT/job/FatJets/out | egrep "\.\.|Warning|Error|Fatal"
	fi

else
	echo problem in cmt make
	echo use 'vim out' and try to debug 
	echo printing errors:
	cat out | grep "\.\."
fi

exit 0
