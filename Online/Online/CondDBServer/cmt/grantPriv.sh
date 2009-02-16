#!/bin/sh

MYSITEROOT=/sw/lib
CMTCONFIG=slc4_ia32_gcc34
if [ -r $MYSITEROOT/scripts/ExtCMT.sh ]; then
    export MYSITEROOT CMTCONFIG
    . $MYSITEROOT/scripts/ExtCMT.sh >& /dev/null
else
    echo CMT environment not found
fi

# This sets ORACLE_HOME and ORACLE_SID (needed?)
. /sw/oracle/linux/bin/oraenv

SetupProject Online v4r14 --use Online/CondDBServer >& /dev/null

date >> grantPriv.log
grantPriv.py >> grantPriv.log

#date > test.log
#printenv | sort >> test.log
