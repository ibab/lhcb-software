##############################################################################
# File for running Gauss with postMC09 configuration
# 
#
# Syntax is:
#   gaudirun.py Gauss-DEV.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Beam450GeV-md100-fix1.py")

##############################################################################
# latest database tags 
##############################################################################
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "head-20100119"
LHCbApp().CondDBtag = "sim-20100119-vc15mm-md100"

