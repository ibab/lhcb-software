##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2012 data (4.0 TeV beams, nu=2.5, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2012.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam4000GeV-md100-2012-nu2.5.py")

#--Set database tags using those for Sim08
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20150522-2"
LHCbApp().CondDBtag = "sim-20150522-2-vc-md100"

#--Pick up new particle table until it is in a global tag
from Configurables import CondDB
CondDB().LocalTags = { "DDDB":["particles-20150720"] }
