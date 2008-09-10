##############################################################################
# File for running a Boole initialisation test with default 2008 geometry
##############################################################################

from Boole.Configuration import *
from Configurables import CondDBAccessSvc

Boole().EvtMax       = 0
Boole().DDDBtag      = "2008-default"
Boole().condDBtag    = "2008-default"
Boole().writeL0ETC   = True   # Write also ETC of L0 selected events

Boole().applyConf()

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Ignore changes to database tags
CondDBAccessSvc('DDDB').OutputLevel = WARNING
CondDBAccessSvc('SIMCOND').OutputLevel = WARNING

# Switch off output
ApplicationMgr().OutStream = []
