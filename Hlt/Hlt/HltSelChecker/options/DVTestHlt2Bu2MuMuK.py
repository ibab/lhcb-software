### @file DVTestHlt2Bu2MuMuK.opts
 #
 #  Test file for HLT Bu->MuMuK* selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-23
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintHeader
from Configurables import CheckSelResult, L0Filter, AlgorithmCorrelationsAlg
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2DILEPTONROOT/options/DVDC06SelBu2MuMuK.py")
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselBu2LLK" ]
PrintHeader("PrintDiLeptonForPreselBu2LLK").OutputLevel = 4
PrintHeader("PrintPreselBu2LLK").OutputLevel = 4
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
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBu2LLK"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B+ -> (^J/psi(1S) => ^mu+ ^mu-) ^K+ ]cc"
#
# correlations on signal
#
ApplicationMgr().TopAlg += [ GaudiSequencer("Corrs") ]
GaudiSequencer("Corrs").Members += [ CheckSelResult("CheckOffline"), L0Filter(), AlgorithmCorrelationsAlg("eeK") ]
CheckSelResult("CheckOffline").Algorithms = [ "DC06SelBu2MuMuK" ]
AlgorithmCorrelationsAlg("eeK").Algorithms = [ "Hlt2SelBu2LLK", "Hlt2SelBu2LLKSignal", "Hlt2SelBu2LLKHighMass", "Hlt2SelBu2LLKJpsi", "Hlt2SelBiasedDiElectron", "Hlt2SelBiasedDiMuon", "Hlt2SelUnbiasedDiMuon", "Hlt2Decision" ]
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2Kmumu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bu2MuMuK.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bu2MuMuK.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
