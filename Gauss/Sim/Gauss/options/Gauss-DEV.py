##############################################################################
# File for running Gauss with latest tags
# (3.5 TeV beams, nu=1, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-DEV.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Beam3500GeV-md100-nu1.py")

##############################################################################
# latest database tags 
##############################################################################
from Configurables import DDDBConf, CondDB, LHCbApp

#CondDB().Simulation = True             # Already taken care of in Gauss()
LHCbApp().DDDBtag   = "head-20110914"   # Must be set for selecting VELO
                                        # geometry list in Gauss()

DDDBConf(DataType = "2011")
CondDB().UseLatestTags = ["2011"]

