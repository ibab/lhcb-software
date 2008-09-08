##############################################################################
# File for running a Boole test with default 2008 geometry
##############################################################################

from Boole.Configuration import *
from Configurables import CondDBAccessSvc

Boole().EvtMax       = 10
Boole().useSpillover = False
Boole().DDDBtag      = "2008-default"
Boole().condDBtag    = "2008-default"
Boole().writeL0ETC   = True   # Write also ETC of L0 selected events
Boole().monitors    += ["FPE", "SC"] # Add Floating Point and StatusCode checks

Boole().applyConf()

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Ignore changes to database tags
CondDBAccessSvc('DDDB').OutputLevel = WARNING
CondDBAccessSvc('SIMCOND').OutputLevel = WARNING

# Data files
importOptions( "$BOOLEOPTS/2008-Files.py" ) 
