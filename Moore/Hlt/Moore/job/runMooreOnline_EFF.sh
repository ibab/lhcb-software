#!/bin/bash

# Scripts for testing on offline machine the initialisation of MooreOnline

#to get online joboptions for buffermanager etc
cd job
. ./setupOnline.sh $*

oldpwd=$PWD
#for cmt
while [ $# -ne 0 ];do
   shift
done   

echo $MOOREROOT
cd /home/bdepaula/cmtuser/Moore_v4r2/Hlt/Moore/v4r2/job
. ./pathsetup
#cd $oldpwd

#setenv DIM_DNS_NODE $HOST
#setenv LD_LIBRARY_PATH $DIM_release_area/DIM/v16r5/slc4_ia32_gcc345/bin/:${LD_LIBRARY_PATH}
#$DIM_release_area/DIM/v16r5/slc4_ia32_gcc345/bin/dns &

#$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline -runable=LHCb::OnlineRunable -options=$MOOREOPTS/MoorePrescale.opts -msgsvc=MessageSvc -loop -auto
#echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${MOOREOPTS}/MoorePrescale.opts
#exec -a ${UTGID} ${CLASS1_TASK} -opt=${MOOREOPTS}/MoorePrescale.opts
echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${MOOREOPTS}/DummyRead.opts
exec -a ${UTGID} ${CLASS1_TASK} -opt=${MOOREOPTS}/DummyRead.opts

#${CLASS1_TASK} -opt=${MOOREOPTS}/MoorePrescale.opts

# Copy the DIM dns executable and start it
# cp $DIM_release_area/DIM/v15r13/slc3_ia32_gcc323/bin/dns .
# chmod +x dns
# dns &

# Set environment variables
# setenv DIM_DNS_NODE $HOST
# setenv UTGID `echo $HOST | cut -d. -f1`/$$
# echo "Starting Moore with UTGID " $UTGID

#..$GAUDIONLINEROOT/slc4a32_gcc323/Moore.exe ../options/MooreOnline.opts
#../slc3_ia32_gcc323/Moore.exe ../options/MooreOnline.opts
