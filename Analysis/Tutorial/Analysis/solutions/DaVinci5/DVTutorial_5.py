6########################################################################
# $Id: DVTutorial_5.py,v 1.9 2009-07-01 12:14:16 pkoppenb Exp $
#
# Options for exercise 5
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
#######################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#######################################################################
#
# Load the sequencer from Ex 4 and catch it
#
importOptions("$ANALYSISROOT/solutions/DaVinci4/TutorialSeq.py")
tutorialseq = GaudiSequencer("TutorialSeq")
#######################################################################
#
# Print Reconstructed Bs
#
from Configurables import PrintDecayTree, PrintDecayTreeTool, PhysDesktop
tree = PrintDecayTree("PrintFoundBs")
tree.InputLocations = [ "Bs2JpsiPhi" ]
tree.addTool( PrintDecayTreeTool("PrintDecay") )
tree.PrintDecay.Information = "Name M P Px Py Pz Pt chi2" 
tutorialseq.Members += [ tree ]
#######################################################################
#
# Print  Print All True Bs
#
from Configurables import PrintMCTree, PrintMCDecayTreeTool
mctree = PrintMCTree("PrintTrueBs")
mctree.addTool( PrintMCDecayTreeTool )
mctree.PrintMCDecayTreeTool.Information = "Name M P Px Py Pz Pt chi2" 
mctree.ParticleNames = [  "B_s0", "B_s~0" ]
mctree.Depth = 2  # down to the K and mu
tutorialseq.Members += [ mctree ]
#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
DaVinci().HistogramFile = "DVHistos_5.root"    # Histogram file
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
# gaudirun.py solutions/DaVinci5/DVTutorial_5.py options/Bs2JpsiPhi2008.py
#
########################################################################
