########################################################################
# $Id: DVTutorial_4.py,v 1.3 2008-12-22 18:08:37 pkoppenb Exp $
#
# Options for exercise 4
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
#######################################################################
#
# Load the sequencer and catch it
#
importOptions("$ANALYSISROOT/solutions/DaVinci4/TutorialSeq.py")
tutorialseq = GaudiSequencer("TutorialSeq")
#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_4.root"    # Histogram file
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
# gaudirun.py solutions/DaVinci4/DVTutorial_4.py options/Bs2JpsiPhi2008.py
#
########################################################################
