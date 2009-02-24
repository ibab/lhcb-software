########################################################################
# $Id: DVTutorial_1.py,v 1.7 2009-02-24 15:10:50 pkoppenb Exp $
#
# Options for exercise 1
#
# @author Patrick Koppenburg
# @date 2008-11-27
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
tutalg = TutorialAlgorithm()
tutorialseq.Members += [ tutalg ]
tutalg.addTool( PhysDesktop )
tutalg.PhysDesktop.InputLocations = [ "StdLooseMuons" ]
#######################################################################
#
# 3) Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
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
# gaudirun.py solutions/DaVinci1/DVTutorial_1.py $DAVINCIROOT/options/DaVinciTestData.py
#
########################################################################
