#!/bin/bash
echo "All args: $*"
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
#rm /dev/shm/bm_* /dev/shm/sem.bm_* /dev/shm/TAN* /dev/shm/sem.TAN*
#
export INFOOPTIONS=$GAUDIONLINEROOT/options/OnlineEnvironment.opts
#
#####start_py_task MBMBuffers "import GaudiOnlineTests;GaudiOnlineTests.runHltBuffer()"
$MINITERM MBMInit@${HOST}   -e "export UTGID=${NODENAME}/MEPInit;   exec -a \${UTGID} $Class0_task -opts=${OPTS}/HLT2MBM.opts"&
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
$MINITERM Moore_0@${HOST}   -e "export UTGID=${NODENAME}/Moore_0;   exec -a \${UTGID} $Class1_task -opts=${OPTS}/PassThrough.opts"&
#
$MINITERM OverflowRdr_0_0@${HOST}   -e "export UTGID=${NODENAME}/OverflowRDR_0;   exec -a \${UTGID} $Class2_task -opts=${OPTS}/OverflowReader.opts"&
#
#
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep2mbm_producer -n=prod_0 -i=Events -p=333 -s=500 -r=2
tail -n 3 ${0}
