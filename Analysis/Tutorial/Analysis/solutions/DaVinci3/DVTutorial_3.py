########################################################################
# $Id: DVTutorial_3.py,v 1.9 2010-08-05 07:30:52 jpalac Exp $
#
# Options for exercise 3
#
# @author Patrick Koppenburg
# @date 2008-06-02
#
########################################################################
from Gaudi.Configuration import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
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
from Configurables import TutorialAlgorithm
jpsi2mumu = TutorialAlgorithm("Jpsi2MuMu")
jpsi2mumu.Inputs = [ "Phys/StdLooseMuons/Particles" ]
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
phi2kk.Inputs = [ "Phys/StdLooseKaons/Particles" ]
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
DaVinci().DataType = "2012"                    # 
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
