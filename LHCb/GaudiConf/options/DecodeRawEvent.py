
# Job options to configure the data on demand service for decoding the RawEvent

from Gaudi.Configuration import *
from Configurables import ( DataOnDemandSvc,
                            DecodeVeloRawBuffer,
                            RawBankToSTClusterAlg,
                            DecodeVeloRawBuffer,
                            RawBankToSTLiteClusterAlg,
                            Rich__DAQ__RawBufferToRichDigitsAlg )
                           
# Set up the algorithms
DataOnDemandSvc().AlgMap["Raw/Velo/Clusters"]     = "DecodeVeloRawBuffer/createVeloClusters"
DataOnDemandSvc().AlgMap["Raw/TT/Clusters"]       = "RawBankToSTClusterAlg/createTTClusters"
DataOnDemandSvc().AlgMap["Raw/IT/Clusters"]       = "RawBankToSTClusterAlg/createITClusters"
DataOnDemandSvc().AlgMap["Raw/Velo/LiteClusters"] = "DecodeVeloRawBuffer/createVeloLiteClusters"
DataOnDemandSvc().AlgMap["Raw/TT/LiteClusters"]   = "RawBankToSTLiteClusterAlg/createTTLiteClusters"
DataOnDemandSvc().AlgMap["Raw/IT/LiteClusters"]   = "RawBankToSTLiteClusterAlg/createITLiteClusters"
DataOnDemandSvc().AlgMap["Raw/OT/Times"]          = "OTTimeCreator"
DataOnDemandSvc().AlgMap["Raw/Muon/Coords"]       = "MuonRec"
DataOnDemandSvc().AlgMap["DAQ/ODIN"]              = "createODIN"
DataOnDemandSvc().AlgMap["Raw/Rich/Digits"]       = "Rich::DAQ::RawBufferToRichDigitsAlg/RichRawEventToDigits"

RawBankToSTClusterAlg("createITClusters").DetType          = "IT"
RawBankToSTLiteClusterAlg("createITLiteClusters").DetType  = "IT"
DecodeVeloRawBuffer("createVeloClusters").DecodeToVeloClusters     = True
DecodeVeloRawBuffer("createVeloClusters").DecodeToVeloLiteClusters = False
Rich__DAQ__RawBufferToRichDigitsAlg("RichRawEventToDigits").DecodeBufferOnly = False
importOptions( "$CALODAQROOT/options/CaloDigitsOnDemand.opts" )
importOptions( "$L0DUOPTS/L0OnDemand.opts" )
