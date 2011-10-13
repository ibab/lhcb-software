from Configurables import GaudiSequencer
from Configurables import TrackSmeared


# file to smear proto particles and their associated track
smear = TrackSmeared("TrackSmeared")
#smear.InputLocation = "Rec/Track/Best"
output = "Smeared"
smear.OutputLocation = output
#smear.Scale = 1.0
smear.smearProto = True
#smear.OutputLevel = 2

#get MC association back
from Configurables import TrackToDST, ChargedPP2MC, TrackAssociator
output="Smeared"
assoctr = TrackAssociator(output+"AssocTr")
assoctr.TracksInContainer = "Rec/Track/"+output
assocpp=ChargedPP2MC(output+"AssocPP")
assocpp.TrackLocations = [ "Rec/Track/"+output ]
assocpp.InputData = [ "Rec/ProtoP/"+output+"ProtoPMaker" ]
assocpp.OutputTable = "Relations/Rec/ProtoP/"+output+"ProtoPMaker"

# add smearProto to a DaVinci sequence before you make the selection
smearProto = GaudiSequencer("SmearProto")
smearProto.Members += [ smear,assoctr,assocpp ]


#importOptions("$TRACKSMEARINGROOT/MyLoosePions.py")
