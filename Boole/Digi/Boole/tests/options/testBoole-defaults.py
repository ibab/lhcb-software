# Default settings for all Boole tests
from Boole.Configuration import *
from Configurables import CondDBAccessSvc

Boole().EvtMax   = 10
Boole().Outputs  = ["DIGI","MDF","L0ETC"] # Test all output types
Boole().Monitors = ["SC","FPE"]           # Add StatusCode and FPE checks

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Ignore changes to database tags
CondDBAccessSvc('DDDB').OutputLevel = WARNING
CondDBAccessSvc('LHCBCOND').OutputLevel = WARNING
