### @file
#
#   Hlt 2 selections test job
#
#   @author P. Koppenburg
#   @date 2008-08-04
# /
from Gaudi.Configuration import *
from Configurables import HistogramPersistencySvc, ReadHltSummary, GaudiSequencer, PrintHeader, HltSelectionsDecision
#
signal = 'HidValley'
#
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
# 
# Uncomment to Read HltSummary and printout candidates
#
ReadHltSummary().PrintParticles = TRUE
ReadHltSummary().Context = "HLT" 

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
DaVinci().MoniSequence += [ moni, ReadHltSummary() ]
DaVinci().Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_001.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_002.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_003.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_004.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_005.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_006.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/m/marcin/HiddenValleySignal/Higgs120_mA35_tA_100/HV_Higgs_120_35_100_007.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

GaudiSequencer("Hlt2SelHidValleyFilterSequence").Members += [ PrintHeader("FoundHidValley") ]

#HltSelectionsDecision("Hlt2Decision").OutputLevel = 2 
#ReadHltSummary().OutputLevel = 2 
