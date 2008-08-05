### @file DVDebug.opts
 # 
 #  Generic options for debugging
 #
 #  @author P. Koppenburg
 #  @date 2007-07-25
 ##
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, PrintMCTree
###--------------------------------------------------------------

GaudiSequencer("SeqHlt2TruthFilter").Members += [ PrintMCTree() ]
PrintMCTree().ParticleNames = [ "B0", "B~0", "B_s0", "B_s~0", "B+", "B-" ]
##
 # type - change here
 ##
importOptions( "$HLTSELCHECKERROOT/options/DVTestHlt2B2DstarMu.py")
ApplicationMgr().EvtMax = 10 
EventSelector().FirstEvent = 1 

