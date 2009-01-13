### $Id: DVTestHlt2B2HH.py,v 1.7 2009-01-13 08:36:21 pkoppenb Exp $
 #
 #  Test file for HLT B->hh selection
 #
 #  @author P. Koppenburg
 #  @date 2007-05-05
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintTree, CheckSelResult
#--------------------------------------------------------------
signal = "B2HH"
#
# Preselection
#
importOptions( "$DAVINCIROOT/options/PreloadUnits.opts")
# BROKEN # importOptions( "$B2UDSROOT/options/DVPreselB2HH.opts")
#
# True filter criterion
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[B0 -> ^pi- ^pi+ {,gamma}{,gamma}{,gamma}]cc, [[B0]nos -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}]cc, [[B0]os -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}]cc, [[B_s0]nos -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}]cc, [[B_s0]os -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}]cc, [B_s0 -> ^K- ^K+ {,gamma}{,gamma}{,gamma}]cc}"
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelB2HH"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "B0 -> ^pi+ ^pi-"
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
DaVinci().UserAlgorithms = [ ] 
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2Kpi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
