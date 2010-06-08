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
itNoiseCalcCentral.NoiseToolType = "ST::STNoiseCalculationTool"
itNoiseCalcCentral.NoiseToolName = "ITNoiseCalculationToolCentral"
itNoiseCalcCentral.OutputLevel = 3;

toolCentral = ST__STNoiseCalculationTool("ITNoiseCalculationToolCentral")
toolCentral.DetType = 'IT'
toolCentral.InputData =  "/Event/Raw/IT/Full"
toolCentral.FollowPeriod = 0
toolCentral.CountRoundRobin = True
toolCentral.OutputLevel = 3

itNZSMonitorCentral = STNZSMonitor("ITNZSMonitorCentral")
itNZSMonitorCentral.DetType = "IT"
itNZSMonitorCentral.UseSourceID = False
itNZSMonitorCentral.NoiseToolType = "ST::STNoiseCalculationTool"
itNZSMonitorCentral.NoiseToolName = "ITNoiseCalculationToolCentral"
itNZSMonitorCentral.OutputLevel = 3
itNZSMonitorCentral.UseODINTime = True

# Prev2
itNoiseCalcPrev2 = ST__STNoiseCalculation("ITNoiseCalculationPrev2")
itNoiseCalcPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
itNoiseCalcPrev2.NoiseToolName = "ITNoiseCalculationToolPrev2"
itNoiseCalcPrev2.OutputLevel = 3;

toolPrev2 = ST__STNoiseCalculationTool("ITNoiseCalculationToolPrev2")
toolPrev2.DetType = 'IT'
toolPrev2.InputData =  "/Event/Prev2/Raw/IT/Full"
toolPrev2.FollowPeriod = 0
toolPrev2.CountRoundRobin = True
toolPrev2.OutputLevel = 3

itNZSMonitorPrev2 = STNZSMonitor("ITNZSMonitorPrev2")
itNZSMonitorPrev2.DetType = "IT"
itNZSMonitorPrev2.UseSourceID = False
itNZSMonitorPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
itNZSMonitorPrev2.NoiseToolName = "ITNoiseCalculationToolPrev2"
itNZSMonitorPrev2.OutputLevel = 3
itNZSMonitorPrev2.UseODINTime = True

# Prev1
itNoiseCalcPrev1 = ST__STNoiseCalculation("ITNoiseCalculationPrev1")
itNoiseCalcPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
itNoiseCalcPrev1.NoiseToolName = "ITNoiseCalculationToolPrev1"
itNoiseCalcPrev1.OutputLevel = 3;

toolPrev1 = ST__STNoiseCalculationTool("ITNoiseCalculationToolPrev1")
toolPrev1.DetType = 'IT'
toolPrev1.InputData =  "/Event/Prev1/Raw/IT/Full"
toolPrev1.FollowPeriod = 0
toolPrev1.CountRoundRobin = True
toolPrev1.OutputLevel = 3

itNZSMonitorPrev1 = STNZSMonitor("ITNZSMonitorPrev1")
itNZSMonitorPrev1.DetType = "IT"
itNZSMonitorPrev1.UseSourceID = False
itNZSMonitorPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
itNZSMonitorPrev1.NoiseToolName = "ITNoiseCalculationToolPrev1"
itNZSMonitorPrev1.OutputLevel = 3
itNZSMonitorPrev1.UseODINTime = True

# Next1
itNoiseCalcNext1 = ST__STNoiseCalculation("ITNoiseCalculationNext1")
itNoiseCalcNext1.NoiseToolType = "ST::STNoiseCalculationTool"
itNoiseCalcNext1.NoiseToolName = "ITNoiseCalculationToolNext1"
itNoiseCalcNext1.OutputLevel = 3;

toolNext1 = ST__STNoiseCalculationTool("ITNoiseCalculationToolNext1")
toolNext1.DetType = 'IT'
toolNext1.InputData =  "/Event/Next1/Raw/IT/Full"
toolNext1.FollowPeriod = 0
toolNext1.CountRoundRobin = True
toolNext1.OutputLevel = 3

itNZSMonitorNext1 = STNZSMonitor("ITNZSMonitorNext1")
itNZSMonitorNext1.DetType = "IT"
itNZSMonitorNext1.UseSourceID = False
itNZSMonitorNext1.NoiseToolType = "ST::STNoiseCalculationTool"
itNZSMonitorNext1.NoiseToolName = "ITNoiseCalculationToolNext1"
itNZSMonitorNext1.OutputLevel = 3
itNZSMonitorNext1.UseODINTime = True

