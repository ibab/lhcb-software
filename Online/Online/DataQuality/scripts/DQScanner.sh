#!/bin/bash
#
cd /group/online/dataflow/cmtuser/DataQuality;
. ./setup.x86_64-slc6-gcc48-dbg.vars
##echo $PYTHONPATH;
export PY_EXE=`which python`;
export PYTHONHOME=`dirname $PY_EXE`;
export PYTHONHOME=`dirname $PYTHONHOME`;
export UTGID;
export DIM_DNS_NODE=mona10;
export PYTHONPATH=${DATAQUALITYROOT}/python:$PYTHONPATH;
#
echo "DIM-DNS: $DIM_DNS_NODE  Working directory: `pwd`"
#
exec -a ${UTGID} `which python` <<EOF

import DataQualityScan.DataQualityScanner as Scan
Scan.run(True)

EOF

