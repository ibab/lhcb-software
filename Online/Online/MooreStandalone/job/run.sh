#!/bin/bash

if test -z "${GaudiOnlineExe}";
then
    cd `dirname $0`/../cmt;
    echo "Executing executable in proper environment using cmt....";
    cmt run "/bin/bash $0 $*";
else
    export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
    cd ${MOORESTANDALONEROOT}/cmt;
    echo GaudiOnlineExe=$GaudiOnlineExe;
    export STATIC_OPTS=${FARMCONFIGROOT}/options;
    export JOBOPTSEARCHPATH=${MOORESTANDALONEROOT}/options;
    export INFO_OPTIONS=${MOORESTANDALONEROOT}/options/InfoOpts.opts;
    export MBM_SETUP_OPTIONS=${MOORESTANDALONEROOT}/options/MBM_setup.opts;
    export DATA_DIRECTORY=${MOORESTANDALONEROOT}/cmt;
    export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
    export DIM_DNS_NODE=`hostname -s`;
    export LOGFIFO=/tmp/logSrv.fifo;
    GUI=;
    HLP=;
    OPTS=../options/MooreTest.opts;
    OPTS=../options/MooreUPI.opts;
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
	OPTS=../options/MooreUPI.opts;
	echo "Running Moore Benachmark application in UPI mode.";
	`which gentest.exe` libMooreStandalone.so moore_standalone -options=${OPTS};
    else
	echo "Running Moore Benachmark application in script mode.";
	`which gentest.exe` libGaudiKernel.so GaudiMain ${OPTS};
    fi;
fi;

