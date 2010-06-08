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
ttNoiseCalcCentral.NoiseToolType = "ST::STNoiseCalculationTool"
ttNoiseCalcCentral.NoiseToolName = "TTNoiseCalculationToolCentral"
ttNoiseCalcCentral.OutputLevel = 3;

toolCentral = ST__STNoiseCalculationTool("TTNoiseCalculationToolCentral")
toolCentral.InputData =  "/Event/Raw/TT/Full"
toolCentral.FollowPeriod = 0
toolCentral.CountRoundRobin = True
toolCentral.OutputLevel = 3

ttNZSMonitorCentral = STNZSMonitor("TTNZSMonitorCentral")
ttNZSMonitorCentral.DetType = "TT"
ttNZSMonitorCentral.UseSourceID = False
ttNZSMonitorCentral.NoiseToolType = "ST::STNoiseCalculationTool"
ttNZSMonitorCentral.NoiseToolName = "TTNoiseCalculationToolCentral"
ttNZSMonitorCentral.OutputLevel = 3
ttNZSMonitorCentral.UseODINTime = True

# Prev2
ttNoiseCalcPrev2 = ST__STNoiseCalculation("TTNoiseCalculationPrev2")
ttNoiseCalcPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
ttNoiseCalcPrev2.NoiseToolName = "TTNoiseCalculationToolPrev2"
ttNoiseCalcPrev2.OutputLevel = 3;

toolPrev2 = ST__STNoiseCalculationTool("TTNoiseCalculationToolPrev2")
toolPrev2.InputData =  "/Event/Prev2/Raw/TT/Full"
toolPrev2.FollowPeriod = 0
toolPrev2.CountRoundRobin = True
toolPrev2.OutputLevel = 3

ttNZSMonitorPrev2 = STNZSMonitor("TTNZSMonitorPrev2")
ttNZSMonitorPrev2.DetType = "TT"
ttNZSMonitorPrev2.UseSourceID = False
ttNZSMonitorPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
ttNZSMonitorPrev2.NoiseToolName = "TTNoiseCalculationToolPrev2"
ttNZSMonitorPrev2.OutputLevel = 3
ttNZSMonitorPrev2.UseODINTime = True

# Prev1
ttNoiseCalcPrev1 = ST__STNoiseCalculation("TTNoiseCalculationPrev1")
ttNoiseCalcPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
ttNoiseCalcPrev1.NoiseToolName = "TTNoiseCalculationToolPrev1"
ttNoiseCalcPrev1.OutputLevel = 3;

toolPrev1 = ST__STNoiseCalculationTool("TTNoiseCalculationToolPrev1")
toolPrev1.InputData =  "/Event/Prev1/Raw/TT/Full"
toolPrev1.FollowPeriod = 0
toolPrev1.CountRoundRobin = True
toolPrev1.OutputLevel = 3

ttNZSMonitorPrev1 = STNZSMonitor("TTNZSMonitorPrev1")
ttNZSMonitorPrev1.DetType = "TT"
ttNZSMonitorPrev1.UseSourceID = False
ttNZSMonitorPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
ttNZSMonitorPrev1.NoiseToolName = "TTNoiseCalculationToolPrev1"
ttNZSMonitorPrev1.OutputLevel = 3
ttNZSMonitorPrev1.UseODINTime = True

# Next1
ttNoiseCalcNext1 = ST__STNoiseCalculation("TTNoiseCalculationNext1")
ttNoiseCalcNext1.NoiseToolType = "ST::STNoiseCalculationTool"
ttNoiseCalcNext1.NoiseToolName = "TTNoiseCalculationToolNext1"
ttNoiseCalcNext1.OutputLevel = 3;

toolNext1 = ST__STNoiseCalculationTool("TTNoiseCalculationToolNext1")
toolNext1.InputData =  "/Event/Next1/Raw/TT/Full"
toolNext1.FollowPeriod = 0
toolNext1.CountRoundRobin = True
toolNext1.OutputLevel = 3

ttNZSMonitorNext1 = STNZSMonitor("TTNZSMonitorNext1")
ttNZSMonitorNext1.DetType = "TT"
ttNZSMonitorNext1.UseSourceID = False
ttNZSMonitorNext1.NoiseToolType = "ST::STNoiseCalculationTool"
ttNZSMonitorNext1.NoiseToolName = "TTNoiseCalculationToolNext1"
ttNZSMonitorNext1.OutputLevel = 3
ttNZSMonitorNext1.UseODINTime = True

