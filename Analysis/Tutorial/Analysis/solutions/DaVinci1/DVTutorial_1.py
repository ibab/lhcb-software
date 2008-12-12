########################################################################
#
# Options for exercise 1
#
# @author Patrick Koppenburg
# @date 2008-11-27
#
########################################################################
from Gaudi.Configuration import *
from Configurables import TutorialAlgorithm, PhysDesktop

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

#
# Let's make it a sequence
#
ApplicationMgr().TopAlg += [ "GaudiSequencer/TutorialSeq" ]
#
# J/psi->mumu selection
#
GaudiSequencer("TutorialSeq").Members += [ "TutorialAlgorithm" ]
TutorialAlgorithm().addTool( PhysDesktop() )
TutorialAlgorithm().PhysDesktop.InputLocations = [ "StdLooseMuons" ]

HistogramPersistencySvc().OutputFile = "DVHistos_1.root"

ApplicationMgr().EvtMax = 1000 