# Next2
itNoiseCalcNext2 = ST__STNoiseCalculation("ITNoiseCalculationNext2")
itNoiseCalcNext2.NoiseToolType = "ST::STNoiseCalculationTool"
itNoiseCalcNext2.NoiseToolName = "ITNoiseCalculationToolNext2"
itNoiseCalcNext2.OutputLevel = 3;

toolNext2 = ST__STNoiseCalculationTool("ITNoiseCalculationToolNext2")
toolNext2.DetType = 'IT'
toolNext2.InputData =  "/Event/Next2/Raw/IT/Full"
toolNext2.FollowPeriod = 0
toolNext2.CountRoundRobin = True
toolNext2.OutputLevel = 3

itNZSMonitorNext2 = STNZSMonitor("ITNZSMonitorNext2")
itNZSMonitorNext2.DetType = "IT"
itNZSMonitorNext2.UseSourceID = False
itNZSMonitorNext2.NoiseToolType = "ST::STNoiseCalculationTool"
itNZSMonitorNext2.NoiseToolName = "ITNoiseCalculationToolNext2"
itNZSMonitorNext2.OutputLevel = 3
itNZSMonitorNext2.UseODINTime = True

#MoniITNZSSeq.Members.append(itNoiseCalcPrev2)
#MoniITNZSSeq.Members.append(itNZSMonitorPrev2)
MoniITNZSSeq.Members.append(itNoiseCalcPrev1)
MoniITNZSSeq.Members.append(itNZSMonitorPrev1)
MoniITNZSSeq.Members.append(itNoiseCalcCentral)
MoniITNZSSeq.Members.append(itNZSMonitorCentral)
MoniITNZSSeq.Members.append(itNoiseCalcNext1)
MoniITNZSSeq.Members.append(itNZSMonitorNext1)
#MoniITNZSSeq.Members.append(itNoiseCalcNext2)
#MoniITNZSSeq.Members.append(itNZSMonitorNext2)


#==============================================================================
# CMS noise calculation (Use LCMS ADCs from the emulator)
#==============================================================================
itCMSNoiseCalcCentral = ST__STNoiseCalculation("ITCMSNoiseCalculationCentral")
itCMSNoiseCalcCentral.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSNoiseCalcCentral.NoiseToolName = "ITCMSNoiseCalculationToolCentral"
itCMSNoiseCalcCentral.OutputLevel = 3;

toolCMSCentral = ST__STNoiseCalculationTool("ITCMSNoiseCalculationToolCentral")
toolCMSCentral.DetType = 'IT'
toolCMSCentral.InputData =  "/Event/Raw/IT/LCMSADCs"
toolCMSCentral.FollowPeriod = 0
toolCMSCentral.CountRoundRobin = True
toolCMSCentral.OutputLevel = 3

itCMSMonitorCentral = STNZSMonitor("ITCMSMonitorCentral")
itCMSMonitorCentral.DetType = "IT"
itCMSMonitorCentral.UseSourceID = False
itCMSMonitorCentral.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSMonitorCentral.NoiseToolName = "ITCMSNoiseCalculationToolCentral"
itCMSMonitorCentral.OutputLevel = 3
itCMSMonitorCentral.UseODINTime = True

# Prev2
itCMSNoiseCalcPrev2 = ST__STNoiseCalculation("ITCMSNoiseCalculationPrev2")
itCMSNoiseCalcPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSNoiseCalcPrev2.NoiseToolName = "ITCMSNoiseCalculationToolPrev2"
itCMSNoiseCalcPrev2.OutputLevel = 3;

toolCMSPrev2 = ST__STNoiseCalculationTool("ITCMSNoiseCalculationToolPrev2")
toolCMSPrev2.DetType = 'IT'
toolCMSPrev2.InputData =  "/Event/Prev2/Raw/IT/LCMSADCs"
toolCMSPrev2.FollowPeriod = 0
toolCMSPrev2.CountRoundRobin = True
toolCMSPrev2.OutputLevel = 3

itCMSMonitorPrev2 = STNZSMonitor("ITCMSMonitorPrev2")
itCMSMonitorPrev2.DetType = "IT"
itCMSMonitorPrev2.UseSourceID = False
itCMSMonitorPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSMonitorPrev2.NoiseToolName = "ITCMSNoiseCalculationToolPrev2"
itCMSMonitorPrev2.OutputLevel = 3
itCMSMonitorPrev2.UseODINTime = True

