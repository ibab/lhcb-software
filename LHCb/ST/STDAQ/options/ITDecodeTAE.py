from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

from Configurables import ( RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg )
        
itClusterCreatorPrev7 = RawBankToSTClusterAlg( 'CreateITClustersPrev7')
itClusterCreatorPrev7.DetType = "IT"
itClusterCreatorPrev7.RawEventLocations = [ '/Event/Prev7/DAQ/RawEvent' ]
itClusterCreatorPrev7.clusterLocation  = 'Prev7/Raw/IT/Clusters'
itClusterCreatorPrev7.summaryLocation  = 'Prev7/Rec/IT/Summary'
itClusterCreatorPrev7.checkValidity    = False
itClusterCreatorPrev7.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorPrev6 = RawBankToSTClusterAlg( 'CreateITClustersPrev6')
itClusterCreatorPrev6.DetType = "IT"
itClusterCreatorPrev6.RawEventLocations = [ '/Event/Prev6/DAQ/RawEvent' ]
itClusterCreatorPrev6.clusterLocation  = 'Prev6/Raw/IT/Clusters'
itClusterCreatorPrev6.summaryLocation  = 'Prev6/Rec/IT/Summary'
itClusterCreatorPrev6.checkValidity    = False
itClusterCreatorPrev6.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorPrev5 = RawBankToSTClusterAlg( 'CreateITClustersPrev5')
itClusterCreatorPrev5.DetType = "IT"
itClusterCreatorPrev5.RawEventLocations = [ '/Event/Prev5/DAQ/RawEvent' ]
itClusterCreatorPrev5.clusterLocation  = 'Prev5/Raw/IT/Clusters'
itClusterCreatorPrev5.summaryLocation  = 'Prev5/Rec/IT/Summary'
itClusterCreatorPrev5.checkValidity    = False
itClusterCreatorPrev5.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorPrev4 = RawBankToSTClusterAlg( 'CreateITClustersPrev4')
itClusterCreatorPrev4.DetType = "IT"
itClusterCreatorPrev4.RawEventLocations = [ '/Event/Prev4/DAQ/RawEvent' ]
itClusterCreatorPrev4.clusterLocation  = 'Prev4/Raw/IT/Clusters'
itClusterCreatorPrev4.summaryLocation  = 'Prev4/Rec/IT/Summary'
itClusterCreatorPrev4.checkValidity    = False
itClusterCreatorPrev4.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorPrev3 = RawBankToSTClusterAlg( 'CreateITClustersPrev3')
itClusterCreatorPrev3.DetType = "IT"
itClusterCreatorPrev3.RawEventLocations = [ '/Event/Prev3/DAQ/RawEvent' ]
itClusterCreatorPrev3.clusterLocation  = 'Prev3/Raw/IT/Clusters'
itClusterCreatorPrev3.summaryLocation  = 'Prev3/Rec/IT/Summary'
itClusterCreatorPrev3.checkValidity    = False
itClusterCreatorPrev3.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorPrev2 = RawBankToSTClusterAlg( 'CreateITClustersPrev2')
itClusterCreatorPrev2.DetType = "IT"
itClusterCreatorPrev2.RawEventLocations = [ '/Event/Prev2/DAQ/RawEvent' ]
itClusterCreatorPrev2.clusterLocation  = 'Prev2/Raw/IT/Clusters'
itClusterCreatorPrev2.summaryLocation  = 'Prev2/Rec/IT/Summary'
itClusterCreatorPrev2.checkValidity    = False
itClusterCreatorPrev2.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorPrev1 = RawBankToSTClusterAlg( 'CreateITClustersPrev1')
itClusterCreatorPrev1.DetType = "IT"
itClusterCreatorPrev1.RawEventLocations = [ '/Event/Prev1/DAQ/RawEvent' ]
itClusterCreatorPrev1.clusterLocation  = 'Prev1/Raw/IT/Clusters'
itClusterCreatorPrev1.summaryLocation  = 'Prev1/Rec/IT/Summary'
itClusterCreatorPrev1.checkValidity    = False
itClusterCreatorPrev1.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreator = RawBankToSTClusterAlg( 'CreateITClustersCentral')
itClusterCreator.DetType = "IT"
#itClusterCreator.RawEventLocations = [ '/Event/DAQ/RawEvent' ]
#itClusterCreator.clusterLocation  = 'Raw/IT/Clusters'
#itClusterCreator.summaryLocation  = 'Rec/IT/Summary'
itClusterCreator.checkValidity    = False
itClusterCreator.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext1 = RawBankToSTClusterAlg( 'CreateITClustersNext1')
itClusterCreatorNext1.DetType = "IT"
itClusterCreatorNext1.RawEventLocations = [ '/Event/Next1/DAQ/RawEvent' ]
itClusterCreatorNext1.clusterLocation  = 'Next1/Raw/IT/Clusters'
itClusterCreatorNext1.summaryLocation  = 'Next1/Rec/IT/Summary'
itClusterCreatorNext1.checkValidity    = False
itClusterCreatorNext1.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext2 = RawBankToSTClusterAlg( 'CreateITClustersNext2')
itClusterCreatorNext2.DetType = "IT"
itClusterCreatorNext2.RawEventLocations = [ '/Event/Next2/DAQ/RawEvent' ]
itClusterCreatorNext2.clusterLocation  = 'Next2/Raw/IT/Clusters'
itClusterCreatorNext2.summaryLocation  = 'Next2/Rec/IT/Summary'
itClusterCreatorNext2.checkValidity    = False
itClusterCreatorNext2.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext3 = RawBankToSTClusterAlg( 'CreateITClustersNext3')
itClusterCreatorNext3.DetType = "IT"
itClusterCreatorNext3.RawEventLocations = [ '/Event/Next3/DAQ/RawEvent' ]
itClusterCreatorNext3.clusterLocation  = 'Next3/Raw/IT/Clusters'
itClusterCreatorNext3.summaryLocation  = 'Next3/Rec/IT/Summary'
itClusterCreatorNext3.checkValidity    = False
itClusterCreatorNext3.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext4 = RawBankToSTClusterAlg( 'CreateITClustersNext4')
itClusterCreatorNext4.DetType = "IT"
itClusterCreatorNext4.RawEventLocations = [ '/Event/Next4/DAQ/RawEvent' ]
itClusterCreatorNext4.clusterLocation  = 'Next4/Raw/IT/Clusters'
itClusterCreatorNext4.summaryLocation  = 'Next4/Rec/IT/Summary'
itClusterCreatorNext4.checkValidity    = False
itClusterCreatorNext4.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext5 = RawBankToSTClusterAlg( 'CreateITClustersNext5')
itClusterCreatorNext5.DetType = "IT"
itClusterCreatorNext5.RawEventLocations = [ '/Event/Next5/DAQ/RawEvent' ]
itClusterCreatorNext5.clusterLocation  = 'Next5/Raw/IT/Clusters'
itClusterCreatorNext5.summaryLocation  = 'Next5/Rec/IT/Summary'
itClusterCreatorNext5.checkValidity    = False
itClusterCreatorNext5.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext6 = RawBankToSTClusterAlg( 'CreateITClustersNext6')
itClusterCreatorNext6.DetType = "IT"
itClusterCreatorNext6.RawEventLocations = [ '/Event/Next6/DAQ/RawEvent' ]
itClusterCreatorNext6.clusterLocation  = 'Next6/Raw/IT/Clusters'
itClusterCreatorNext6.summaryLocation  = 'Next6/Rec/IT/Summary'
itClusterCreatorNext6.checkValidity    = False
itClusterCreatorNext6.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

