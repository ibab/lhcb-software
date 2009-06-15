from Configurables import (GaudiSequencer, TrackMonitor, TrackVertexMonitor,
                           TrackFitMatchMonitor,TrackV0Monitor,TrackDiMuonMonitor,
                           OTTrackMonitor)

def ConfiguredTrackMonitorSequence(Name = "TrackMonitorSequence",
                                   HistoPrint = False ):
    seq = GaudiSequencer(Name)
    seq.Members.append( TrackMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackVertexMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackFitMatchMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackV0Monitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackDiMuonMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( OTTrackMonitor(HistoPrint=HistoPrint) )
    
    return seq
