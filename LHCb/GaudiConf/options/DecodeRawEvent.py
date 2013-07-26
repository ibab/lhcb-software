
# Job options to configure the data on demand service for decoding the RawEvent
print "# WARNING: You have imported DecodeRawEvent.py, there is now a configurable doing the same job. See savannah task #19106."

from Gaudi.Configuration import *
from Configurables import ( DataOnDemandSvc,
                            DecodeVeloRawBuffer,
			    VLRawBankDecoder,
                            RawBankToSTClusterAlg,
                            DecodeVeloRawBuffer,
                            RawBankToSTLiteClusterAlg,
                            FTRawBankDecoder,
                            Rich__DAQ__RawBufferToRichDigitsAlg,VPRawBankToLiteCluster,VPRawBankToPartialCluster )
# for the upgrade
VPLiteClusters = VPRawBankToLiteCluster("createVPLiteClusters")
VPLiteClusters.ClusterLocation  = "Raw/VP/LiteClusters"
VPClusters     = VPRawBankToPartialCluster("createVPClusters")
VPClusters.ClusterLocation = "Raw/VP/Clusters"
#
VLClusters = VLRawBankDecoder("createVLClusters")
VLClusters.DecodeToClusters     = True
VLLiteClusters = VLRawBankDecoder("createVLLiteClusters")
VLLiteClusters.DecodeToLiteClusters = True
VLLiteClusters.DecodeToClusters     = False
FTClusters =  FTRawBankDecoder("createFTClusters")
# Set up the algorithms
DataOnDemandSvc().AlgMap["Raw/VP/Clusters"]       = VPClusters 
DataOnDemandSvc().AlgMap["Raw/VP/LiteClusters"]   = VPLiteClusters
DataOnDemandSvc().AlgMap["Raw/VL/Clusters"]       = VLClusters 
DataOnDemandSvc().AlgMap["Raw/VL/LiteClusters"]   = VLLiteClusters
DataOnDemandSvc().AlgMap["Raw/Velo/Clusters"]     = "DecodeVeloRawBuffer/createVeloClusters"
DataOnDemandSvc().AlgMap["Raw/TT/Clusters"]       = "RawBankToSTClusterAlg/createTTClusters"
DataOnDemandSvc().AlgMap["Raw/IT/Clusters"]       = "RawBankToSTClusterAlg/createITClusters"
DataOnDemandSvc().AlgMap["Raw/UT/Clusters"]       = "RawBankToSTClusterAlg/createUTClusters"
DataOnDemandSvc().AlgMap["Raw/Velo/LiteClusters"] = "DecodeVeloRawBuffer/createVeloLiteClusters"
DataOnDemandSvc().AlgMap["Raw/TT/LiteClusters"]   = "RawBankToSTLiteClusterAlg/createTTLiteClusters"
DataOnDemandSvc().AlgMap["Raw/IT/LiteClusters"]   = "RawBankToSTLiteClusterAlg/createITLiteClusters"
DataOnDemandSvc().AlgMap["Raw/UT/LiteClusters"]   = "RawBankToSTLiteClusterAlg/createUTLiteClusters"
DataOnDemandSvc().AlgMap["Raw/OT/Times"]          = "OTTimeCreator"
DataOnDemandSvc().AlgMap["Raw/FT/RawClusters"]    = FTClusters
DataOnDemandSvc().AlgMap["Raw/Muon/Coords"]       = "MuonRec"
DataOnDemandSvc().AlgMap["DAQ/ODIN"]              = "createODIN"
DataOnDemandSvc().AlgMap["Raw/Rich/Digits"]       = "Rich::DAQ::RawBufferToRichDigitsAlg/RichRawEventToDigits"

RawBankToSTClusterAlg("createITClusters").DetType          = "IT"
RawBankToSTLiteClusterAlg("createITLiteClusters").DetType  = "IT"

RawBankToSTClusterAlg("createUTClusters").DetType          = "UT"
RawBankToSTLiteClusterAlg("createUTLiteClusters").DetType  = "UT"

#Moved to ST/STTools/python/STTools/STOfflineConf, as with TT and IT
#from Configurables import STOfflinePosition
#UT = STOfflinePosition('ToolSvc.UTClusterPosition')
#UT.DetType = "UT"

DecodeVeloRawBuffer("createVeloClusters").DecodeToVeloClusters     = True
DecodeVeloRawBuffer("createVeloClusters").DecodeToVeloLiteClusters = False
DecodeVeloRawBuffer("createVeloLiteClusters").DecodeToVeloClusters     = False
DecodeVeloRawBuffer("createVeloLiteClusters").DecodeToVeloLiteClusters = True

Rich__DAQ__RawBufferToRichDigitsAlg("RichRawEventToDigits").DecodeBufferOnly = False
importOptions( "$CALODAQROOT/options/CaloDigitsOnDemand.opts" )
importOptions( "$L0DUOPTS/L0OnDemand.opts" )
