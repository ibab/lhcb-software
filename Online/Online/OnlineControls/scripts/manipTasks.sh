#!/bin/bash
EXECS=/opt/FMC/sbin:/opt/FMC/bin:${PATH};
export LD_LIBRARY_PATH=/opt/FMC/lib;
cd /home/frankm/cmtuser/Gaudi_v19r2/Online/OnlineControls/cmt
cmd=`/usr/bin/python -c "import dataflowBoot; dataflowBoot.${1}()"`
eval $cmd
