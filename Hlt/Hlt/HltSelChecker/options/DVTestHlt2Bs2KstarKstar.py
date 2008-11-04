 ### @file 
 #  Test file for HLT2, Bs K*0K*~0
 #
 #  @author Diego Martinez Santos (USC)
 #  @date 2008-10-14
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, TupleToolDecay, CheckSelResult
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2UDSROOT/options/DVPreselBs2Kst0Kst0.opts" )
#
# Run correlations only on offline selected events
#
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselBs2Kst0Kst0" ]
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
FilterTrueTracks().MCDecayFinder.Decay =  "[B_s0 -> (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) ( K*(892)~0 -> ^pi+ ^K- {,gamma}{,gamma})]cc"
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
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelBs2KstarKstar"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B_s0 -> (^K*(892)0 -> ^K+ ^pi-)(^K*(892)~0 -> ^K- ^pi+)]cc"
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/diegoms/BsKst0Kst0.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bs2KstarKstar.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bs2KstarKstar.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 10 

