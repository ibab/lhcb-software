from Configurables import GaudiSequencer
from Configurables import TrackSmeared
# file to smear proto particles and their associated track
smear = TrackSmeared("TrackSmeared")
#smear.InputLocation = "Rec/Track/Best"
#smear.Scale = 1.0
smear.smearBest = True
#smear.OutputLevel = 2
smearBest = GaudiSequencer("SmearBest")
smearBest.Members += [ smear ]


