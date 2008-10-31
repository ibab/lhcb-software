# Default settings for all Boole tests
from Boole.Configuration import *
from Configurables import CondDBAccessSvc

Boole().EvtMax       = 10
Boole().writeL0ETC   = True   # Write also ETC of L0 selected events
Boole().monitors    += ["SC"] # Add StatusCode check

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Ignore changes to database tags
CondDBAccessSvc('DDDB').OutputLevel = WARNING
CondDBAccessSvc('LHCBCOND').OutputLevel = WARNING
