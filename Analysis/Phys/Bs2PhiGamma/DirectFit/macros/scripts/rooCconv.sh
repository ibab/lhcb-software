#!/bin/bash

tag=B2Kstg
if [ $2 ]; then
export tag=$2
fi

export fil=` echo $1 | awk -F '\.C\' '{print $1}'`


if [ -f $fil.C ]; then

if [ `grep -c plot $fil.C` == 0 ]; then

export nl=`awk 'END{print NR}' $fil.C`
awk '{if(NR==1){print "TH1F* '$fil' (bool plot=false)"$0 }else if(NR=='$nl'){print "return '$tag';"$0}else if($0=="   '$tag'->SetStats(0);"){print "   '$tag'->SetStats(0);if(!plot)return '$tag';"}else{print $0}}' $fil.C  > ttemp
mv ttemp $fil.C

echo "OK ! the file '$fil.C' has been converted"
else
echo "Why insisting ?? the file '$fil.C' had already been converted !!"
fi
else
echo "Are you sure ?? The fike '$fil.C' does not exist !!"
fi