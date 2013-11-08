from Gauss.Configuration import *
from Configurables import Gauss

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB
LHCbApp().DDDBtag    = "dddb-20131108"
LHCbApp().CondDBtag  = "sim-20131108-vc-md100"


