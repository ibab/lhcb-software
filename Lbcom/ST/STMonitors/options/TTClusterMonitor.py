from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

MoniTTSeq = GaudiSequencer( 'MoniTTSeq' )
MoniTTSeq.MeasureTime = True
#ApplicationMgr().TopAlg.append( MoniTTSeq )

from Configurables import ( ST__STClusterMonitor )
        
ttClusterMonitorCentral = ST__STClusterMonitor("TTClusterMonitorCentral")
ttClusterMonitorCentral.DetType = "TT"
ttClusterMonitorCentral.ClusterLocation = "Raw/TT/Clusters"
ttClusterMonitorCentral.HitMaps = False
ttClusterMonitorCentral.ByDetectorRegion = False
ttClusterMonitorCentral.ByServiceBox = True
ttClusterMonitorCentral.ByPort = True
ttClusterMonitorCentral.FullDetail = False
ttClusterMonitorCentral.OutputLevel = 3

ttClusterMonitorPrev7 = ST__STClusterMonitor("TTClusterMonitorPrev7")
ttClusterMonitorPrev7.DetType = "TT"
ttClusterMonitorPrev7.ClusterLocation = "Prev7/Raw/TT/Clusters"
ttClusterMonitorPrev7.HitMaps = False
ttClusterMonitorPrev7.ByDetectorRegion = False
ttClusterMonitorPrev7.ByServiceBox = True
ttClusterMonitorPrev7.ByPort = True
ttClusterMonitorPrev7.FullDetail = False
ttClusterMonitorPrev7.OutputLevel = 3

ttClusterMonitorPrev6 = ST__STClusterMonitor("TTClusterMonitorPrev6")
ttClusterMonitorPrev6.DetType = "TT"
ttClusterMonitorPrev6.ClusterLocation = "Prev6/Raw/TT/Clusters"
ttClusterMonitorPrev6.HitMaps = False
ttClusterMonitorPrev6.ByDetectorRegion = False
ttClusterMonitorPrev6.ByServiceBox = True
ttClusterMonitorPrev6.ByPort = True
ttClusterMonitorPrev6.FullDetail = False
ttClusterMonitorPrev6.OutputLevel = 3

ttClusterMonitorPrev5 = ST__STClusterMonitor("TTClusterMonitorPrev5")
ttClusterMonitorPrev5.DetType = "TT"
ttClusterMonitorPrev5.ClusterLocation = "Prev5/Raw/TT/Clusters"
ttClusterMonitorPrev5.HitMaps = False
ttClusterMonitorPrev5.ByDetectorRegion = False
ttClusterMonitorPrev5.ByServiceBox = True
ttClusterMonitorPrev5.ByPort = True
ttClusterMonitorPrev5.FullDetail = False
ttClusterMonitorPrev5.OutputLevel = 3

ttClusterMonitorPrev4 = ST__STClusterMonitor("TTClusterMonitorPrev4")
ttClusterMonitorPrev4.DetType = "TT"
ttClusterMonitorPrev4.ClusterLocation = "Prev4/Raw/TT/Clusters"
ttClusterMonitorPrev4.HitMaps = False
ttClusterMonitorPrev4.ByDetectorRegion = False
ttClusterMonitorPrev4.ByServiceBox = True
ttClusterMonitorPrev4.ByPort = True
ttClusterMonitorPrev4.FullDetail = False
ttClusterMonitorPrev4.OutputLevel = 3

ttClusterMonitorPrev3 = ST__STClusterMonitor("TTClusterMonitorPrev3")
ttClusterMonitorPrev3.DetType = "TT"
ttClusterMonitorPrev3.ClusterLocation = "Prev3/Raw/TT/Clusters"
ttClusterMonitorPrev3.HitMaps = False
ttClusterMonitorPrev3.ByDetectorRegion = False
ttClusterMonitorPrev3.ByServiceBox = True
ttClusterMonitorPrev3.ByPort = True
ttClusterMonitorPrev3.FullDetail = False
ttClusterMonitorPrev3.OutputLevel = 3

ttClusterMonitorPrev2 = ST__STClusterMonitor("TTClusterMonitorPrev2")
ttClusterMonitorPrev2.DetType = "TT"
ttClusterMonitorPrev2.ClusterLocation = "Prev2/Raw/TT/Clusters"
ttClusterMonitorPrev2.HitMaps = False
ttClusterMonitorPrev2.ByDetectorRegion = False
ttClusterMonitorPrev2.ByServiceBox = True
ttClusterMonitorPrev2.ByPort = True
ttClusterMonitorPrev2.FullDetail = False
ttClusterMonitorPrev2.OutputLevel = 3

ttClusterMonitorPrev1 = ST__STClusterMonitor("TTClusterMonitorPrev1")
ttClusterMonitorPrev1.DetType = "TT"
ttClusterMonitorPrev1.ClusterLocation = "Prev1/Raw/TT/Clusters"
ttClusterMonitorPrev1.HitMaps = False
ttClusterMonitorPrev1.ByDetectorRegion = False
ttClusterMonitorPrev1.ByServiceBox = True
ttClusterMonitorPrev1.ByPort = True
ttClusterMonitorPrev1.FullDetail = False
ttClusterMonitorPrev1.OutputLevel = 3

