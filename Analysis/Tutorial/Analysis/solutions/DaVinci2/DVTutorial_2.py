########################################################################
# $Id: DVTutorial_2.py,v 1.9 2009-06-11 16:22:28 rlambert Exp $
#
# Options for exercise 2
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
# 2) Add the Tutorial algorithm
#
from Configurables import TutorialAlgorithm
jpsi2mumu = TutorialAlgorithm("Jpsi2MuMu")
tutorialseq.Members += [ jpsi2mumu ]
jpsi2mumu.Inputs = [ "Phys/StdLooseMuons/Particles" ]
from GaudiKernel.SystemOfUnits import MeV
jpsi2mumu.MassWindow = 30*MeV
jpsi2mumu.MaxChi2 = 100 
#######################################################################
#
# 3) Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_2.root"    # Histogram file
DaVinci().EvtMax = 10000                        # Number of events
DaVinci().DataType = "2012"                    # Default is "DC06"
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ tutorialseq ]
DaVinci().MainOptions  = ""                    # None
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci2/DVTutorial_2.py options/DimuonR14S20.py
#
########################################################################
