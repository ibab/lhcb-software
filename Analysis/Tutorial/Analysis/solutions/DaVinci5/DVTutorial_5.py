########################################################################
#
# Options for exercise 5
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units 
from Gaudi.Configuration import *
from Configurables import CombineParticles, TutorialAlgorithm, PhysDesktop
from Configurables import LoKi__Hybrid__PlotTool as PlotTool
from Configurables import PrintTree, PrintMCTree, PrintDecayTreeTool, PrintMCDecayTreeTool

importOptions( "$ANALYSISROOT/solutions/DaVinci4/DVTutorial_4.py" )

#
# Print Reconstructed Bs
#
tree = PrintTree("PrintFoundBs")
tree.addTool( PhysDesktop() )
tree.PhysDesktop.InputLocations = [ "Phys/Bs2JpsiPhi" ]
tree.addTool( PrintDecayTreeTool() )
tree.PrintDecayTreeTool.Information = "Name M P Px Py Pz Pt chi2" 
tree.PrintTruth = True 
GaudiSequencer("TutorialSeq").Members.append(tree)
#
# Print All True Bs
#
mctree = PrintMCTree("PrintTrueBs")
mctree.addTool( PrintMCDecayTreeTool() )
mctree.PrintMCDecayTreeTool.Information = "Name M P Px Py Pz Pt chi2" 
mctree.ParticleNames = [  "B_s0", "B_s~0" ]
mctree.Depth = 2  # down to the K and mu
GaudiSequencer("TutorialSeq").Members.append(mctree)

HistogramPersistencySvc().OutputFile = "DVHistos_5.root"
ApplicationMgr().EvtMax = 100 
EventSelector().PrintFreq = 1 
