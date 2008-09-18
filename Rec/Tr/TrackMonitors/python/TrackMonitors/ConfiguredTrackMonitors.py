from Configurables import (GaudiSequencer, TrackMonitor, TrackVertexMonitor,
                           TrackFitMatchMonitor)

def ConfiguredTrackMonitorSequence(Name = "TrackMonitorSequence",
                                   HistoPrint = False ):
    seq = GaudiSequencer(Name)
    seq.Members.append( TrackMonitor(HistoPrint) )
    seq.Members.append( TrackVertexMonitor(HistoPrint) )
    seq.Members.append( TrackFitMatchMonitor(HistoPrint) )
    return seq
