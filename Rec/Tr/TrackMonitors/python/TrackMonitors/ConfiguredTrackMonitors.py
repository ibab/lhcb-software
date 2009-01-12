from Configurables import (GaudiSequencer, TrackMonitor, TrackVertexMonitor,
                           TrackFitMatchMonitor,TrackV0Monitor)

def ConfiguredTrackMonitorSequence(Name = "TrackMonitorSequence",
                                   HistoPrint = False ):
    seq = GaudiSequencer(Name)
    seq.Members.append( TrackMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackVertexMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackFitMatchMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackV0Monitor(HistoPrint=HistoPrint) )
        
    return seq
