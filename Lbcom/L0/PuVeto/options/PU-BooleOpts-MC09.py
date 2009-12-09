"""
###############################################################################
#  
#  Example to run the PU emulator in Boole                                    #
#                                                                             #
#  Example usage:                                                             #
#    gaudirun.py Boole-Default.py PU-BooleOpts-MC09.py                        #
#                                                                             #
#                                                                             #
###############################################################################
"""

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp
from GaudiKernel.Configurable import appendPostConfigAction

from Configurables import PuVetoFillRawBuffer
from Configurables import PuVetoAlg


# Minimum bias events from Gauss v37r2, default MC09 settings
datasetName = 'MinBias-5ev-20090715'
EventSelector().Input = ["DATAFILE='PFN:$HOME/cmtuser/Gauss_v37r2/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]


PuVetoFillRawBuffer().OutputLevel = 2
PuVetoAlg("L0PuVeto").OutputLevel = 2 #instantiate PuVetoAlg with a new name
PuVetoAlg("L0PuVeto").OutputFileName = "PUVetoAlg-"+datasetName+".root"
PuVetoAlg("L0PuVeto").MakePlots = True
#def PUemu():
#   l0SimulationSeq = GaudiSequencer("L0SimulationSeq")
#   l0SimulationSeq.Members.remove( "PuVetoFillRawBuffer" )
#   l0SimulationSeq.Members.insert( 0, "PuVetoFillRawBuffer_vs2" )
#   print "Check L0 simulation sequence: ", l0SimulationSeq.Members
#appendPostConfigAction( PUemu )


#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20090508"
LHCbApp().CondDBtag = "sim-20090508-vc-mu100"


# Default output files names are set up using value Boole().DatasetName property
Boole().DatasetName = datasetName
# Redefine defaults by uncommenting one or more of options below 

OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + datasetName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
Boole().DigiType = 'Extended'

