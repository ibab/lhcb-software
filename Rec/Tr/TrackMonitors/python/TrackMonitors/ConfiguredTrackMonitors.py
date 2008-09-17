from Configurables import (GaudiSequencer, TrackMonitor, TrackVertexMonitor,
                           TrackFitMatchMonitor)

def ConfiguredTrackMonitorSequence(Name = "TrackMonitorSequence"):
    seq = GaudiSequencer(Name)
    seq.Members.append( TrackMonitor() )
    seq.Members.append( TrackVertexMonitor() )
    seq.Members.append( TrackFitMatchMonitor() )
    return seq
