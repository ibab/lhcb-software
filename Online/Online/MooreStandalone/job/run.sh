#!/bin/bash

if test -z "${GaudiOnlineExe}";
then
    cd `dirname $0`/../cmt;
    echo "Executing executable in proper environment using cmt CONFIG=${CMTCONFIG}....";
    cmt run "/bin/bash $0 $*";
else
    export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
    cd ${MOORESTANDALONEROOT}/cmt;
    echo GaudiOnlineExe=$GaudiOnlineExe;
    export STATIC_OPTS=${FARMCONFIGROOT}/options;
    export JOBOPTSEARCHPATH=${MOORESTANDALONEROOT}/options;
    export INFO_OPTIONS=${MOORESTANDALONEROOT}/options/InfoOpts.opts;
    export MBM_SETUP_OPTIONS=${MOORESTANDALONEROOT}/options/MBM_setup.opts;
    export MOORESTANDALONE_UI_OPTS=${MOORESTANDALONEROOT}/options/MooreTestConsole.opts;
    export MOORESTANDALONE_OPTIONS=${MOORESTANDALONEROOT}/options/MooreStandalone.opts;
    export DATA_DIRECTORY=${MOORESTANDALONEROOT}/cmt/BrunelData;
    export DATA_DIRECTORY=${MOORESTANDALONEROOT}/cmt/MEPData;
    export GaudiOnlineExe=`which GaudiOnlineExe.exe`;
    export DIM_DNS_NODE=`hostname -s`;
    export LOGFIFO=/tmp/log${USER}.fifo;
    GUI=;
    HLP=;
    OPTS=../options/MooreUPI.opts;
    OPTS=../options/MooreTest.opts;
    while [[ $# > 0 ]]
    do
	key="$1"
	shift;
	
	case $key in
	    -u|--upi)
		GUI=upi
		export MOORESTANDALONE_UI_OPTS=${MOORESTANDALONEROOT}/options/MooreTestUPI.opts;
		shift
		;;
	    -g|--gui)
		export MOORESTANDALONE_UI_OPTS=${MOORESTANDALONEROOT}/options/MooreTestUPI.opts;
		GUI=upi
		shift
		;;
	    -o|--opts)
		OPTS="$1"
		echo "++++ Options=$OPTS"
		shift
		;;
	    -h|--help)
		HLP=yes;
		shift
		;;
	    --default)
		HLP=yes
		shift
		;;
	    *)
            # unknown option
		;;
	esac
    done  

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
	echo "Running Moore Benchmark application in UPI mode.";
	`which gentest.exe` libMooreStandalone.so moore_standalone -options=${OPTS};
    else
	echo "Running Moore Benchmark application in script mode.";
	`which gentest.exe` libGaudiKernel.so GaudiMain ${OPTS};
    fi;
fi;

