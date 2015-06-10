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
if [ "$act" = "Alignment|Muon" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('MuonAlignment');"
elif [ "$act" = "Alignment|Tracker" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('TrackerAlignment');"
elif [ "$act" = "Alignment|Velo" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('VeloAlignment');"
elif [ "$act" = "Alignment|VeloHalf" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('VeloHalfAlignment');"
elif [ "$act" = "Alignment|TED" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligIterator import doIt; doIt('TEDAlignment');"
elif [ "$act" = "Alignment|Rich" ]; then
  . /group/online/dataflow/cmtuser/Panoptes/OnlPanoptesSys/cmt/setup.x86_64-slc6-gcc48-dbg.vars
  cd ${FARMCONFIGROOT}/job
  export PYTHONPATH=${ONLPANOPTESSYSROOT}/python:$PYTHONPATH
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; importOptions('${ONLPANOPTESSYSROOT}/python/MirrorIterator.py');"
elif [ "$act" = "Calibration|Calo" ]; then
  . /group/online/dataflow/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=/group/online/dataflow/options/LHCbA/HLT:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; importOptions('');"
elif [ "$act" = "BWDivision" ]; then
  . /group/online/bw_division/cmtuser/BWDivisionDev/setup.x86_64-slc6-gcc48-opt.vars
  . /group/online/bw_division/root/bin/thisroot.sh
  export PYTHONPATH=/group/online/bw_division/pydim/lib/python2.7/site-packages:$PYTHONPATH
  export OUTPUTDIR=/group/online/bw_division/output
  python -c "from PyGeneticOnline import Iterator; Iterator.run()"
fi
