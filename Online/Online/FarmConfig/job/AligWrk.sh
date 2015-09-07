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
fix_python()
{
  export UTGID;
  PY=`which python`;
  PY=`dirname ${PY}`;
  export PYTHONHOME=`dirname ${PY}`;
  export PYTHONPATH=/group/online/bw_division/pydim/lib/python2.7/site-packages:$PYTHONPATH;
  unset PY;
}
#
#
act=${RUN_TYPE}
if [ "$act" = "Alignment|Muon" ]; then
  . /group/online/dataflow/cmtuser/AlignmentRelease/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligAnalyzer import doIt; doIt('/localdisk/Alignment/Muon', 'MuonAlignment');"
elif [ "$act" = "Alignment|Tracker" ]; then
  . /group/online/dataflow/cmtuser/AlignmentRelease/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligAnalyzer import doIt; doIt('/localdisk/Alignment/Tracker', 'TrackerAlignment');"
elif [ "$act" = "Alignment|Velo" ]; then
  . /group/online/dataflow/cmtuser/AlignmentRelease/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligAnalyzer import doIt; doIt('/localdisk/Alignment/Velo', 'VeloAlignment');"
elif [ "$act" = "Alignment|VeloHalf" ]; then
  . /group/online/dataflow/cmtuser/AlignmentRelease/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligAnalyzer import doIt; doIt('/localdisk/Alignment/Velo', 'VeloHalfAlignment');"
elif [ "$act" = "Alignment|TED" ]; then
  . /group/online/dataflow/cmtuser/AlignmentRelease/setup.x86_64-slc6-gcc48-dbg.vars
  export PYTHONPATH=${ALIGNONLINEROOT}/python:$PYTHONPATH
  cd ${FARMCONFIGROOT}/job
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from TrAligAnalyzer import doIt; doIt('/localdisk/Alignment/Tracker', 'TEDAlignment');"
elif [ "$act" = "Alignment|Rich1" ]; then
  . /group/rich/sw/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
#  export PYTHONPATH=/group/online/bw_division/pydim/lib/python2.7/site-packages:$PYTHONPATH
  export PYTHONPATH=/group/online/dataflow/options/LHCbA/HLT:$PYTHONPATH
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from PyMirrAlignOnline import RichAnalyzer; RichAnalyzer.doIt(filename = '/localdisk/Alignment/Rich', whichRich = 1);"
elif [ "$act" = "Alignment|Rich2" ]; then
  . /group/rich/sw/cmtuser/AlignmentOnlineDev_v10r4/setup.x86_64-slc6-gcc48-dbg.vars
#  export PYTHONPATH=/group/online/bw_division/pydim/lib/python2.7/site-packages:$PYTHONPATH
  export PYTHONPATH=/group/online/dataflow/options/LHCbA/HLT:$PYTHONPATH
  exec -a ${UTGID} ${Class1_task} libGaudiOnline.so OnlineTask -tasktype=LHCb::Class1Task -main=/group/online/dataflow/templates/options/Main.opts -opt=command="import Gaudi,GaudiKernel.ProcessJobOptions; from Gaudi.Configuration import importOptions; GaudiKernel.ProcessJobOptions.printing_level=999; from PyMirrAlignOnline import RichAnalyzer; RichAnalyzer.doIt(filename = '/localdisk/Alignment/Rich', whichRich = 2);"
elif [ "$act" = "Calibration|Calo" ]; then
  . /group/calo/cmtuser/CaloCalibrationDev_v10r4p1/setup.x86_64-slc6-gcc48-dbg.vars;
  fix_python;
  export PYTHONPATH=/group/online/dataflow/options/LHCbA/HLT:$PYTHONPATH; 
  cd ${FARMCONFIGROOT}/job;
  exec -a ${UTGID} python -c "from PyKaliOnline import Analyzer; Analyzer.run(0, '/group/online/CalibWork')"
elif [ "$act" = "BWDivision" ]; then
  . /group/online/bw_division/cmtuser/BWDivisionDev/setup.x86_64-slc6-gcc48-opt.vars
  fix_python;  
  export PYTHONPATH=/group/online/bw_division/pydim/lib/python2.7/site-packages:/scratch/jenkins/benchmark/python:$PYTHONPATH
  export OUTPUTDIR=/group/online/bw_division/output
  export UTGID
  exec -a ${UTGID} python -c "from PyGeneticOnline import Analyzer; Analyzer.run()"
fi
