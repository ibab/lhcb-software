### @file
#
#   Hlt 2 selections test job
#
#   @author P. Koppenburg
#   @date 2008-08-04
# /
from Gaudi.Configuration import *
from Configurables import HistogramPersistencySvc, ReadHltSummary, GaudiSequencer, PrintHeader, HltSelectionsDecision
importOptions ("$DAVINCIROOT/options/DaVinciCommon.opts")
##################################################################
# 
# to get Hlt1 only
#
#importOptions( "$L0DUROOT/options/ReplaceL0DUBankWithEmulated.opts" )
#
# Temporary hack to get L0 to do what it should
#
importOptions('$L0DUROOT/options/L0DUBankSwap.opts')
importOptions('$L0DUROOT/options/DefaultTCK.opts')
#
importOptions( "$HLTSYSROOT/options/Hlt1.opts")
##################################################################
# 
# to get Hlt2 only
#
#GaudiSequencer("Hlt2").Members += [ "L0Filter" ]
importOptions( "$HLTSYSROOT/options/Hlt2.opts")
##################################################################
# 
# to get Hlt2 correlations
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
##################################################################
# 
# to get Hlt2 moniror plots
#
# importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
##################################################################
# 
# Uncomment to Read HltSummary and printout candidates
#
ApplicationMgr().TopAlg += [ ReadHltSummary() ]
ReadHltSummary().PrintParticles = TRUE
ReadHltSummary().Context = "HLT" 
##################################################################
# 
# Save histograms
#
ApplicationMgr().HistogramPersistency = "ROOT"
HistogramPersistencySvc().OutputFile = "DVHlt2.root"
##################################################################
# 
# Events
#
EventSelector().FirstEvent = 1 
ApplicationMgr().EvtMax = 100
EventSelector().PrintFreq = 1
# EventSelector().FirstEvent = 60
##################################################################

# HiddenValley
EventSelector().Input = [
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
