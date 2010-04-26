6########################################################################
# $Id: DVTutorial_5.py,v 1.11 2010-04-26 07:39:08 pkoppenb Exp $
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
# Load the sequencer from Ex 4 
#
#
# import the SelectionSequence
from DaVinci4.solutions.Bs2JpsiPhi import SeqBs2JpsiPhi
# get the GaudiSequencer with everything we need
seq = SeqBs2JpsiPhi.sequence()
#######################################################################
#
# Print Reconstructed Bs
#
from Configurables import PrintDecayTree, PrintDecayTreeTool, PhysDesktop
tree = PrintDecayTree("PrintFoundBs")
tree.InputLocations = [ SeqBs2JpsiPhi.outputLocation() ]
tree.addTool( PrintDecayTreeTool, name = "PrintDecay" )
tree.PrintDecay.Information = "Name M P Px Py Pz Pt chi2"
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
#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
DaVinci().UserAlgorithms = [ seq ]             # The selection sequence
DaVinci().MoniSequence = [ tree, mctree ]      # The monitoring stuff
DaVinci().HistogramFile = "DVHistos_5.root"    # Histogram file
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "MC09"                    # Default is "2010"
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ seq ]
DaVinci().MainOptions  = ""                    # None
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci5/DVTutorial_5.py options/Bs2JpsiPhi2008.py
#
########################################################################
