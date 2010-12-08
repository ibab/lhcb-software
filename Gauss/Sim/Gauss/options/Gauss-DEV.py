##############################################################################
# File for running Gauss with postMC09 configuration
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
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "head-20101206"
LHCbApp().CondDBtag = "sim-20101208-vc-md100"

