from Configurables import CondDB, Gauss
CondDB().Upgrade = True
Gauss().DataType = "Upgrade"

from Configurables import LHCbApp
LHCbApp().EvtMax    = 1
LHCbApp().DDDBtag   = "dddb-20150729"
LHCbApp().CondDBtag = "sim-20150716-vc-md100"
