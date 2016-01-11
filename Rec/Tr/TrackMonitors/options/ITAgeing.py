##############################################################################
#                                                                            #
#  Configuration for ST Aging CCE scan analysis for IT                       #
#                                                                            #
#  @package Tr/TrackMonitors                                                 #
#  @author  Ch. Elsasser, E. Graverini                                       #
#  @date    15/06/2013                                                       #
#                                                                            #
##############################################################################


from GaudiKernel.ProcessJobOptions import importOptions

from Configurables import GaudiSequencer
from TrackSys.Configuration import *
from Configurables import ( TrackSys, RecSysConf )
from Configurables import TrackContainerCopy
from Configurables import Velo__VeloTrackMonitor
from Configurables import EventClockSvc
from Configurables import Vetra
from Configurables import VetraRecoConf
from Configurables import ST__STDumpADCs
 


generalOutputLevel = 3

#Vetra().MainSequence = [ 'ProcessPhase/Reco', GaudiSequencer('MoniTTSeq'), GaudiSequencer('MoniSTSeq')]
Vetra().MainSequence = [ 'ProcessPhase/Reco', GaudiSequencer('MoniITSeq'), GaudiSequencer('MoniSTSeq')]

TrackSys().ExcludedLayers = ["T3X2"]
#TrackSys().TrackPatRecAlgorithms = TrackSys().DefaultPatRecAlgorithmsRun2
#VetraRecoConf().Sequence = [ "Decoding" ] + RecSysConf().DefaultTrackingSubdets # = ["Decoding", "VELO","TT","IT","OT","Tr","Vertex"]
VetraRecoConf().Sequence = ["Decoding", "VELO","TT","IT","OT","TrHLT1", "Vertex", "TrHLT2" ]
VetraRecoConf().TrackPatRecAlgorithms = TrackSys().DefaultPatRecAlgorithmsRun2

print '----------------------------------------------------------------------------------------------------\n\n\n\n\n'
print TrackSys().DefaultPatRecAlgorithms
print RecSysConf().DefaultTrackingSubdets
print '----------------------------------------------------------------------------------------------------\n\n\n\n\n'

from Configurables import CondDB, CondDBAccessSvc, CondDBTimeSwitchSvc
connection="sqlite_file:$STSQLDDDBROOT/db/STCOND.db/COND"
CondDB().addLayer( CondDBAccessSvc("COND", ConnectionString = connection) )
CondDB().IgnoreHeartBeat = True
CondDB().EnableRunStampCheck = False


#importOptions('$HOME/cmtuser/Vetra_v15r0/ST/STVetraAnalysis/options/TTEmulator.py')
#importOptions('$STVETRAANALYSISROOT/options/TTEmulator.py')
importOptions('$STTELL1ALGORITHMSROOT/options/ITEmulator.py')

from Configurables import STNZSResolution, STADCTrackMonitor

#ODINChecker = STODINCheck()
#ODINChecker.OutputLevel = generalOutputLevel
#ODINChecker.ODINData = "DAQ/ODIN";

#GaudiSequencer('ODINPreChecker').Members = [ODINChecker]

ttRes = STNZSResolution("ITResolution")
ttRes.OutputLevel = 4
ttRes.DetType = "IT"
ttRes.UseNZSdata = True
ttRes.InputData =  "/Event/Raw/IT/LCMSADCs";

TrackMonitor = STADCTrackMonitor()
TrackMonitor.OutputLevel = generalOutputLevel
TrackMonitor.DetType = "IT"
TrackMonitor.RawDataCent = "/Event/Raw/IT/LCMSADCs";
TrackMonitor.ODINData = "DAQ/ODIN";
TrackMonitor.Layer   = "T3X2"
TrackMonitor.MaxNeighbours = 3



GaudiSequencer('MoniITSeq').Members = [ GaudiSequencer('EmuITSeq'), GaudiSequencer('EmuITSeqPrev'), GaudiSequencer('EmuITSeqNext'),TrackMonitor]
GaudiSequencer('MoniITSeq').IgnoreFilterPassed = True#ReturnOK = True

histoname = "Histo.root"
tuplename = "Tuple.root"

Vetra().EvtMax =         -1
#Vetra().OutputLevel =    INFO
Vetra().HistogramFile =  histoname
Vetra().TupleFile =      tuplename
Vetra().DataType = '2015'
vetra = Vetra()

def doMyChanges():
    from Configurables import VetraInit, FilterByBankType
    bankFilter = FilterByBankType()
    bankFilter.BankNames = [ "TTFull", "ITFull" ]
    from Configurables import LoKi__ODINFilter
    odinFilter = LoKi__ODINFilter('BBODINFilter', Code = " LHCb.ODIN.BeamCrossing == ODIN_BXTYP ")
    GaudiSequencer( 'Init' ).Members = [   VetraInit(), odinFilter, bankFilter   ]
appendPostConfigAction(doMyChanges)
