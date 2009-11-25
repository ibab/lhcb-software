##############################################################################
# File for running Gauss with postMC09 configuration
# (3.5 TeV beams, nu=1, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-DEV.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/MC09-b3,5TeV-md100-nu1.py")

##############################################################################
# latest database tags 
##############################################################################
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "head-20091120"
LHCbApp().CondDBtag = "sim-20091112-vc-md100"

