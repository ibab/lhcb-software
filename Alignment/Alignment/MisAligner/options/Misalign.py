##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Brunel-2008.py <someDataFiles>.py
##############################################################################
from Gaudi.Configuration import *
from Configurables import ( MisAligner, LHCbApp )

MisAligner().DDDBtag = "default"
MisAligner().CondDBtag = "default"
MisAligner().Detectors = ["TT"]
MisAligner().OutputLevel = 3

# In order to have LHCbApp execute also the execute chain,
# we need to loop over data... even if it's only a single event.
# Therefore, we need data:
from Configurables import EventClockSvc
EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";
#EventSelector().Input = ["DATAFILE='PFN:/data/data/FieldOff_450GeV.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

from GaudiConf import IOHelper
IOHelper().inputFiles( ["PFN:/castor/cern.ch/user/j/jblouw/FieldOff_450GeV.dst"] )

#EventSelector().Input = ["DATAFILE='PFN:/work/blouw/data//FieldOff_450GeV.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
