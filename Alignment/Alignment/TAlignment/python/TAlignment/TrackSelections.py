# Base class for track selections. Each selection holds a sequence and
# an outputllistname

class AlignTrackSelection :
    __slots__ = {
        "_name" : ""   # Name of this selection"
        }
    
    def __init__( self, Name ) :
        self._name = Name

    def name( self ):
        return self._name 

    def algorithm( self ):
        return None

    def location( self ):
        return 'Rec/Track/' + self.name()

def BestTracks() :
    return AlignTrackSelection("Best")

# Base class for selectors that use a simple refiner
class AlignTrackRefiner(  AlignTrackSelection ):
    __slots__ = {
        "_inputLocation" : "" # Location of input track list"
        }

    def __init__( self, Name , InputLocation = "Rec/Track/Best" ) :
        AlignTrackSelection.__init__(self,Name = Name)
        self._inputLocation = InputLocation
        
    def algorithm( self ):
        from Configurables import TrackListRefiner
        refiner = TrackListRefiner( self.name() + "SelectorAlg",
                                    inputLocation = self._inputLocation,
                                    outputLocation = self.location())
        self.configureSelector( refiner )
        return refiner

    def configureSelector( self, selector):
        print 'Base classes must overload configureSelector'
       
# Selection for good long tracks for alignment
class GoodLongTracks( AlignTrackRefiner ):
    def __init__( self, Name = "GoodLongTracks", InputLocation = "Rec/Track/Best" ) :
        AlignTrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation)
    def configureSelector( self, a ):
        print 'inGoodLongTracks.configureSelector'
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.MaxChi2Cut = 5
        a.Selector.MaxChi2PerDoFMatch = 5
        a.Selector.MaxChi2PerDoFVelo = 5
        a.Selector.MaxChi2PerDoFDownstream = 5
        a.Selector.MinPCut  =   5000
        a.Selector.MaxPCut  = 200000
        a.Selector.MinPtCut = 2000
        a.Selector.TrackTypes = ["Long"]

# selection for box overlap tracks in IT
class ITBoxOverlapTracks( AlignTrackRefiner ):
    def __init__( self, Name = "ITBoxOverlapTracks", InputLocation = "Rec/Track/Best" ) :
        AlignTrackRefiner.__init__(self, Name, InputLocation)
    def configureSelector( self, a ):
        from Configurables import ITTrackSelector
        a.Selector = ITTrackSelector()
        a.Selector.RequireOverlap = True
        a.Selector.MinPCut =  10000
        a.Selector.MinPtCut = 500
        a.Selector.MaxChi2Cut = 5
        a.Selector.MaxChi2PerDoFMatch = 5
        a.Selector.MaxChi2PerDoFVelo = 5
        a.Selector.MaxChi2PerDoFDownstream = 5
        a.Selector.TrackTypes = ["Long"]

# selection for module overlap tracks in IT
class ITModuleOverlapTracks( AlignTrackRefiner ):
    def __init__( self, Name = "GoodLongTracks", InputLocation = "Rec/Track/Best" ) :
        AlignTrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation)
    def configureSelector( self, a ):
        from Configurables import ITTrackSelector
        a.Selector = ITTrackSelector()
        a.Selector.RequireModuleOverlap = True
        a.Selector.MinPCut =  10000
        a.Selector.MinPtCut = 100
        a.Selector.MaxChi2Cut = 5
        a.Selector.MaxChi2PerDoFMatch = 5
        a.Selector.MaxChi2PerDoFVelo = 5
        a.Selector.MaxChi2PerDoFDownstream = 5
        a.Selector.TrackTypes = ["Long"]

# selection of high momentum TTracks
class HighMomentumTTracks( AlignTrackRefiner ):
    def __init__( self, Name = "HighMomentumTTracks", InputLocation = "Rec/Track/Best" ) :
        AlignTrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.MinPCut = 50000
        a.Selector.MaxChi2Cut = 5
        a.Selector.TrackTypes = ["Ttrack"]

# selection of Velo backward tracks
class VeloBackwardTracks( AlignTrackRefiner ):
    def __init__( self, Name = "VeloBackwardTracks", InputLocation = "Rec/Track/Best" ) :
        AlignTrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.MaxChi2Cut = 5
        a.Selector.TrackTypes = ["Backward"]
        a.Selector.MinNVeloRHits = 4
        a.Selector.MinNVeloPhiHits = 4
        a.Selector.MaxNVeloHoles = 0

# selection of Velo overlap tracks
class VeloOverlapTracks( AlignTrackRefiner ):
    def __init__( self, Name = "VeloOverlapTracks", InputLocation = "Rec/Track/Best" ) :
        AlignTrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation)
    def configureSelector( self, a ):
        from Configurables import VeloTrackSelector
        a.Selector = VeloTrackSelector()
        a.Selector.MaxChi2PerDoFVelo = 10
        a.Selector.MaxChi2Cut = 5
        a.Selector.MinHitsASide = 1
        a.Selector.MinHitsCSide = 1
        a.Selector.MinNVeloRHits = 4
        a.Selector.MinNVeloPhiHits = 4
        a.Selector.MaxNVeloHoles = 0

