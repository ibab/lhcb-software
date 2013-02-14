from Gauss.Configuration import *
from Configurables import Gauss
from Configurables import MessageSvc
# Set output level for various algos

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB

CondDB().Upgrade     = True
LHCbApp().DDDBtag    = "dddb-20121018"
LHCbApp().CondDBtag  = "simcond-20121001-vc-md100"
Gauss().DataType     = "Upgrade" 

