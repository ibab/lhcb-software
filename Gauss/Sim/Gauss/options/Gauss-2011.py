##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2011 data (3.5 TeV beams, nu=2, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2011.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam3500GeV-md100-2011-nu2.py")

#--Set database tags using those for Sim08
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20130312"
LHCbApp().CondDBtag = "sim-20130222-vc-md100"