# Next2
ttNoiseCalcNext2 = ST__STNoiseCalculation("TTNoiseCalculationNext2")
ttNoiseCalcNext2.NoiseToolType = "ST::STNoiseCalculationTool"
ttNoiseCalcNext2.NoiseToolName = "TTNoiseCalculationToolNext2"
ttNoiseCalcNext2.OutputLevel = 3;

toolNext2 = ST__STNoiseCalculationTool("TTNoiseCalculationToolNext2")
toolNext2.InputData =  "/Event/Next2/Raw/TT/Full"
toolNext2.FollowPeriod = 0
toolNext2.CountRoundRobin = True
toolNext2.OutputLevel = 3

ttNZSMonitorNext2 = STNZSMonitor("TTNZSMonitorNext2")
ttNZSMonitorNext2.DetType = "TT"
ttNZSMonitorNext2.UseSourceID = False
ttNZSMonitorNext2.NoiseToolType = "ST::STNoiseCalculationTool"
ttNZSMonitorNext2.NoiseToolName = "TTNoiseCalculationToolNext2"
ttNZSMonitorNext2.OutputLevel = 3
ttNZSMonitorNext2.UseODINTime = True

#MoniTTNZSSeq.Members.append(ttNoiseCalcPrev2)
#MoniTTNZSSeq.Members.append(ttNZSMonitorPrev2)
MoniTTNZSSeq.Members.append(ttNoiseCalcPrev1)
MoniTTNZSSeq.Members.append(ttNZSMonitorPrev1)
MoniTTNZSSeq.Members.append(ttNoiseCalcCentral)
MoniTTNZSSeq.Members.append(ttNZSMonitorCentral)
MoniTTNZSSeq.Members.append(ttNoiseCalcNext1)
MoniTTNZSSeq.Members.append(ttNZSMonitorNext1)
#MoniTTNZSSeq.Members.append(ttNoiseCalcNext2)
#MoniTTNZSSeq.Members.append(ttNZSMonitorNext2)


#==============================================================================
# CMS noise calculation (Use LCMS ADCs from the emulator)
#==============================================================================
ttCMSNoiseCalcCentral = ST__STNoiseCalculation("TTCMSNoiseCalculationCentral")
ttCMSNoiseCalcCentral.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSNoiseCalcCentral.NoiseToolName = "TTCMSNoiseCalculationToolCentral"
ttCMSNoiseCalcCentral.OutputLevel = 3;

toolCMSCentral = ST__STNoiseCalculationTool("TTCMSNoiseCalculationToolCentral")
toolCMSCentral.InputData =  "/Event/Raw/TT/LCMSADCs"
toolCMSCentral.FollowPeriod = 0
toolCMSCentral.CountRoundRobin = True
toolCMSCentral.OutputLevel = 3

ttCMSMonitorCentral = STNZSMonitor("TTCMSMonitorCentral")
ttCMSMonitorCentral.DetType = "TT"
ttCMSMonitorCentral.UseSourceID = False
ttCMSMonitorCentral.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSMonitorCentral.NoiseToolName = "TTCMSNoiseCalculationToolCentral"
ttCMSMonitorCentral.OutputLevel = 3
ttCMSMonitorCentral.UseODINTime = True

# Prev2
ttCMSNoiseCalcPrev2 = ST__STNoiseCalculation("TTCMSNoiseCalculationPrev2")
ttCMSNoiseCalcPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSNoiseCalcPrev2.NoiseToolName = "TTCMSNoiseCalculationToolPrev2"
ttCMSNoiseCalcPrev2.OutputLevel = 3;

toolCMSPrev2 = ST__STNoiseCalculationTool("TTCMSNoiseCalculationToolPrev2")
toolCMSPrev2.InputData =  "/Event/Prev2/Raw/TT/LCMSADCs"
toolCMSPrev2.FollowPeriod = 0
toolCMSPrev2.CountRoundRobin = True
toolCMSPrev2.OutputLevel = 3

ttCMSMonitorPrev2 = STNZSMonitor("TTCMSMonitorPrev2")
ttCMSMonitorPrev2.DetType = "TT"
ttCMSMonitorPrev2.UseSourceID = False
ttCMSMonitorPrev2.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSMonitorPrev2.NoiseToolName = "TTCMSNoiseCalculationToolPrev2"
ttCMSMonitorPrev2.OutputLevel = 3
ttCMSMonitorPrev2.UseODINTime = True

