##############################################################################
# GAUSS PARTICLE GUN SIMULATION FOR RESOLUTION BIAS STUDIES
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam4000GeV-md100-2012-nu2.5.py")

#--Set database tags using those for Sim08
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "Sim08-20130503-1"
LHCbApp().CondDBtag = "Sim08-20130503-1-vc-md100"

from Gauss.Configuration import *
Gauss().Production = 'PGUN'

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts = 250000
LHCbApp().EvtMax = nEvts

Gauss().Phases = ["Generator","GenToMCTree"]
#Gauss().Output = 'NONE'
#Gauss().Histograms = 'NONE'
