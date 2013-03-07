from Gauss.Configuration import *
from Configurables import Gauss

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB
#LHCbApp().DDDBtag    = "dddb-20130214"
LHCbApp().DDDBtag    = "dddb-20130124"
LHCbApp().CondDBtag  = "simcond-20121001-vc-md100"


