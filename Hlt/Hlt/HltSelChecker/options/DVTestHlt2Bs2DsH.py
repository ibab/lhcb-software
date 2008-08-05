### @file DVTestHlt2Bs2DsH.opts
 #
 #  Test file for HLT Bs->Ds(KKpi)(K,pi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
 #/
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2DSXROOT/options/DVPreselBs2DsH.opts")
HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "PreselBs2DsH" ]
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
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^K- {,gamma}{,gamma}]cc, [[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^pi- {,gamma}{,gamma}]cc}"  
FilterTrueTracks("FilterTrueMuons").addTool(MCDecayFinder())
FilterTrueTracks("FilterTrueMuons").MCDecayFinder.Decay =  "{[[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^K- {,gamma}{,gamma}]cc, [[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^pi- {,gamma}{,gamma}]cc}" 
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Decay Tree Tuple
#
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBs2DsH"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B_s0 -> (^D_s- => ^K+ ^K- ^pi- ) ^pi+ ]cc" 
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2Dspi-Decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bs2DsH.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bs2DsH.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 

