from Gauss.Configuration import *
from Configurables import Gauss

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB
LHCbApp().DDDBtag    = "dddb-20130722"
LHCbApp().CondDBtag  = "sim-20130722-vc-md100"


