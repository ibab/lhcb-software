
# Job options to configure the data on demand service for decoding the RawEvent

from Configurables import ( DataOnDemandSvc,
                            DecodeVeloRawBuffer,
                            RawBankToSTClusterAlg,
                            DecodeVeloRawBuffer,
                            RawBankToSTLiteClusterAlg,
                            Rich__DAQ__RawBufferToRichDigitsAlg )
                           
# Set up the algorithms
DataOnDemandSvc().AlgMap += [
   "Raw/Velo/Clusters"     : "DecodeVeloRawBuffer/createVeloClusters"
  ,"Raw/TT/Clusters"       : "RawBankToSTClusterAlg/createTTClusters"
  ,"Raw/IT/Clusters"       : "RawBankToSTClusterAlg/createITClusters"
  ,"Raw/Velo/LiteClusters" : "DecodeVeloRawBuffer/createVeloLiteClusters"
  ,"Raw/TT/LiteClusters"   : "RawBankToSTLiteClusterAlg/createTTLiteClusters"
  ,"Raw/IT/LiteClusters"   : "RawBankToSTLiteClusterAlg/createITLiteClusters"
  ,"Raw/OT/Times"          : "OTTimeCreator"
  ,"Raw/Muon/Coords"       : "MuonRec"
  ,"DAQ/ODIN"              : "createODIN"
  ,"Raw/Rich/Digits"       : "Rich::DAQ::RawBufferToRichDigitsAlg/RichRawEventToDigits"
]

RawBankToSTClusterAlg("createITCluster").detType           = "IT"
RawBankToSTLiteClusterAlg("createITLiteClusters").detType  = "IT"
DecodeVeloRawBuffer("createVeloLiteClusters").DecodeToVeloClusters     = True
DecodeVeloRawBuffer("createVeloLiteClusters").DecodeToVeloLiteClusters = False
Rich__DAQ__RawBufferToRichDigitsAlg("RichRawEventToDigits").DecodeBufferOnly = False

importOptions( "$CALODAQROOT/options/CaloDigitsOnDemand.opts" )
importOptions( "$L0DUOPTS/L0OnDemand.opts" )