# Prev1
itCMSNoiseCalcPrev1 = ST__STNoiseCalculation("ITCMSNoiseCalculationPrev1")
itCMSNoiseCalcPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSNoiseCalcPrev1.NoiseToolName = "ITCMSNoiseCalculationToolPrev1"
itCMSNoiseCalcPrev1.OutputLevel = 3;

toolCMSPrev1 = ST__STNoiseCalculationTool("ITCMSNoiseCalculationToolPrev1")
toolCMSPrev1.DetType = 'IT'
toolCMSPrev1.InputData =  "/Event/Prev1/Raw/IT/LCMSADCs"
toolCMSPrev1.FollowPeriod = 0
toolCMSPrev1.CountRoundRobin = True
toolCMSPrev1.OutputLevel = 3

itCMSMonitorPrev1 = STNZSMonitor("ITCMSMonitorPrev1")
itCMSMonitorPrev1.DetType = "IT"
itCMSMonitorPrev1.UseSourceID = False
itCMSMonitorPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSMonitorPrev1.NoiseToolName = "ITCMSNoiseCalculationToolPrev1"
itCMSMonitorPrev1.OutputLevel = 3
itCMSMonitorPrev1.UseODINTime = True

# Next1
itCMSNoiseCalcNext1 = ST__STNoiseCalculation("ITCMSNoiseCalculationNext1")
itCMSNoiseCalcNext1.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSNoiseCalcNext1.NoiseToolName = "ITCMSNoiseCalculationToolNext1"
itCMSNoiseCalcNext1.OutputLevel = 3;

toolCMSNext1 = ST__STNoiseCalculationTool("ITCMSNoiseCalculationToolNext1")
toolCMSNext1.DetType = 'IT'
toolCMSNext1.InputData =  "/Event/Next1/Raw/IT/LCMSADCs"
toolCMSNext1.FollowPeriod = 0
toolCMSNext1.CountRoundRobin = True
toolCMSNext1.OutputLevel = 3

itCMSMonitorNext1 = STNZSMonitor("ITCMSMonitorNext1")
itCMSMonitorNext1.DetType = "IT"
itCMSMonitorNext1.UseSourceID = False
itCMSMonitorNext1.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSMonitorNext1.NoiseToolName = "ITCMSNoiseCalculationToolNext1"
itCMSMonitorNext1.OutputLevel = 3
itCMSMonitorNext1.UseODINTime = True

# Next2
itCMSNoiseCalcNext2 = ST__STNoiseCalculation("ITCMSNoiseCalculationNext2")
itCMSNoiseCalcNext2.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSNoiseCalcNext2.NoiseToolName = "ITCMSNoiseCalculationToolNext2"
itCMSNoiseCalcNext2.OutputLevel = 3;

toolCMSNext2 = ST__STNoiseCalculationTool("ITCMSNoiseCalculationToolNext2")
toolCMSNext2.DetType = 'IT'
toolCMSNext2.InputData =  "/Event/Next2/Raw/IT/LCMSADCs"
toolCMSNext2.FollowPeriod = 0
toolCMSNext2.CountRoundRobin = True
toolCMSNext2.OutputLevel = 3

itCMSMonitorNext2 = STNZSMonitor("ITCMSMonitorNext2")
itCMSMonitorNext2.DetType = "IT"
itCMSMonitorNext2.UseSourceID = False
itCMSMonitorNext2.NoiseToolType = "ST::STNoiseCalculationTool"
itCMSMonitorNext2.NoiseToolName = "ITCMSNoiseCalculationToolNext2"
itCMSMonitorNext2.OutputLevel = 3
itCMSMonitorNext2.UseODINTime = True

#MoniITNZSSeq.Members.append(itCMSNoiseCalcPrev2)
#MoniITNZSSeq.Members.append(itCMSMonitorPrev2)
MoniITNZSSeq.Members.append(itCMSNoiseCalcPrev1)
MoniITNZSSeq.Members.append(itCMSMonitorPrev1)
MoniITNZSSeq.Members.append(itCMSNoiseCalcCentral)
MoniITNZSSeq.Members.append(itCMSMonitorCentral)
MoniITNZSSeq.Members.append(itCMSNoiseCalcNext1)
MoniITNZSSeq.Members.append(itCMSMonitorNext1)
#MoniITNZSSeq.Members.append(itCMSNoiseCalcNext2)
#MoniITNZSSeq.Members.append(itCMSMonitorNext2)

