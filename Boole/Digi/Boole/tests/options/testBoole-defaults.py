# Default settings for all Boole tests
from Boole.Configuration import *

Boole().EvtMax   = 10
Boole().Outputs  = ["DIGI","MDF"] # Test all output types
Boole().Monitors = ["SC","FPE"]   # Add StatusCode and FPE checks
