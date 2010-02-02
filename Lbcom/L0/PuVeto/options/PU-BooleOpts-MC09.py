"""
###############################################################################
#                                                                             #
#  Example to run the PU emulator in Boole                                    #
#                                                                             #
#  Example usage                                                              #
#    to run in the latest LHC conditions:                                     #
#    gaudirun.py  $APPCONFIGOPTS/Boole/2009-WithTruth.py PU-BooleOpts-MC09.py #
#    default:                                                                 #
#    gaudirun.py  Boole-Default.py PU-BooleOpts-MC09.py                       #
#                                                                             #
#                                                                             #
###############################################################################
"""

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp
#from GaudiKernel.Configurable import appendPostConfigAction

from Configurables import PuVetoFillRawBuffer
from Configurables import PuVetoAlg


# BeamGas events from Gauss v37r2, default MC09 settings
datasetName = 'BeamGas60001001-100evts'
EventSelector().Input = ["DATAFILE='PFN:/castor/cern.ch/user/s/serena/lhcbPileUp/BeamGas/BeamGas60002008-1000evts-9001.sim' TYP='POOL_ROOTTREE' OPT='READ'"]

Boole().EvtMax = 10

PuVetoFillRawBuffer().OutputLevel = 3
PuVetoAlg("L0PuVeto").OutputLevel = 3 #instantiate PuVetoAlg with a new name
#PuVetoAlg("L0PuVeto").OutputFileName = "PUVetoAlg-"+datasetName+".root"
#PuVetoAlg("L0PuVeto").MakePlots = False

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20100119"
LHCbApp().CondDBtag = "sim-20100119-vc15mm-md100"

# Default output files names are set up using value Boole().DatasetName property
Boole().DatasetName = datasetName
# Redefine defaults by uncommenting one or more of options below 

OutputStream("DigiWriter").Output = "DATAFILE='PFN:/tmp/serena/" + datasetName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
Boole().DigiType = 'Extended'

