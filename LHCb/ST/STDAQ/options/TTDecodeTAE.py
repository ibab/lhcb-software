from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

from Configurables import ( RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg )
        
ttClusterCreatorPrev7 = RawBankToSTClusterAlg( 'CreateTTClustersPrev7')
ttClusterCreatorPrev7.DetType = "TT"
ttClusterCreatorPrev7.RawEventLocations = [ '/Event/Prev7/DAQ/RawEvent' ]
ttClusterCreatorPrev7.clusterLocation  = 'Prev7/Raw/TT/Clusters'
ttClusterCreatorPrev7.summaryLocation  = 'Prev7/Rec/TT/Summary'
ttClusterCreatorPrev7.checkValidity    = False
ttClusterCreatorPrev7.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorPrev6 = RawBankToSTClusterAlg( 'CreateTTClustersPrev6')
ttClusterCreatorPrev6.DetType = "TT"
ttClusterCreatorPrev6.RawEventLocations = [ '/Event/Prev6/DAQ/RawEvent' ]
ttClusterCreatorPrev6.clusterLocation  = 'Prev6/Raw/TT/Clusters'
ttClusterCreatorPrev6.summaryLocation  = 'Prev6/Rec/TT/Summary'
ttClusterCreatorPrev6.checkValidity    = False
ttClusterCreatorPrev6.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorPrev5 = RawBankToSTClusterAlg( 'CreateTTClustersPrev5')
ttClusterCreatorPrev5.DetType = "TT"
ttClusterCreatorPrev5.RawEventLocations = [ '/Event/Prev5/DAQ/RawEvent' ]
ttClusterCreatorPrev5.clusterLocation  = 'Prev5/Raw/TT/Clusters'
ttClusterCreatorPrev5.summaryLocation  = 'Prev5/Rec/TT/Summary'
ttClusterCreatorPrev5.checkValidity    = False
ttClusterCreatorPrev5.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorPrev4 = RawBankToSTClusterAlg( 'CreateTTClustersPrev4')
ttClusterCreatorPrev4.DetType = "TT"
ttClusterCreatorPrev4.RawEventLocations = [ '/Event/Prev4/DAQ/RawEvent' ]
ttClusterCreatorPrev4.clusterLocation  = 'Prev4/Raw/TT/Clusters'
ttClusterCreatorPrev4.summaryLocation  = 'Prev4/Rec/TT/Summary'
ttClusterCreatorPrev4.checkValidity    = False
ttClusterCreatorPrev4.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorPrev3 = RawBankToSTClusterAlg( 'CreateTTClustersPrev3')
ttClusterCreatorPrev3.DetType = "TT"
ttClusterCreatorPrev3.RawEventLocations = [ '/Event/Prev3/DAQ/RawEvent' ]
ttClusterCreatorPrev3.clusterLocation  = 'Prev3/Raw/TT/Clusters'
ttClusterCreatorPrev3.summaryLocation  = 'Prev3/Rec/TT/Summary'
ttClusterCreatorPrev3.checkValidity    = False
ttClusterCreatorPrev3.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorPrev2 = RawBankToSTClusterAlg( 'CreateTTClustersPrev2')
ttClusterCreatorPrev2.DetType = "TT"
ttClusterCreatorPrev2.RawEventLocations = [ '/Event/Prev2/DAQ/RawEvent' ]
ttClusterCreatorPrev2.clusterLocation  = 'Prev2/Raw/TT/Clusters'
ttClusterCreatorPrev2.summaryLocation  = 'Prev2/Rec/TT/Summary'
ttClusterCreatorPrev2.checkValidity    = False
ttClusterCreatorPrev2.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorPrev1 = RawBankToSTClusterAlg( 'CreateTTClustersPrev1')
ttClusterCreatorPrev1.DetType = "TT"
ttClusterCreatorPrev1.RawEventLocations = [ '/Event/Prev1/DAQ/RawEvent' ]
ttClusterCreatorPrev1.clusterLocation  = 'Prev1/Raw/TT/Clusters'
ttClusterCreatorPrev1.summaryLocation  = 'Prev1/Rec/TT/Summary'
ttClusterCreatorPrev1.checkValidity    = False
ttClusterCreatorPrev1.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreator = RawBankToSTClusterAlg( 'CreateTTClustersDefault')
ttClusterCreator.DetType = "TT"
ttClusterCreator.RawEventLocations = [ '/Event/DAQ/RawEvent' ]
ttClusterCreator.clusterLocation  = 'Raw/TT/Clusters'
ttClusterCreator.summaryLocation  = 'Rec/TT/Summary'
ttClusterCreator.checkValidity    = False
ttClusterCreator.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext1 = RawBankToSTClusterAlg( 'CreateTTClustersNext1')
ttClusterCreatorNext1.DetType = "TT"
ttClusterCreatorNext1.RawEventLocations = [ '/Event/Next1/DAQ/RawEvent' ]
ttClusterCreatorNext1.clusterLocation  = 'Next1/Raw/TT/Clusters'
ttClusterCreatorNext1.summaryLocation  = 'Next1/Rec/TT/Summary'
ttClusterCreatorNext1.checkValidity    = False
ttClusterCreatorNext1.recoverMode      = True
#itClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext2 = RawBankToSTClusterAlg( 'CreateTTClustersNext2')
ttClusterCreatorNext2.DetType = "TT"
ttClusterCreatorNext2.RawEventLocations = [ '/Event/Next2/DAQ/RawEvent' ]
ttClusterCreatorNext2.clusterLocation  = 'Next2/Raw/TT/Clusters'
ttClusterCreatorNext2.summaryLocation  = 'Next2/Rec/TT/Summary'
ttClusterCreatorNext2.checkValidity    = False
ttClusterCreatorNext2.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext3 = RawBankToSTClusterAlg( 'CreateTTClustersNext3')
ttClusterCreatorNext3.DetType = "TT"
ttClusterCreatorNext3.RawEventLocations = [ '/Event/Next3/DAQ/RawEvent' ]
ttClusterCreatorNext3.clusterLocation  = 'Next3/Raw/TT/Clusters'
ttClusterCreatorNext3.summaryLocation  = 'Next3/Rec/TT/Summary'
ttClusterCreatorNext3.checkValidity    = False
ttClusterCreatorNext3.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext4 = RawBankToSTClusterAlg( 'CreateTTClustersNext4')
ttClusterCreatorNext4.DetType = "TT"
ttClusterCreatorNext4.RawEventLocations = [ '/Event/Next4/DAQ/RawEvent' ]
ttClusterCreatorNext4.clusterLocation  = 'Next4/Raw/TT/Clusters'
ttClusterCreatorNext4.summaryLocation  = 'Next4/Rec/TT/Summary'
ttClusterCreatorNext4.checkValidity    = False
ttClusterCreatorNext4.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext5 = RawBankToSTClusterAlg( 'CreateTTClustersNext5')
ttClusterCreatorNext5.DetType = "TT"
ttClusterCreatorNext5.RawEventLocations = [ '/Event/Next5/DAQ/RawEvent' ]
ttClusterCreatorNext5.clusterLocation  = 'Next5/Raw/TT/Clusters'
ttClusterCreatorNext5.summaryLocation  = 'Next5/Rec/TT/Summary'
ttClusterCreatorNext5.checkValidity    = False
ttClusterCreatorNext5.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext6 = RawBankToSTClusterAlg( 'CreateTTClustersNext6')
ttClusterCreatorNext6.DetType = "TT"
ttClusterCreatorNext6.RawEventLocations = [ '/Event/Next6/DAQ/RawEvent' ]
ttClusterCreatorNext6.clusterLocation  = 'Next6/Raw/TT/Clusters'
ttClusterCreatorNext6.summaryLocation  = 'Next6/Rec/TT/Summary'
ttClusterCreatorNext6.checkValidity    = False
ttClusterCreatorNext6.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