itClusterCreatorNext7 = RawBankToSTClusterAlg( 'CreateITClustersNext7')
itClusterCreatorNext7.DetType = "IT"
itClusterCreatorNext7.RawEventLocations = [ '/Event/Next7/DAQ/RawEvent' ]
itClusterCreatorNext7.clusterLocation  = 'Next7/Raw/IT/Clusters'
itClusterCreatorNext7.summaryLocation  = 'Next7/Rec/IT/Summary'
itClusterCreatorNext7.checkValidity    = False
itClusterCreatorNext7.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

decodeSeq = GaudiSequencer( 'DecodeITSeq' )
decodeSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( decodeSeq )
decodeSeq.Members.append(itClusterCreatorPrev7)
decodeSeq.Members.append(itClusterCreatorPrev6)
decodeSeq.Members.append(itClusterCreatorPrev5)
decodeSeq.Members.append(itClusterCreatorPrev4)
decodeSeq.Members.append(itClusterCreatorPrev3)
decodeSeq.Members.append(itClusterCreatorPrev2)
decodeSeq.Members.append(itClusterCreatorPrev1)
decodeSeq.Members.append(itClusterCreator)
decodeSeq.Members.append(itClusterCreatorNext1)
decodeSeq.Members.append(itClusterCreatorNext2)
decodeSeq.Members.append(itClusterCreatorNext3)
decodeSeq.Members.append(itClusterCreatorNext4)
decodeSeq.Members.append(itClusterCreatorNext5)
decodeSeq.Members.append(itClusterCreatorNext6)
decodeSeq.Members.append(itClusterCreatorNext7)

