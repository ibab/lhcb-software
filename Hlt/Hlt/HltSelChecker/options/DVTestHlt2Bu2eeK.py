### @file DVTestHlt2Bu2LLK.opts
 #
 #  Test file for HLT Bu->LLK* selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-23
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintHeader, PrintTree
from Configurables import CheckSelResult, L0Filter, AlgorithmCorrelationsAlg
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2DILEPTONROOT/options/DVDC06SelBu2eeK.opts")
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "DC06SelBu2eeK" ]
PrintHeader("PrintDiLeptonForPreselBu2LLK").OutputLevel = 4 
PrintHeader("PrintPreselBu2LLK").OutputLevel = 4
PrintHeader("PrintDC06SelBu2eeK").OutputLevel = 4
PrintTree("PrintTreeDC06SelBu2eeK").OutputLevel = 4
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
from HltConf.Configuration import *
HltConf().Hlt2IgnoreHlt1Decision = True  # do both Hlt1 and 2
HltConf().applyConf()
#
# Plots
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# True filter criterion
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[B+ -> ^K+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc, [B+ -> ^K+ ^e+ ^e- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc}"
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBu2LLKSignal"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B+ -> (^J/psi(1S) => ^e+ ^e-) ^K+]cc"#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bu2Kee-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#
# correlations on signal
#
ApplicationMgr().TopAlg += [ GaudiSequencer("Corrs") ]
GaudiSequencer("Corrs").Members += [ CheckSelResult("CheckOffline"), L0Filter(), AlgorithmCorrelationsAlg("eeK") ]
CheckSelResult("CheckOffline").Algorithms = [ "DC06SelBu2eeK" ]
AlgorithmCorrelationsAlg("eeK").Algorithms = [ "DC06SelBu2eeK", "Hlt2SelBu2LLK", "Hlt2SelBu2LLKSignal", "Hlt2SelBu2LLKHighMass", "Hlt2SelBu2LLKJpsi", "Hlt2SelBiasedDiElectron", "Hlt2SelBiasedDiMuon", "Hlt2SelUnbiasedDiMuon", "Hlt2Decision" ]

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bu2eeK.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bu2eeK.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 1 

