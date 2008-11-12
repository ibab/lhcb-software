##############################################################################
# File for running a Boole initialisation test with default 2008 geometry
##############################################################################

from Boole.Configuration import *
from Configurables import CondDBAccessSvc

Boole().EvtMax   = 0
Boole().Outputs  = ["DIGI","MDF","L0ETC"] # Test all output types
Boole().Histograms = "NONE"

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Ignore changes to database tags
CondDBAccessSvc('DDDB').OutputLevel = WARNING
CondDBAccessSvc('LHCBCOND').OutputLevel = WARNING
