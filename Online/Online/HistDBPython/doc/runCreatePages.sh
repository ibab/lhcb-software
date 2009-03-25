#!/bin/bash 

###############################################
#                                             #
# Script to generate pages of different types #
#       Nicola Chiapolini, 03.10.2008         #
#                                             #
###############################################


#basePath=`pwd`

# needed so that python finds the include files
export JOBOPTSEARCHPATH=`pwd`

# print help text if necessary
if [[ $# -eq $startPageList ]]
then
	echo "  Usage: ./runCreateHistos.sh page1 page2 ..."
	echo "  for each PAGE there needs to exist a file:"
        echo "  ${JOBOPTSEARCHPATH}PAGE.py"
	exit 0
fi


# preparing cmt (just to be save)
#cd ${basePath}/cmt
#source setup.sh ""

# run the gaudi app for each page file specified on the commandline
for file in $@ 
do
	if [ -f $JOBOPTSEARCHPATH$file.py ]
	then
		gaudirun.py $file.py
	else
		echo "$file.py does not exist in search path!" 
	fi
done

