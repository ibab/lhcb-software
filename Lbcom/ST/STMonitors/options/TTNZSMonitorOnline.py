from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

MoniTTNZSSeq = GaudiSequencer( 'MoniTTNZSSeq' )
MoniTTNZSSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( MoniTTNZSSeq )

from Configurables import (STNZSMonitor, ST__STNoiseCalculation, ST__STNoiseCalculationTool,
                           ST__STCMSNoiseCalculationTool )

#==============================================================================
# Raw noise calculation...
#==============================================================================
ttNoiseCalcCentral = ST__STNoiseCalculation("TTNoiseCalculationCentral")
ttNoiseCalcCentral.NoiseToolType = "ST::STCMSNoiseCalculationTool"
ttNoiseCalcCentral.NoiseToolName = "TTNoiseCalculationToolCentral"
ttNoiseCalcCentral.OutputLevel = 3;

ttNZSMonitorCentral = STNZSMonitor("TTNZSMonitorCentral")
ttNZSMonitorCentral.DetType = "TT"
ttNZSMonitorCentral.UseSourceID = False
ttNZSMonitorCentral.NoiseToolType = "ST::STCMSNoiseCalculationTool"
ttNZSMonitorCentral.NoiseToolName = "TTNoiseCalculationToolCentral"
ttNZSMonitorCentral.OutputLevel = 3
ttNZSMonitorCentral.UseODINTime = True
ttNZSMonitorCentral.UseRawNoise = True

MoniTTNZSSeq.Members.append(ttNoiseCalcCentral)
MoniTTNZSSeq.Members.append(ttNZSMonitorCentral)

toolCMSCentral = ST__STCMSNoiseCalculationTool("TTNoiseCalculationToolCentral")
toolCMSCentral.DetType = "TT"
toolCMSCentral.FollowPeriod = 0
toolCMSCentral.CountRoundRobin = True
toolCMSCentral.OutputLevel = 3
toolCMSCentral.CondPath = "TTCondDB"

ttCMSMonitorCentral = STNZSMonitor("TTCMSMonitorCentral")
ttCMSMonitorCentral.DetType = "TT"
ttCMSMonitorCentral.UseSourceID = False
ttCMSMonitorCentral.NoiseToolType = "ST::STCMSNoiseCalculationTool"
ttCMSMonitorCentral.NoiseToolName = "TTNoiseCalculationToolCentral"
ttCMSMonitorCentral.OutputLevel = 3
ttCMSMonitorCentral.UseODINTime = True

MoniTTNZSSeq.Members.append(ttCMSMonitorCentral)
