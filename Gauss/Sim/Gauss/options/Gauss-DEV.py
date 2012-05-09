##############################################################################
# File for running Gauss with latest tags
# (3.5 TeV beams, nu=1, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-DEV.py <someInputJobConfiguration>.py
##############################################################################

from Configurables import Gauss
theApp = Gauss()

##############################################################################
# latest database tags 
##############################################################################
from Configurables import DDDBConf, CondDB, LHCbApp

LHCbApp().DDDBtag   = "head-20120413"   # Must be set for selecting VELO
                                        # geometry list in Gauss()

DDDBConf(DataType = "2011")
CondDB().UseLatestTags = ["2012"]
