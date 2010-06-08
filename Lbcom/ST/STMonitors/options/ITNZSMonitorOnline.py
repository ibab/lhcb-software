from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

MoniITNZSSeq = GaudiSequencer( 'MoniITNZSSeq' )
MoniITNZSSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( MoniITNZSSeq )

from Configurables import (STNZSMonitor, ST__STNoiseCalculation, ST__STNoiseCalculationTool,
                           ST__STCMSNoiseCalculationTool )

#==============================================================================
# Raw noise calculation...
#==============================================================================
itNoiseCalcCentral = ST__STNoiseCalculation("ITNoiseCalculationCentral")
itNoiseCalcCentral.NoiseToolType = "ST::STCMSNoiseCalculationTool"
itNoiseCalcCentral.NoiseToolName = "ITNoiseCalculationToolCentral"
itNoiseCalcCentral.OutputLevel = 3;

itNZSMonitorCentral = STNZSMonitor("ITNZSMonitorCentral")
itNZSMonitorCentral.DetType = "IT"
itNZSMonitorCentral.UseSourceID = False
itNZSMonitorCentral.NoiseToolType = "ST::STCMSNoiseCalculationTool"
itNZSMonitorCentral.NoiseToolName = "ITNoiseCalculationToolCentral"
itNZSMonitorCentral.OutputLevel = 3
itNZSMonitorCentral.UseODINTime = True
itNZSMonitorCentral.UseRawNoise = True

MoniITNZSSeq.Members.append(itNoiseCalcCentral)
MoniITNZSSeq.Members.append(itNZSMonitorCentral)

toolCMSCentral = ST__STCMSNoiseCalculationTool("ITNoiseCalculationToolCentral")
toolCMSCentral.DetType = "IT"
toolCMSCentral.FollowPeriod = 0
toolCMSCentral.CountRoundRobin = True
toolCMSCentral.OutputLevel = 3
toolCMSCentral.CondPath = "ITCondDB"

itCMSMonitorCentral = STNZSMonitor("ITCMSMonitorCentral")
itCMSMonitorCentral.DetType = "IT"
itCMSMonitorCentral.UseSourceID = False
itCMSMonitorCentral.NoiseToolType = "ST::STCMSNoiseCalculationTool"
itCMSMonitorCentral.NoiseToolName = "ITNoiseCalculationToolCentral"
itCMSMonitorCentral.OutputLevel = 3
itCMSMonitorCentral.UseODINTime = True

MoniITNZSSeq.Members.append(itCMSMonitorCentral)
