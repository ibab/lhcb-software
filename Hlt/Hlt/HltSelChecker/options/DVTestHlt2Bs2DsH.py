### @file DVTestHlt2Bs2DsH.opts
 #
 #  Test file for HLT Bs->Ds(KKpi)(K,pi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
 #/
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, CheckSelResult
#--------------------------------------------------------------
signal = "Bs2DsH"
#
# Preselection
#
importOptions( "$STDOPTS/PreloadUnits.opts")
importOptions( "$B2DSXROOT/options/DoPreselBs2DsH.opts")
#
# True filter criterion - will only run HLT on TRUE signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^K- {,gamma}{,gamma}]cc, [[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^pi- {,gamma}{,gamma}]cc}" 
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = True
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# Decay Tree Tuple
#
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelB2DplusH"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B_s0 -> (^D+ => ^K+ ^pi+ ^pi- ) ^pi- ]cc" 
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2IgnoreHlt1Decision = True        # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2Dspi-Decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
