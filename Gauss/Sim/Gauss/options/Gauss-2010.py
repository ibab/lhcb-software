##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2010 data (3.5 TeV beams, nu=0.14, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2010.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam3500GeV-md100-2010-nu0.14.py")

#--Set database tags using those for Sim08
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20150522"
LHCbApp().CondDBtag = "sim-20150522-vc-md100"

#--Pick up new particle table until it is in a global tag
from Configurables import CondDB
CondDB().LocalTags = { "DDDB":["particles-20150720"] }
