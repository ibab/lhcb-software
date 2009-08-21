### $Id: DVTestHlt2Bd2DstarMu.py,v 1.2 2009-08-21 14:37:50 pkoppenb Exp $
 #
 #  Test file for HLT Bd->D*MuNu
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
 #/
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, CheckSelResult
#--------------------------------------------------------------
signal = "Bd2DstarMu"
#
# True filter criterion - will only run HLT on True signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder)
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]nos -> nu_mu ^mu+ (D*(2010)- -> ^pi- (D~0 -> ^K+ ^pi-))]cc, [[B0]os -> nu_mu~ ^mu- (D*(2010)+ -> ^pi+ (D0 -> ^K- ^pi+))]cc}" 
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqTrueSignalTracks").IgnoreFilterPassed = True
#
# Overwrite input - uncomment to run HLT on True signal only
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
#
# Decay Tree Tuple
#
#importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
#DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2B2DplusHCombine"]
#DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B_s0 -> (^D+ => ^K+ ^pi+ ^pi- ) ^pi- ]cc" 
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
DaVinci().MoniSequence += [ moni ] #, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bd2DstarMu-lum2-fixedL0.dst' TYP='POOL_ROOTTREE' OPT='READ'"
  ]
