class VertexSelection:
    def __init__( self, Name, Location, Algorithm = None ) :
        self._name = Name
        self._location = Location
        self._algorithm = Algorithm

    def algorithm(self) : return self._algorithm
    def location(self) : return self._location
    def name(self) : return self._name

def configuredPVSelection():
    # we assume that the PV reconstruction has already been performed.
    # (can always add later)
    inputlocation = "Rec/Vertex/Primary"
    outputlocation = "Rec/Vertex/AlignPrimaryVertices"
    from Configurables import TrackPVRefitter,VertexListRefiner,GaudiSequencer
    pvrefitter = TrackPVRefitter("AlignPVRefitter",
                                 PVContainer =  "Rec/Vertex/Primary")
    pvselector = VertexListRefiner("AlignPVSelector",
                                   InputLocation = "Rec/Vertex/Primary",
                                   OutputLocation = "Rec/Vertex/AlignPrimaryVertices",
                                   MaxChi2PerDoF = 5,
                                   MinNumTracks = 10,
                                   MinNumLongTracks = 2 )
    seq = GaudiSequencer("AlignPVSelSeq")
    seq.Members += [ pvrefitter, pvselector ]
    sel = VertexSelection(Name = "DefaultPVSelection",
                          Location = "Rec/Vertex/AlignPrimaryVertices",
                          Algorithm = seq)
    return sel

def configuredLoosePVSelection():
    # we assume that the PV reconstruction has already been performed.
    # (can always add later)
    inputlocation = "Rec/Vertex/Primary"
    outputlocation = "Rec/Vertex/AlignPrimaryVertices"
    from Configurables import TrackPVRefitter,VertexListRefiner,GaudiSequencer
    pvrefitter = TrackPVRefitter("AlignPVRefitter",
                                 PVContainer =  "Rec/Vertex/Primary")
    pvselector = VertexListRefiner("AlignPVSelector",
                                   InputLocation = "Rec/Vertex/Primary",
                                   OutputLocation = "Rec/Vertex/AlignPrimaryVertices",
                                   MaxChi2PerDoF = 5,
                                   MinNumTracks = 4,
                                   MinNumLongTracks = 0 )
    seq = GaudiSequencer("AlignPVSelSeq")
    seq.Members += [ pvrefitter, pvselector ]
    sel = VertexSelection(Name = "DefaultPVSelection",
                          Location = "Rec/Vertex/AlignPrimaryVertices",
                          Algorithm = seq)
    return sel

def configuredPVSelectionFromDST():
    print "I still need to implement this. "\
        "Use the PV Refitter. It can also refit the tracks"
    return None
