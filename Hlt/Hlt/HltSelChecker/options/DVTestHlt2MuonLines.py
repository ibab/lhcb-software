### @file DVTestHlt2Bd2MuMuKstar.opts
 #
 # Test file for HLT Bd->MuMuK* selection
 #
 # @author P. Koppenburg
 # @date 2007-07-20
###
#--------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
#
# Plots
#
#importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# True filter criterion
#
#importOptions( "$HLTSELCHECKERROOT/options/TrueBd2MuMuKstar.opts")
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
#importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Options
#
importOptions( "$HLTSELECTIONSROOT/options/MinimumBiasPFN.opts")

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr.ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bd2MuMuKstar.root' TYP='ROOT' OPT='NEW'" ] 

###
 #Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelUnbiasedDiMuon"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "J/psi(1S) => ^mu+ ^mu-"
# special for head
#DecayTreeTuple("Hlt2DecayTreeTuple").Branches = [
#  "Head" : "[B0]cc : [B0 -> (J/psi(1S) => mu+ mu-) (K*(892)0 -> K+ pi-)]cc" 

