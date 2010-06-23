from Configurables import ( DecodeVeloRawBuffer, RawBankToSTLiteClusterAlg,
		RawBankToSTClusterAlg, DataOnDemandSvc )

DecodeVeloRawBuffer().DecodeToVeloLiteClusters = True
DecodeVeloRawBuffer().DecodeToVeloClusters = False
DecodeVeloRawBuffer("DecodeVeloClusters").DecodeToVeloLiteClusters = False
DecodeVeloRawBuffer("DecodeVeloClusters").DecodeToVeloClusters = True
RawBankToSTClusterAlg("CreateITClusters").DetType = "IT"
RawBankToSTLiteClusterAlg("CreateITLiteClusters").DetType = "IT"

DataOnDemandSvc().AlgMap["/Event/Raw/Velo/LiteClusters"] = DecodeVeloRawBuffer()
DataOnDemandSvc().AlgMap["/Event/Raw/Velo/Clusters"] = DecodeVeloRawBuffer("DecodeVeloClusters")
DataOnDemandSvc().AlgMap["/Event/Raw/TT/LiteClusters"] = RawBankToSTLiteClusterAlg("CreateTTLiteClusters")
DataOnDemandSvc().AlgMap["/Event/Raw/TT/Clusters"] = RawBankToSTClusterAlg("CreateTTClusters")
DataOnDemandSvc().AlgMap["/Event/Raw/IT/LiteClusters"] = RawBankToSTLiteClusterAlg("CreateITLiteClusters")
DataOnDemandSvc().AlgMap["/Event/Raw/IT/Clusters"] = RawBankToSTClusterAlg("CreateITClusters")
