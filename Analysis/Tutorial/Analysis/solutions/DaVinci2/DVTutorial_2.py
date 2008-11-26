########################################################################
#
# Options for exercise 2
#
# @author Patrick Koppenburg
# @date 2008-06-02
#
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units 
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
jpsi2mumu = TutorialAlgorithm("Jpsi2MuMu");

jpsi2mumu.addTool( PhysDesktop() )
jpsi2mumu.PhysDesktop.InputLocations = [ "Phys/StdLooseMuons" ]
jpsi2mumu.MassWindow = 30*Units.MeV 
jpsi2mumu.OutputLevel = 3 ;

GaudiSequencer("TutorialSeq").Members.append(jpsi2mumu)

HistogramPersistencySvc().OutputFile = "DVHistos_2.root";

ApplicationMgr().EvtMax = 1000 ;
