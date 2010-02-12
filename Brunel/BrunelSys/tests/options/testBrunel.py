#####################################################################
# File for running a Brunel initialisation test with default geometry
#####################################################################

from Configurables import Brunel
from Gaudi.Configuration import *

Brunel().EvtMax    = 0
Brunel().InputType = "MDF"
Brunel().Monitors  = ["FPE","SC"]

# Switch off output
ApplicationMgr().OutStream = []
