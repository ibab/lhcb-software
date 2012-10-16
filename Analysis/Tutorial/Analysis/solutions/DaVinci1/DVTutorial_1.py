########################################################################
# $Id: DVTutorial_1.py,v 1.8 2009-06-11 16:22:28 rlambert Exp $
#
# Options for exercise 1
#
# @author Patrick Koppenburg
# @date 2008-11-27
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
tutalg = TutorialAlgorithm()
tutorialseq.Members += [ tutalg ]
tutalg.Inputs = [ "Phys/StdLooseMuons/Particles" ]
#######################################################################
#
# 3) Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2012"                    # 
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ tutorialseq ]
########################################################################
#
#
########################################################################
