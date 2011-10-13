#Configuration file to smear copied tracks
#which can then be used to make protoparticles and particles
from Gaudi.Configuration import *
from Configurables import TrackSmeared
#smear = TrackSmeared("TrackSmeared")
#smear.InputLocation = "Rec/Track/Best"
#smear.smearCopied = True
#set the path right
importOptions("$TRACKSMEARINGROOT/options/MakeMyParticles.py")
