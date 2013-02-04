# Default settings for all Brunel tests
from Brunel.Configuration import *

Brunel().EvtMax   = 10
Brunel().Monitors = ["SC","FPE"]           # Add StatusCode checks

importOptions("$BRUNELROOT/tests/options/enableANNPrintout.py")

# Disable known FPEs...
#from Configurables import FPEAuditor
#FPEAuditor().DisableTrapFor += ['MergedPi0Rec']

# Use current time in initialisation
#from datetime import datetime
#from Configurables import EventClockSvc
#ecs = EventClockSvc()
#dt = datetime.utcnow() - datetime(1970, 1, 1, 0)
#ns = (dt.days * 24 * 60 * 60 + dt.seconds) * 1000000000
#ecs.InitialTime = ns
