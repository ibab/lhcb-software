##############################################################################
# File for running Gauss with MC09 configuration and beam conditions as in
# production (5 TeV beams, nu=1, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-MC09.py <someInputJobConfiguration>.py
##############################################################################

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/MC09-b5TeV-md100.py")

##############################################################################
# Database tags must be set and are defined in a separate file
##############################################################################
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "MC09-20090602"
LHCbApp().CondDBtag = "MC09-20100430-vc-md100"