# Prev1
ttCMSNoiseCalcPrev1 = ST__STNoiseCalculation("TTCMSNoiseCalculationPrev1")
ttCMSNoiseCalcPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSNoiseCalcPrev1.NoiseToolName = "TTCMSNoiseCalculationToolPrev1"
ttCMSNoiseCalcPrev1.OutputLevel = 3;

toolCMSPrev1 = ST__STNoiseCalculationTool("TTCMSNoiseCalculationToolPrev1")
toolCMSPrev1.InputData =  "/Event/Prev1/Raw/TT/LCMSADCs"
toolCMSPrev1.FollowPeriod = 0
toolCMSPrev1.CountRoundRobin = True
toolCMSPrev1.OutputLevel = 3

ttCMSMonitorPrev1 = STNZSMonitor("TTCMSMonitorPrev1")
ttCMSMonitorPrev1.DetType = "TT"
ttCMSMonitorPrev1.UseSourceID = False
ttCMSMonitorPrev1.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSMonitorPrev1.NoiseToolName = "TTCMSNoiseCalculationToolPrev1"
ttCMSMonitorPrev1.OutputLevel = 3
ttCMSMonitorPrev1.UseODINTime = True

# Next1
ttCMSNoiseCalcNext1 = ST__STNoiseCalculation("TTCMSNoiseCalculationNext1")
ttCMSNoiseCalcNext1.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSNoiseCalcNext1.NoiseToolName = "TTCMSNoiseCalculationToolNext1"
ttCMSNoiseCalcNext1.OutputLevel = 3;

toolCMSNext1 = ST__STNoiseCalculationTool("TTCMSNoiseCalculationToolNext1")
toolCMSNext1.InputData =  "/Event/Next1/Raw/TT/LCMSADCs"
toolCMSNext1.FollowPeriod = 0
toolCMSNext1.CountRoundRobin = True
toolCMSNext1.OutputLevel = 3

ttCMSMonitorNext1 = STNZSMonitor("TTCMSMonitorNext1")
ttCMSMonitorNext1.DetType = "TT"
ttCMSMonitorNext1.UseSourceID = False
ttCMSMonitorNext1.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSMonitorNext1.NoiseToolName = "TTCMSNoiseCalculationToolNext1"
ttCMSMonitorNext1.OutputLevel = 3
ttCMSMonitorNext1.UseODINTime = True

# Next2
ttCMSNoiseCalcNext2 = ST__STNoiseCalculation("TTCMSNoiseCalculationNext2")
ttCMSNoiseCalcNext2.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSNoiseCalcNext2.NoiseToolName = "TTCMSNoiseCalculationToolNext2"
ttCMSNoiseCalcNext2.OutputLevel = 3;

toolCMSNext2 = ST__STNoiseCalculationTool("TTCMSNoiseCalculationToolNext2")
toolCMSNext2.InputData =  "/Event/Next2/Raw/TT/LCMSADCs"
toolCMSNext2.FollowPeriod = 0
toolCMSNext2.CountRoundRobin = True
toolCMSNext2.OutputLevel = 3

ttCMSMonitorNext2 = STNZSMonitor("TTCMSMonitorNext2")
ttCMSMonitorNext2.DetType = "TT"
ttCMSMonitorNext2.UseSourceID = False
ttCMSMonitorNext2.NoiseToolType = "ST::STNoiseCalculationTool"
ttCMSMonitorNext2.NoiseToolName = "TTCMSNoiseCalculationToolNext2"
ttCMSMonitorNext2.OutputLevel = 3
ttCMSMonitorNext2.UseODINTime = True

#MoniTTNZSSeq.Members.append(ttCMSNoiseCalcPrev2)
#MoniTTNZSSeq.Members.append(ttCMSMonitorPrev2)
MoniTTNZSSeq.Members.append(ttCMSNoiseCalcPrev1)
MoniTTNZSSeq.Members.append(ttCMSMonitorPrev1)
MoniTTNZSSeq.Members.append(ttCMSNoiseCalcCentral)
MoniTTNZSSeq.Members.append(ttCMSMonitorCentral)
MoniTTNZSSeq.Members.append(ttCMSNoiseCalcNext1)
MoniTTNZSSeq.Members.append(ttCMSMonitorNext1)
#MoniTTNZSSeq.Members.append(ttCMSNoiseCalcNext2)
#MoniTTNZSSeq.Members.append(ttCMSMonitorNext2)

