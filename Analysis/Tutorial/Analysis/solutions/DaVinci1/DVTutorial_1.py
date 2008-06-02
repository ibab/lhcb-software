########################################################################
#
# Options for exercise 1
#
# @author Marco Cattaneo
# @date 2008-06-02
#
########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import TutorialAlgorithm, PhysDesktop

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

#
# Let's make it a sequence
#
ApplicationMgr().TopAlg += [ "GaudiSequencer/TutorialSeq" ]
#
# J/spi->mumu selection
#
GaudiSequencer("TutorialSeq").Members += [ "TutorialAlgorithm" ]
TutorialAlgorithm().addTool( PhysDesktop() )
TutorialAlgorithm().PhysDesktop.InputLocations = [ "Phys/StdLooseMuons" ]

HistogramPersistencySvc().OutputFile = "DVHistos_1.root";

ApplicationMgr().EvtMax = 1000 ;

# load input data
daVinciTutorialData = os.environ["DAVINCITUTORIALDATA"]
importOptions( daVinciTutorialData )
########################################################################
