from Gauss.Configuration import *
from Configurables import Gauss
from Configurables import MessageSvc
# Set output level for various algos

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB

CondDB().Upgrade     = True
LHCbApp().DDDBtag    = "dddb-20130808"
LHCbApp().CondDBtag  = "sim-20130722-vc-md100"
Gauss().DataType     = "Upgrade" 

CondDB().AllLocalTagsByDataType = ["Muon_NoM1"]
