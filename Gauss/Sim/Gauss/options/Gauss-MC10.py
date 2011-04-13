##############################################################################
# File for running Gauss with MC10 configuration and beam conditions as in
# production (3.5 TeV beams, nu=1, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-MC09.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Beam3500GeV-md100-MC10-nu2,5.py")

##############################################################################
# Database tags must be set and are defined in a separate file
##############################################################################
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "head-20101206"
LHCbApp().CondDBtag = "MC10-20101210-vc-md100"
