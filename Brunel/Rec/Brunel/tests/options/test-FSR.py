# Test, will read teh FSR from a DST file
from Gaudi.Configuration import *
from Configurables import EventSelector, LHCbApp
from LumiAlgs.Configuration import *

#Select DST file
EventSelector().Input   = [
  "DATAFILE='PFN:bigEvents-2010.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]

# Run LumiTest
#importOptions("$LUMIALGSROOT/job/LumiAlgsConfTest.py")

#select correct data format
LumiAlgsConf().InputType='DST'
LumiAlgsConf().DataType='2009'
#print verbose output
LumiAlgsConf().OutputLevel=1

lSeq=GaudiSequencer("LumiSequence")

LumiAlgsConf().LumiSequencer=lSeq

lSeq.OutputLevel=1

LHCbApp().EvtMax = 100

ApplicationMgr().TopAlg+=[lSeq]
ApplicationMgr().OutputLevel=1

