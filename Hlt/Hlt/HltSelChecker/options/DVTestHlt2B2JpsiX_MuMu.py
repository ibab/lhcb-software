### @file DVTestHlt2B2JpsiX_MuMu.opts
 #
 #  Test file for HLT B -> J/psi(MuMu) X selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintHeader
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$CCBARROOT/options/DVPreselHeavyDimuon.opts")
HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "PreselHeavyDimuon" ]
PrintHeader("PrintPreselHeavyDimuon").OutputLevel = 3 
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
#
# Plots
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# True filter criterion
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{<Xb>, <Xb~>, [<Xb~>]os, [<Xb>]os} -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}{,gamma}) ..." 
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
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelB2JpsiX_MuMu"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "J/psi(1S) -> ^mu+ ^mu-" 
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bs2psiphi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-B2JpsiX_MuMu.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-B2JpsiX.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
