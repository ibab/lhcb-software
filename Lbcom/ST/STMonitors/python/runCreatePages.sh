#!/bin/bash 

###############################################
#                                             #
# Script to generate pages of different types #
#       Nicola Chiapolini, 02.12.2008         #
#                                             #
###############################################

# needed so that python finds the include files
export JOBOPTSEARCHPATH=`pwd`

# print help text if necessary
if [[ $# -eq $startPageList ]]
then
	echo "  Usage: ./runCreateHistos.sh page1.py page2.py ..."
	echo "  for each page there needs to exist a file:"
        echo "  ${JOBOPTSEARCHPATH}page.py"
	exit 0
fi

# run the gaudi app for each page file specified on the commandline
for file in $@ 
do
	if [ -f $JOBOPTSEARCHPATH/$file ]
	then
		gaudirun.py $file
	else
		echo "$file does not exist in search path!" 
	fi
done
