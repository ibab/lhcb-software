#!/bin/bash
# =========================================================================
#
#  Default script to start the data writer task on a farm node
#
#  Author   M.Frank
#  Version: 1.0
#  Date:    20/05/2013
#
# =========================================================================
#
act=${RUN_TYPE}
if [ "$act" = "Alignment|Tracker" ]; then
#  . /group/online/dataflow/cmtuser/Alignment/AlignmentOnline/AlignOnline/cmt/setup.x86_64-slc6-gcc48-dbg.vars
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r2/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
#  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; importOptions('${ALIGNONLINEROOT}/python/TrAligIterator.py');"
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('TrackerAlignment');"
elif [ "$act" = "Alignment|Velo" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r2/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('VeloHalfAlignment');"
elif [ "$act" = "Alignment|TED" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r2/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('TEDAlignment');"
elif [ "$act" = "RichMirror" ]; then
  . /group/online/dataflow/cmtuser/Panoptes/OnlPanoptesSys/cmt/setup.x86_64-slc6-gcc48-dbg.vars
  cd ${FARMCONFIGROOT}/job
  export PYTHONPATH=${ONLPANOPTESSYSROOT}/python:$PYTHONPATH
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; importOptions('${ONLPANOPTESSYSROOT}/python/MirrorIterator.py');"
fi
