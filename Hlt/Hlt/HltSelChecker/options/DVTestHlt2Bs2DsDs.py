### @file DVTestHlt2Bs2DsDs.opts
 #
 #  Test file for HLT Bs->Ds(KKpi)Ds(KKpi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, PrintHeader, CheckSelResult
#--------------------------------------------------------------
signal = "Bs2DsDs"
#
# True filter criterion - will only run HLT on TRUE signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay = "[B_s0 -> (D_s+ => ^K+ ^K- ^pi+ {,gamma}{,gamma}{,gamma}) (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ...]cc, [B_s0 -> (D*_s+ -> (D_s+ => ^K+ ^K- ^pi+ {,gamma}{,gamma}{,gamma}) ... ) (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ...]cc, [B_s0 -> (D*_s+ -> (D_s+ => ^K+ ^K- ^pi+ {,gamma}{,gamma}{,gamma}) ... ) (D*_s- -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ...) ...]cc,  [B_s0 -> (D_s+ => ^K+ ^K- ^pi+ {,gamma}{,gamma}{,gamma}) (D*_s- -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ...) ...]cc" 
#
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
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bs2DsDsCombine"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B_s0 -> (D_s+ -> ^K+ ^K- ^pi+ ) (D_s- => ^K+ ^K- ^pi- )]cc"
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2DsDs-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
