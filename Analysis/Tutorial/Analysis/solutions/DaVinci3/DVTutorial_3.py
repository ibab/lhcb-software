########################################################################
#
# Options for exercise 3
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
jpsi2mumu.PhysDesktop.InputLocations = [ "StdLooseMuons" ]
jpsi2mumu.MassWindow = 30*Units.MeV 
jpsi2mumu.OutputLevel = 3 ;
jpsi2mumu.MaxChi2 = 100 ;
jpsi2mumu.Particle = "J/psi(1S)" ;

GaudiSequencer("TutorialSeq").Members.append(jpsi2mumu)
#
# Phi->KK selection
#
phi2kk = TutorialAlgorithm("Phi2KK");

phi2kk.addTool( PhysDesktop() )
phi2kk.PhysDesktop.InputLocations = [ "StdLooseKaons" ]
phi2kk.MassWindow = 50*Units.MeV 
phi2kk.Particle =  "phi(1020)" ;
phi2kk.MaxChi2 = 20 ;
phi2kk.OutputLevel = 3 ;

GaudiSequencer("TutorialSeq").Members.append(phi2kk)

GaudiSequencer("TutorialSeq").IgnoreFilterPassed = True # get all phis

HistogramPersistencySvc().OutputFile = "DVHistos_3.root";

ApplicationMgr().EvtMax = 1000 ;

