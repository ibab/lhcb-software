### @file DVTestHlt2B2DstarMu.opts
 #
 #  Test file for HLT D*->D0(hh)pi selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2DSTARXROOT/options/DVPreselBd2DstarMu.opts")
HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "PreselBd2DstarMu" ]
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
#
# Plots
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# True filter criterion - will only run HLT on TRUE signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]nos -> nu_mu ^mu+ {,gamma}{,gamma} (D*(2010)- -> ^pi- {,gamma}{,gamma} (D~0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma})) ...]cc, [[B~0]os -> nu_mu ^mu+ {,gamma}{,gamma} (D*(2010)- -> ^pi- {,gamma}{,gamma} (D~0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma})) ...]cc, [[B0]nos -> nu_tau {,gamma} (tau+ -> ^mu+ nu_mu nu_tau~ {,gamma}{,gamma}) (D*(2010)- -> ^pi- {,gamma}{,gamma} (D~0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma})) ...]cc, [[B~0]os -> nu_tau {,gamma} (tau+ -> ^mu+ nu_mu nu_tau~ {,gamma}{,gamma}) (D*(2010)- -> ^pi- {,gamma}{,gamma} (D~0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma})) ...]cc}" ;
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelB2DstarMu"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B0 -> ^mu+ (^D*(2010)- -> ^pi-  (^D~0 -> ^K+ ^pi-))]cc"
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2DstMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"


ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-B2DstarMu.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-B2DstarMu.root"


ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 10