ttClusterMonitorNext1 = ST__STClusterMonitor("TTClusterMonitorNext1")
ttClusterMonitorNext1.DetType = "TT"
ttClusterMonitorNext1.ClusterLocation = "Next1/Raw/TT/Clusters"
ttClusterMonitorNext1.HitMaps = False
ttClusterMonitorNext1.ByDetectorRegion = False
ttClusterMonitorNext1.ByServiceBox = True
ttClusterMonitorNext1.ByPort = True
ttClusterMonitorNext1.FullDetail = False
ttClusterMonitorNext1.OutputLevel = 3

ttClusterMonitorNext2 = ST__STClusterMonitor("TTClusterMonitorNext2")
ttClusterMonitorNext2.DetType = "TT"
ttClusterMonitorNext2.ClusterLocation = "Next2/Raw/TT/Clusters"
ttClusterMonitorNext2.HitMaps = False
ttClusterMonitorNext2.ByDetectorRegion = False
ttClusterMonitorNext2.ByServiceBox = True
ttClusterMonitorNext2.ByPort = True
ttClusterMonitorNext2.FullDetail = False
ttClusterMonitorNext2.OutputLevel = 3

ttClusterMonitorNext3 = ST__STClusterMonitor("TTClusterMonitorNext3")
ttClusterMonitorNext3.DetType = "TT"
ttClusterMonitorNext3.ClusterLocation = "Next3/Raw/TT/Clusters"
ttClusterMonitorNext3.HitMaps = False
ttClusterMonitorNext3.ByDetectorRegion = False
ttClusterMonitorNext3.ByServiceBox = True
ttClusterMonitorNext3.ByPort = True
ttClusterMonitorNext3.FullDetail = False
ttClusterMonitorNext3.OutputLevel = 3

ttClusterMonitorNext4 = ST__STClusterMonitor("TTClusterMonitorNext4")
ttClusterMonitorNext4.DetType = "TT"
ttClusterMonitorNext4.ClusterLocation = "Next4/Raw/TT/Clusters"
ttClusterMonitorNext4.HitMaps = False
ttClusterMonitorNext4.ByDetectorRegion = False
ttClusterMonitorNext4.ByServiceBox = True
ttClusterMonitorNext4.ByPort = True
ttClusterMonitorNext4.FullDetail = False
ttClusterMonitorNext4.OutputLevel = 3

ttClusterMonitorNext5 = ST__STClusterMonitor("TTClusterMonitorNext5")
ttClusterMonitorNext5.DetType = "TT"
ttClusterMonitorNext5.ClusterLocation = "Next5/Raw/TT/Clusters"
ttClusterMonitorNext5.HitMaps = False
ttClusterMonitorNext5.ByDetectorRegion = False
ttClusterMonitorNext5.ByServiceBox = True
ttClusterMonitorNext5.ByPort = True
ttClusterMonitorNext5.FullDetail = False
ttClusterMonitorNext5.OutputLevel = 3

ttClusterMonitorNext6 = ST__STClusterMonitor("TTClusterMonitorNext6")
ttClusterMonitorNext6.DetType = "TT"
ttClusterMonitorNext6.ClusterLocation = "Next6/Raw/TT/Clusters"
ttClusterMonitorNext6.HitMaps = False
ttClusterMonitorNext6.ByDetectorRegion = False
ttClusterMonitorNext6.ByServiceBox = True
ttClusterMonitorNext6.ByPort = True
ttClusterMonitorNext6.FullDetail = False
ttClusterMonitorNext6.OutputLevel = 3

ttClusterMonitorNext7 = ST__STClusterMonitor("TTClusterMonitorNext7")
ttClusterMonitorNext7.DetType = "TT"
ttClusterMonitorNext7.ClusterLocation = "Next7/Raw/TT/Clusters"
ttClusterMonitorNext7.HitMaps = False
ttClusterMonitorNext7.ByDetectorRegion = False
ttClusterMonitorNext7.ByServiceBox = True
ttClusterMonitorNext7.ByPort = True
ttClusterMonitorNext7.FullDetail = False
ttClusterMonitorNext7.OutputLevel = 3

#MoniTTSeq.Members.append(ttClusterMonitorPrev7)
#MoniTTSeq.Members.append(ttClusterMonitorPrev6)
#MoniTTSeq.Members.append(ttClusterMonitorPrev5)
#MoniTTSeq.Members.append(ttClusterMonitorPrev4)
#MoniTTSeq.Members.append(ttClusterMonitorPrev3)
MoniTTSeq.Members.append(ttClusterMonitorPrev2)
MoniTTSeq.Members.append(ttClusterMonitorPrev1)
MoniTTSeq.Members.append(ttClusterMonitorCentral)
MoniTTSeq.Members.append(ttClusterMonitorNext1)
MoniTTSeq.Members.append(ttClusterMonitorNext2)
#MoniTTSeq.Members.append(ttClusterMonitorNext3)
#MoniTTSeq.Members.append(ttClusterMonitorNext4)
#MoniTTSeq.Members.append(ttClusterMonitorNext5)
#MoniTTSeq.Members.append(ttClusterMonitorNext6)
#MoniTTSeq.Members.append(ttClusterMonitorNext7)

