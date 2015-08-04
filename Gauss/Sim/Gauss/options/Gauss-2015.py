##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2015 data (6.5 TeV beams, nu=1.6, spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2015.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Beam6500GeV-md100-nu1.6.py")
importOptions("$APPCONFIGOPTS/Gauss/EnableSpillover-25ns.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2015.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")


#--Set database tags using those for Sim08
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20150724"
LHCbApp().CondDBtag = "sim-20150703-vc-md100"
