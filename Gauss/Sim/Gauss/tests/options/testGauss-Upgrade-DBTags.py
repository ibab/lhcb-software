from Gauss.Configuration import *
from Configurables import Gauss

#--Number of events
nEvts = 1
LHCbApp().EvtMax = nEvts

from Configurables import LHCbApp, CondDB
LHCbApp().DDDBtag    = "dddb-20131025"
LHCbApp().CondDBtag  = "sim-20130830-vc-md100"


