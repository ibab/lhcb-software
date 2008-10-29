### @file DVTestHlt2Bu2D0K_KsHH.opts
 #
 #  Test file for HLT Bd->D0(Kpi)K*(Kpi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-25
 #/
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintHeader, CheckSelResult
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2D0XROOT/options/DVPreselBu2D0K_D02KsPiPi.opts")
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselBu2D0K_D02KsPiPi" ]
PrintHeader("PrintPreselBu2D0K_D02KsHH").OutputLevel = 3 
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
FilterTrueTracks().MCDecayFinder.Decay =  "[B+ -> (D~0 -> (KS0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc" 
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
 #/
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBu2D0K_D02KsHH_Ks2DD"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "{[B+ -> (^D0 -> (^KS0 -> ^pi+ ^pi-) ^pi+ ^pi-) ^K+]cc, [B+ -> (^D~0 -> (^KS0 -> ^pi+ ^pi-) ^pi+ ^pi-) ^K+]cc}"
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2KD-KSPiPi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bu2D0K_KsHH_KsDD.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bu2D0K_KsHH_KsDD.root"


ApplicationMgr().EvtMax = -1 
#EventSelector().FirstEvent = 815 
EventSelector().PrintFreq = 10
