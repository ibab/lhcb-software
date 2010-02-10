from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

MoniITSeq = GaudiSequencer( 'MoniITSeq' )
MoniITSeq.MeasureTime = True
#ApplicationMgr().TopAlg.append( MoniITSeq )

from Configurables import ( ST__STClusterMonitor )
        
itClusterMonitorCentral = ST__STClusterMonitor("ITClusterMonitorCentral")
itClusterMonitorCentral.DetType = "IT"
itClusterMonitorCentral.ClusterLocation = "Raw/IT/Clusters"
itClusterMonitorCentral.HitMaps = True
itClusterMonitorCentral.ByDetectorRegion = False
itClusterMonitorCentral.ByServiceBox = True
itClusterMonitorCentral.ByPort = True;
itClusterMonitorCentral.FullDetail = False
itClusterMonitorCentral.OutputLevel = 3

itClusterMonitorPrev7 = ST__STClusterMonitor("ITClusterMonitorPrev7")
itClusterMonitorPrev7.DetType = "IT"
itClusterMonitorPrev7.ClusterLocation = "Prev7/Raw/IT/Clusters"
itClusterMonitorPrev7.HitMaps = False
itClusterMonitorPrev7.ByDetectorRegion = False
itClusterMonitorPrev7.ByServiceBox = True
itClusterMonitorPrev7.ByPort = True;
itClusterMonitorPrev7.FullDetail = False
itClusterMonitorPrev7.OutputLevel = 3

itClusterMonitorPrev6 = ST__STClusterMonitor("ITClusterMonitorPrev6")
itClusterMonitorPrev6.DetType = "IT"
itClusterMonitorPrev6.ClusterLocation = "Prev6/Raw/IT/Clusters"
itClusterMonitorPrev6.HitMaps = False
itClusterMonitorPrev6.ByDetectorRegion = False
itClusterMonitorPrev6.ByServiceBox = True
itClusterMonitorPrev6.ByPort = True;
itClusterMonitorPrev6.FullDetail = False
itClusterMonitorPrev6.OutputLevel = 3

itClusterMonitorPrev5 = ST__STClusterMonitor("ITClusterMonitorPrev5")
itClusterMonitorPrev5.DetType = "IT"
itClusterMonitorPrev5.ClusterLocation = "Prev5/Raw/IT/Clusters"
itClusterMonitorPrev5.HitMaps = False
itClusterMonitorPrev5.ByDetectorRegion = False
itClusterMonitorPrev5.ByServiceBox = True
itClusterMonitorPrev5.ByPort = True;
itClusterMonitorPrev5.FullDetail = False
itClusterMonitorPrev5.OutputLevel = 3

itClusterMonitorPrev4 = ST__STClusterMonitor("ITClusterMonitorPrev4")
itClusterMonitorPrev4.DetType = "IT"
itClusterMonitorPrev4.ClusterLocation = "Prev4/Raw/IT/Clusters"
itClusterMonitorPrev4.HitMaps = False
itClusterMonitorPrev4.ByDetectorRegion = False
itClusterMonitorPrev4.ByServiceBox = True
itClusterMonitorPrev4.ByPort = True;
itClusterMonitorPrev4.FullDetail = False
itClusterMonitorPrev4.OutputLevel = 3

itClusterMonitorPrev3 = ST__STClusterMonitor("ITClusterMonitorPrev3")
itClusterMonitorPrev3.DetType = "IT"
itClusterMonitorPrev3.ClusterLocation = "Prev3/Raw/IT/Clusters"
itClusterMonitorPrev3.HitMaps = False
itClusterMonitorPrev3.ByDetectorRegion = False
itClusterMonitorPrev3.ByServiceBox = True
itClusterMonitorPrev3.ByPort = True;
itClusterMonitorPrev3.FullDetail = False
itClusterMonitorPrev3.OutputLevel = 3

itClusterMonitorPrev2 = ST__STClusterMonitor("ITClusterMonitorPrev2")
itClusterMonitorPrev2.DetType = "IT"
itClusterMonitorPrev2.ClusterLocation = "Prev2/Raw/IT/Clusters"
itClusterMonitorPrev2.HitMaps = False
itClusterMonitorPrev2.ByDetectorRegion = False
itClusterMonitorPrev2.ByServiceBox = True
itClusterMonitorPrev2.ByPort = True;
itClusterMonitorPrev2.FullDetail = False
itClusterMonitorPrev2.OutputLevel = 3

itClusterMonitorPrev1 = ST__STClusterMonitor("ITClusterMonitorPrev1")
itClusterMonitorPrev1.DetType = "IT"
itClusterMonitorPrev1.ClusterLocation = "Prev1/Raw/IT/Clusters"
itClusterMonitorPrev1.HitMaps = False
itClusterMonitorPrev1.ByDetectorRegion = False
itClusterMonitorPrev1.ByServiceBox = True
itClusterMonitorPrev1.ByPort = True;
itClusterMonitorPrev1.FullDetail = False
itClusterMonitorPrev1.OutputLevel = 3

