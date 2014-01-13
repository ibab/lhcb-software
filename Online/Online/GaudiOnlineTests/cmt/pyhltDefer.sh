#!/bin/bash
echo "All args: $*"
. ${GAUDIONLINEROOT}/tests/cmt/preamble.sh
#
mkdir -p data;
for i in $(seq -w 0 100);
  do 
  ln -s `pwd`/mepData_0.dat data/RUN_$i_$i.dat;
done;
#
start_py_task MBMBuffers "import GaudiOnlineTests;GaudiOnlineTests.runHltBuffer()"
#
sleep 4
#
#  Monitors:
#
$BIGTERM MBMMon@${HOST}     -e "export UTGID=${NODENAME}/MBMMon;    exec -a \${UTGID} $gaudi_run libOnlineKernel.so mbm_mon"&
#
start_py_task RDR_0     "import GaudiOnlineTests;GaudiOnlineTests.runMepFeeder(buffer='Events')"
#
# For debugging enable this and disable any other
#$MINITERM Sender@${HOST}    -e "export UTGID=${NODENAME}/DbgTask   ; cat gaudi.gdb; gdb -x gaudi.gdb $GAUDIONLINEROOT/$CMTCONFIG/Gaudi.exe" &
# $BIGTERM MBMDump@${HOST} -e "export UTGID=${NODENAME}/MBMDump; $gaudi_run libMBMDump.so mbmdump" &
# export UTGID=${NODENAME}/prod_0; $gaudi_run libGaudiOnline.so mep_producer -n=prod_0 -p=333 -s=500 -r=2 -i=Mep -f=mepData_0.dat
tail -n 3 ${0}