ttClusterCreatorNext7 = RawBankToSTClusterAlg( 'CreateTTClustersNext7')
ttClusterCreatorNext7.DetType = "TT"
ttClusterCreatorNext7.RawEventLocations = [ '/Event/Next7/DAQ/RawEvent' ]
ttClusterCreatorNext7.clusterLocation  = 'Next7/Raw/TT/Clusters'
ttClusterCreatorNext7.summaryLocation  = 'Next7/Rec/TT/Summary'
ttClusterCreatorNext7.checkValidity    = False
ttClusterCreatorNext7.recoverMode      = True
#ttClusterCreator.OutputLevel      = DEBUG

decodeSeq = GaudiSequencer( 'DecodeTTSeq' )
decodeSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( decodeSeq )
decodeSeq.Members.append(ttClusterCreatorPrev7)
decodeSeq.Members.append(ttClusterCreatorPrev6)
decodeSeq.Members.append(ttClusterCreatorPrev5)
decodeSeq.Members.append(ttClusterCreatorPrev4)
decodeSeq.Members.append(ttClusterCreatorPrev3)
decodeSeq.Members.append(ttClusterCreatorPrev2)
decodeSeq.Members.append(ttClusterCreatorPrev1)
decodeSeq.Members.append(ttClusterCreator)
decodeSeq.Members.append(ttClusterCreatorNext1)
decodeSeq.Members.append(ttClusterCreatorNext2)
decodeSeq.Members.append(ttClusterCreatorNext3)
decodeSeq.Members.append(ttClusterCreatorNext4)
decodeSeq.Members.append(ttClusterCreatorNext5)
decodeSeq.Members.append(ttClusterCreatorNext6)
decodeSeq.Members.append(ttClusterCreatorNext7)

