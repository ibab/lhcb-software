#####################################################################
# File for running a Brunel initialisation test with default geometry
#####################################################################

from Brunel.Configuration import *
from Configurables import CondDBAccessSvc, MagneticFieldSvc

Brunel().EvtMax       = 0
Brunel().applyConf()

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Ignore changes to database tags
CondDBAccessSvc('DDDB').OutputLevel = WARNING
CondDBAccessSvc('LHCBCOND').OutputLevel = WARNING

# Ignore changes to file paths
CondDBAccessSvc('ONLINE_200808').OutputLevel = WARNING
MagneticFieldSvc().OutputLevel = WARNING

# Switch off output
ApplicationMgr().OutStream = []
