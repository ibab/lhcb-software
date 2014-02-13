#!/bin/bash

if test -z "$GaudiOnlineExe";
then
    cd `dirname $0`/../cmt;
    cmt run "/bin/bash $0 $*";
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
    GUI=;
    HLP=;
    for i in $*; 
    do
	arg=`echo $i | tr a-z 'A-Z'`;
	if test "$arg" = "-GUI";
	then 
	    GUI=upi;
	elif test "$arg" = "-UPI";
	then
	    GUI=upi;
	elif test "$arg" = "-HELP";
	then
	    HLP=yes;
	elif test "$arg" = "-H";
	then
	    HLP=yes;
	elif test "$arg" = "-?";
	then
	    HLP=yes;
	fi;
    done;

    if test -n "$HLP";
    then
	echo "";
	echo " Usage: $0 -arg [-args]";
	echo "   Default/No args: run in scripting mode.";
	echo "   -upi             Run with terminal gui";
	echo "   -h(elp) , -?     Get this help.";
	echo "";
    elif test "$GUI" = "upi";
    then
	echo "Running Moore Benachmark application in UPI mode.";
	`which gentest.exe` libMooreStandalone.so moore_standalone;
    else
	echo "Running Moore Benachmark application in script mode.";
	`which gentest.exe` libGaudiKernel.so GaudiMain ../options/MooreTest.opts;
    fi;
fi;

