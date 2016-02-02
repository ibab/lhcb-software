#!/bin/bash
#==========================================================================
#  LHCb Online software suite
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see OnlineSys/LICENSE.
#
# Author     : M.Frank
#
#==========================================================================
PROCID=$$;
HOST=`hostname -s`;
HOST=`echo $HOST | tr a-z A-Z`;
MINITERM='xterm -ls -132 -geometry 132x32';
kill_all_tasks_of_type()
{
    killall -9 $1;
    pkill -9 $1;
}
#
start_gentest()
{
    LIB=${1};
    shift;
    TITLE=${1};
    shift;
    EXE=`which gentest.exe`;
    if test -z "${EXE}"; then
	echo "ERROR:  Did not find gentest.exe: ${EXE}.....";
    else
	xterm -ls -132 -geometry 132x32 -title "${HOST}::${TITLE} $*" -e "exec -a ${TITLE}_${PROCID} `which gentest.exe` ${LIB} $*" &
	#xterm -ls -132 -geometry 132x32 -title "${HOST}::${TITLE} $*" -e "exec -a ${TITLE}_${PROCID}  numactl --membind=0 --cpunodebind=0 `which gentest.exe` ${LIB} $*" &
    fi;
}
#
start_gentest_debug()
{
    LIB=${1};
    shift;
    TITLE=${1};
    shift;
    EXE=`which gentest.exe`;
    if test -z "${EXE}"; then
	echo "ERROR:  Did not find gentest.exe: ${EXE}.....";
    else
	xterm -ls -132 -geometry 132x32 -title ${HOST}::${TITLE} -e "exec -a ${TITLE}_${PROCID} `which debug` --args `which gentest.exe` ${LIB} $*" &
    fi;
}
#
start_gentest_online()
{
    TITLE=${1};
    shift;
    start_gentest libOnlineKernel.so ${TITLE} $*;
}
#
start_gentest_online_debug()
{
    TITLE=${1};
    shift;
    start_gentest_debug libOnlineKernel.so ${TITLE} $*;
}
