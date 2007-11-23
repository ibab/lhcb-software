#!/bin/bash
test -n "$1" ; export TOPLEVEL=$1
test -n "$2" ; export PARTNAME=$2

export INFOOPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
export OPTIONS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_${TOPLEVEL}_HLT.opts
shift

. ./setupOnline_dbg.sh

#${CLASS2_TASK}  -opts=${HLTOPTS}/DiskWR.opts
${gaudi_exe} -tasktype=LHCb::Class2Task -main=${HLTOPTS}/Main.opts -opts=${HLTOPTS}/DiskWR.opts
