#!/bin/bash 

###############################################
#                                             #
#    Script to run multiple option files      #
#       Nicola Chiapolini, 03.06.2009         #
#                                             #
###############################################

# print help text if necessary
if [[ $# -eq $startPageList ]]
then
  echo "  Usage: ./runOptionFiles.sh file1.py file2.py ..."
  exit 0
fi

# run the gaudi app for each page file specified on the commandline
for file in $@ 
do
	if [ -f $file ]
	then
		gaudirun.py $file
	else
		echo "$file does not exist in search path!" 
	fi
done

