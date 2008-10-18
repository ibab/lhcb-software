### @file DVTestHlt2Bs2JpsieePhi.py
 #
 #  Test file for HLT Bs -> Jpsi(ee) Phi selection
 #
 #  @author A. Hicheur
 #  @date 2008-10-18
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintHeader, PrintTree
from Configurables import CheckSelResult, L0Filter, SelResultCorrelations
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$CCBARROOT/options/DVDC06SelBs2JpsieePhi.py")
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "DC06SelBs2JpsieePhi" ]

#PrintHeader("PrintDC06SelBs2JpsieePhi").OutputLevel = 4

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
FilterTrueTracks().MCDecayFinder.Decay =  "[B_s0 -> (phi(1020)-> ^K+ ^K- {,gamma}{,gamma})(J/psi(1S) -> ^e+ ^e- {,gamma}{,gamma})]cc"
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
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBs2JpsieePhi"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "B_s0 -> (^phi(1020) => ^K+ ^K-) (^J/psi(1S) => ^e+ ^e-)"#
# Options
#
EventSelector().Input   = [
 "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001581/DST/0000/00001581_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]   


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#


ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bs2JpsieePhi.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bs2JpsieePhi.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 