itClusterMonitorNext1 = ST__STClusterMonitor("ITClusterMonitorNext1")
itClusterMonitorNext1.DetType = "IT"
itClusterMonitorNext1.ClusterLocation = "Next1/Raw/IT/Clusters"
itClusterMonitorNext1.HitMaps = False
itClusterMonitorNext1.ByDetectorRegion = False
itClusterMonitorNext1.ByServiceBox = True
itClusterMonitorNext1.ByPort = True;
itClusterMonitorNext1.FullDetail = False
itClusterMonitorNext1.OutputLevel = 3

itClusterMonitorNext2 = ST__STClusterMonitor("ITClusterMonitorNext2")
itClusterMonitorNext2.DetType = "IT"
itClusterMonitorNext2.ClusterLocation = "Next2/Raw/IT/Clusters"
itClusterMonitorNext2.HitMaps = False
itClusterMonitorNext2.ByDetectorRegion = False
itClusterMonitorNext2.ByServiceBox = True
itClusterMonitorNext2.ByPort = True;
itClusterMonitorNext2.FullDetail = False
itClusterMonitorNext2.OutputLevel = 3

itClusterMonitorNext3 = ST__STClusterMonitor("ITClusterMonitorNext3")
itClusterMonitorNext3.DetType = "IT"
itClusterMonitorNext3.ClusterLocation = "Next3/Raw/IT/Clusters"
itClusterMonitorNext3.HitMaps = False
itClusterMonitorNext3.ByDetectorRegion = False
itClusterMonitorNext3.ByServiceBox = True
itClusterMonitorNext3.ByPort = True;
itClusterMonitorNext3.FullDetail = False
itClusterMonitorNext3.OutputLevel = 3

itClusterMonitorNext4 = ST__STClusterMonitor("ITClusterMonitorNext4")
itClusterMonitorNext4.DetType = "IT"
itClusterMonitorNext4.ClusterLocation = "Next4/Raw/IT/Clusters"
itClusterMonitorNext4.HitMaps = False
itClusterMonitorNext4.ByDetectorRegion = False
itClusterMonitorNext4.ByServiceBox = True
itClusterMonitorNext4.ByPort = True;
itClusterMonitorNext4.FullDetail = False
itClusterMonitorNext4.OutputLevel = 3

itClusterMonitorNext5 = ST__STClusterMonitor("ITClusterMonitorNext5")
itClusterMonitorNext5.DetType = "IT"
itClusterMonitorNext5.ClusterLocation = "Next5/Raw/IT/Clusters"
itClusterMonitorNext5.HitMaps = False
itClusterMonitorNext5.ByDetectorRegion = False
itClusterMonitorNext5.ByServiceBox = True
itClusterMonitorNext5.ByPort = True;
itClusterMonitorNext5.FullDetail = False
itClusterMonitorNext5.OutputLevel = 3

itClusterMonitorNext6 = ST__STClusterMonitor("ITClusterMonitorNext6")
itClusterMonitorNext6.DetType = "IT"
itClusterMonitorNext6.ClusterLocation = "Next6/Raw/IT/Clusters"
itClusterMonitorNext6.HitMaps = False
itClusterMonitorNext6.ByDetectorRegion = False
itClusterMonitorNext6.ByServiceBox = True
itClusterMonitorNext6.ByPort = True;
itClusterMonitorNext6.FullDetail = False
itClusterMonitorNext6.OutputLevel = 3

itClusterMonitorNext7 = ST__STClusterMonitor("ITClusterMonitorNext7")
itClusterMonitorNext7.DetType = "IT"
itClusterMonitorNext7.ClusterLocation = "Next7/Raw/IT/Clusters"
itClusterMonitorNext7.HitMaps = False
itClusterMonitorNext7.ByDetectorRegion = False
itClusterMonitorNext7.ByServiceBox = True
itClusterMonitorNext7.ByPort = True;
itClusterMonitorNext7.FullDetail = False
itClusterMonitorNext7.OutputLevel = 3

#MoniITSeq.Members.append(itClusterMonitorPrev7)
#MoniITSeq.Members.append(itClusterMonitorPrev6)
#MoniITSeq.Members.append(itClusterMonitorPrev5)
#MoniITSeq.Members.append(itClusterMonitorPrev4)
#MoniITSeq.Members.append(itClusterMonitorPrev3)
MoniITSeq.Members.append(itClusterMonitorPrev2)
MoniITSeq.Members.append(itClusterMonitorPrev1)
MoniITSeq.Members.append(itClusterMonitorCentral)
MoniITSeq.Members.append(itClusterMonitorNext1)
MoniITSeq.Members.append(itClusterMonitorNext2)
#MoniITSeq.Members.append(itClusterMonitorNext3)
#MoniITSeq.Members.append(itClusterMonitorNext4)
#MoniITSeq.Members.append(itClusterMonitorNext5)
#MoniITSeq.Members.append(itClusterMonitorNext6)
#MoniITSeq.Members.append(itClusterMonitorNext7)
