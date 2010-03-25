##############################################################################
# File for running Gauss with postMC09 configuration
# (3.5 TeV beams, nu=1, no spill-over)
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
#LHCbApp().CondDBtag = "MC-20100205-vc15mm-md100"  # as in gauss v38r3
LHCbApp().CondDBtag = "MC-2010321-vc15mm-md100"  # latest simcond for 2009

