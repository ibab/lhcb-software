#!/bin/csh

# Scripts for testing on offline machine the initialisation of MooreOnline

source ../cmt/setup.csh

# Copy the DIM dns executable and start it
cp $DIM_release_area/DIM/v15r13/slc3_ia32_gcc323/bin/dns .
chmod +x dns
dns &

# Set environment variables
setenv DIM_DNS_NODE $HOST
setenv UTGID `echo $HOST | cut -d. -f1`/$$
echo "Starting Moore with UTGID " $UTGID

../slc3_ia32_gcc323/Moore.exe ../options/MooreOnline.opts
