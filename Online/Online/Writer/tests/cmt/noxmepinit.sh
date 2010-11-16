# 
# hltrx.sh
# Markus Frank & Niko Neufeld
# Simple script to run the event builder chain stand-alone
# Requires CMTPROJECTPATH set and setup.sh sourced in $ONLINETASKSROOT/cmt
# 
#
if [ -z $DIM_DNS_NODE ]; then
        echo "Please define DIM_DNS_NODE"
        exit 1
fi
if [ $# -ge 1 ]; then
        if [ $1 = "dbg" ]; then
                DBG=1
        fi
fi

. ./preamble.sh

export LD_LIBRARY_PATH=/sw/lib/lcg/external/gcc/4.3.2/x86_64-slc5/lib64/:$LD_LIBRARY_PATH

export TAN_PORT YES
export TAN_NODE=$(hostname -f)

export BIGTERM='xterm  -ls -132 -geometry 132x45 -title '
export WIDETERM='xterm -hold -vb -j -sl 10000 -ls -132 -geometry 160x40-0-0 -title '
export MINITERM='xterm -iconic -sl 10000 -ls -132 -geometry 132x10 -title '

# shortcuts for starting jobs
export gaudi_run="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineStart "
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=${MSGSVC} -auto"
export gaudi_exe2="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto"


# MBMInit initializes the shared memory
#$MINITERM MEPInit@${HOST} -e "
export UTGID=${HOST}/MEPInit ; ${gaudi_exe2} -main=./noxmepinit.opts  -opt=$GAUDIONLINEROOT/options/Daemon.opts 
#"&
