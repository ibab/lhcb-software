########################################################################
# $Id: DVTutorial_2.py,v 1.6 2008-12-22 18:08:36 pkoppenb Exp $
#
# Options for exercise 2
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
# 2) Add the Tutorial algorithm
#
from Configurables import TutorialAlgorithm, PhysDesktop
jpsi2mumu = TutorialAlgorithm("Jpsi2MuMu")
tutorialseq.Members += [ jpsi2mumu ]
jpsi2mumu.addTool( PhysDesktop() )
jpsi2mumu.PhysDesktop.InputLocations = [ "StdLooseMuons" ]
from GaudiKernel.SystemOfUnits import MeV
jpsi2mumu.MassWindow = 30*MeV 
#######################################################################
#
# 3) Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_2.root"    # Histogram file
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
