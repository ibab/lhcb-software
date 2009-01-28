# Default settings for all Brunel tests
from Brunel.Configuration import *

Brunel().EvtMax   = 10
Brunel().Monitors = ["SC","FPE"]           # Add StatusCode checks

# Disable known FPEs...
from Configurables import FPEAuditor
FPEAuditor().DisableTrapFor += ['MergedPi0Rec']
