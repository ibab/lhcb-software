########################################################################
# $Id: DVTutorial_3.py,v 1.8 2009-06-11 16:22:28 rlambert Exp $
#
# Options for exercise 3
#
# @author Patrick Koppenburg
# @date 2008-06-02
#
########################################################################
from Gaudi.Configuration import *
#######################################################################
#
# 1) Let's define a sequence
#
from Configurables import GaudiSequencer
tutorialseq = GaudiSequencer("TutorialSeq")
#######################################################################
#
# 2) Add the J/psi
#
from Configurables import TutorialAlgorithm, PhysDesktop
jpsi2mumu = TutorialAlgorithm("Jpsi2MuMu")
jpsi2mumu.InputLocations = [ "StdLooseMuons" ]
from GaudiKernel.SystemOfUnits import MeV
jpsi2mumu.MassWindow = 30*MeV 
jpsi2mumu.MaxChi2 = 100 
jpsi2mumu.Particle = "J/psi(1S)"
tutorialseq.Members += [ jpsi2mumu ]
#######################################################################
#
# 2) Add the Phi
#
phi2kk = TutorialAlgorithm("Phi2KK");

phi2kk.InputLocations = [ "StdLooseKaons" ]
phi2kk.MassWindow = 50*MeV 
phi2kk.Particle =  "phi(1020)" 
phi2kk.MaxChi2 = 20 
tutorialseq.Members += [ phi2kk ]
#######################################################################
#
# Say True to have all phis. False to be fast.
#
tutorialseq.IgnoreFilterPassed = True # get all phis
#######################################################################
#
# 3) Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_3.root"    # Histogram file
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ tutorialseq ]
DaVinci().MainOptions  = ""                    # None
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci2/DVTutorial_2.py $DAVINCIROOT/options/DaVinciTestData.py
#
########################################################################
