#!/bin/bash

if test -z "$GaudiOnlineExe";
then
    cmt run /bin/bash $0;
else
    export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
    cd ${MOORESTANDALONEROOT}/cmt;
    echo GaudiOnlineExe=$GaudiOnlineExe;
    export STATIC_OPTS=${FARMCONFIGROOT}/options;
    export JOBOPTSEARCHPATH=${MOORESTANDALONEROOT}/options;
    export INFO_OPTIONS=${MOORESTANDALONEROOT}/options/InfoOpts.opts;
    export LOGFIFO=/tmp/logSrv.fifo;
    export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
    export DIM_DNS_NODE=`hostname -s`;
    export MBM_SETUP_OPTIONS=${MOORESTANDALONEROOT}/options/MBM_setup.opts;
    export DATA_DIRECTORY=${MOORESTANDALONEROOT}/cmt;
    export NUMBER_OF_SLAVES=10;
    `which gentest.exe` libGaudiKernel.so GaudiMain ../options/MooreTest.opts;
fi;

