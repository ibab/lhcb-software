#!/bin/csh

# Scripts for testing on offline machine the initialisation of MooreOnline

source ../cmt/setup.csh
setenv DIM_DNS_NODE $HOST
setenv LD_LIBRARY_PATH $DIM_release_area/DIM/v16r5/slc4_ia32_gcc345/bin/:${LD_LIBRARY_PATH}
$DIM_release_area/DIM/v16r5/slc4_ia32_gcc345/bin/dns &
$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline -runable=LHCb::OnlineRunable -options=$MOOREOPTS/MoorePrescale.opts -msgsvc=MessageSvc -loop -auto
#$GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe $GAUDIONLINEROOT/$CMTCONFIG/libGaudiOnline.so GaudiOnline -runable=LHCb::OnlineRunable -options=$MOOREOPTS/MooreWhole.opts -msgsvc=MessageSvc -loop -auto

# Copy the DIM dns executable and start it
# cp $DIM_release_area/DIM/v15r13/slc3_ia32_gcc323/bin/dns .
# chmod +x dns
# dns &

# Set environment variables
# setenv DIM_DNS_NODE $HOST
# setenv UTGID `echo $HOST | cut -d. -f1`/$$
# echo "Starting Moore with UTGID " $UTGID

# ..$GAUDIONLINEROOT/slc4a32_gcc323/Moore.exe ../options/MooreOnline.opts
#../slc3_ia32_gcc323/Moore.exe ../options/MooreOnline.opts
