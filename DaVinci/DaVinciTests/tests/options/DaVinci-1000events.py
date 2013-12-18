from Gaudi.Configuration import *
from Configurables import DaVinci

# Number of events etc.
DaVinci().EvtMax    = 1000
DaVinci().PrintFreq = 10

# Temporary. Force a particular CondDB tag
#from Configurables import LHCbApp
#LHCbApp().DDDBtag="head-20120413"
#LHCbApp().CondDBtag="cond-20131028"
