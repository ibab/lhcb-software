from Gauss.Configuration import *
from Configurables import Gauss
from Configurables import MessageSvc
# Set output level for various algos

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB

CondDB().Upgrade     = True
Gauss().DataType     = "Upgrade" 

CondDB().AllLocalTagsByDataType = ["Muon_NoM1"]
