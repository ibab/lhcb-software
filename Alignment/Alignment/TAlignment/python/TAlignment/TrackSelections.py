# Base class for track selections. Each selection holds a sequence and
# an outputllistname

class TrackSelection :
    __slots__ = {
        "_name" : ""   # Name of this selection"
        }
    
    def __init__( self, Name ) : 
        self._name = Name

    def name( self ): return self._name 
    def algorithm( self ): return None
    def location( self ): return 'Rec/Track/' + self.name()

def BestTracks() :
    return TrackSelection("Best")

# Base class for selectors that use a simple refiner
class TrackRefiner(  TrackSelection ):
    __slots__ = {
        "_inputLocation" : "" # Location of input track list"
        ,"_fitted" : True
        }

    def __init__( self, Name , InputLocation = "Rec/Track/Best", Fitted=True ) :
        TrackSelection.__init__(self,Name = Name)
        self._inputLocation = InputLocation
        self._fitted = Fitted
        
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
class GoodLongTracks( TrackRefiner ):
    def __init__( self, Name = "GoodLongTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation,
                                   Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.MinPCut  =   5000
        a.Selector.MaxPCut  = 200000
        a.Selector.MinPtCut =    200
        a.Selector.TrackTypes = ["Long"]
        if self._fitted:
            a.Selector.MaxChi2Cut = 5
            a.Selector.MaxChi2PerDoFMatch = 5
            a.Selector.MaxChi2PerDoFVelo = 5
            a.Selector.MaxChi2PerDoFDownstream = 5


# selection for box overlap tracks in IT
class ITBoxOverlapTracks( TrackRefiner ):
    def __init__( self, Name = "ITBoxOverlapTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name, InputLocation, Fitted)
    def configureSelector( self, a ):
        from Configurables import ITTrackSelector
        a.Selector = ITTrackSelector()
        a.Selector.RequireOverlap = True
        a.Selector.MinPCut =  5000
        a.Selector.TrackTypes = ["Long"]
        if self._fitted:
            a.Selector.MaxChi2Cut = 5
            a.Selector.MaxChi2PerDoFMatch = 5
            a.Selector.MaxChi2PerDoFVelo = 5
            a.Selector.MaxChi2PerDoFDownstream = 5

# selection for module overlap tracks in IT
class ITModuleOverlapTracks( TrackRefiner ):
    def __init__( self, Name = "GoodLongTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import ITTrackSelector
        a.Selector = ITTrackSelector()
        a.Selector.RequireModuleOverlap = True
        a.Selector.MinPCut =  10000
        a.Selector.MinPtCut = 100
        a.Selector.TrackTypes = ["Long"]
        if self._fitted:
            a.Selector.MaxChi2Cut = 5
            a.Selector.MaxChi2PerDoFMatch = 5
            a.Selector.MaxChi2PerDoFVelo = 5
            a.Selector.MaxChi2PerDoFDownstream = 5

# selection of high momentum TTracks
class HighMomentumTTracks( TrackRefiner ):
    def __init__( self, Name = "HighMomentumTTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.MinPCut = 50000
        a.Selector.TrackTypes = ["Ttrack"]
        if self._fitted:
            a.Selector.MaxChi2Cut = 5

# selection of Velo backward tracks
class VeloBackwardTracks( TrackRefiner ):
    def __init__( self, Name = "VeloBackwardTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.TrackTypes = ["Backward"]
        a.Selector.MinNVeloRHits = 4
        a.Selector.MinNVeloPhiHits = 4
        a.Selector.MaxNVeloHoles = 0
        if self._fitted:
            a.Selector.MaxChi2Cut = 5


# selection of Velo overlap tracks
class VeloOverlapTracks( TrackRefiner ):
    def __init__( self, Name = "VeloOverlapTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import VeloTrackSelector
        a.Selector = VeloTrackSelector()
        a.Selector.MinHitsASide = 1
        a.Selector.MinHitsCSide = 1
        a.Selector.MinNVeloRHits = 4
        a.Selector.MinNVeloPhiHits = 4
        a.Selector.MaxNVeloHoles = 0
        a.Selector.TrackTypes = ["Long","Velo"]
        if self._fitted:
            a.Selector.MaxChi2PerDoFVelo = 5
            a.Selector.MaxChi2Cut = 5


# a favourite cocktail of rare tracks
class FavouriteTrackCocktail(TrackSelection): 
    __slots__ = {
        "_inputLocation" : "" # Location of input track list"
        , "_fitted" : True
        }
    def __init__( self, Name, InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackSelection.__init__(self, Name )
        self._inputLocation = InputLocation
        self._fitted = Fitted
    def algorithm( self ):
        from Configurables import GaudiSequencer, TrackListMerger
        Name = self._name
        loc = self._inputLocation
        b = self._fitted
        seq = GaudiSequencer( Name + "SelectionSeq" )
        selections = [ VeloOverlapTracks(Name = Name + "VeloOverlap",
                                         InputLocation = loc,Fitted = b),
                       VeloBackwardTracks(Name = Name + "VeloBackward",
                                          InputLocation = loc,Fitted= b),
                       ITBoxOverlapTracks(Name = Name + "ITBoxOverlap",
                                          InputLocation = loc,Fitted = b),
                       GoodLongTracks(Name = Name + "GoodLong",
                                      InputLocation = loc,Fitted = b) ]
        seq.Members += [ i.algorithm() for i in selections ]
        merger = TrackListMerger( Name + "Merger" )
        merger.inputLocations = [ i.location() for i in selections ]
        merger.outputLocation = self.location()
        seq.Members.append( merger )
        return seq 

# This configures a complete sequence to get the tracks from the HLT
# event. It puts them in a different location than usual.
class NoPIDTracksFromHlt(TrackSelection):
    def __init__( self, RerunVeloTracking = False ) :
        TrackSelection.__init__(self, 'NoPIDTracksFromHlt')
        self._runVelo = RerunVeloTracking

    def algorithm( self ):
        from Configurables import GaudiSequencer, HltTrackConverter, TrackContainerCopy
        from TAlignment.Utils import configuredFitAndHitAdderSequence
        seq = GaudiSequencer(self.name() + "Seq")

        # configure algorithm to revive HLT tracks. it seems that the
        # Hlt2Global decision is the most useful, because it has TT
        # hits on the tracks. adding Hlt1Global does rather
        # little. (if you do, make sure to put it after Hlt2Global,
        # since the clone rejection takes the first track.)
        hltTrackConvAll = HltTrackConverter("HltTrackConvAll",TrackDestignation = 'Rec/Track/NoPIDBest',
                                            HltLinesToUse = ['Hlt2Global'])
        seq.Members += [ hltTrackConvAll ]
        # configure algorithm to run Velo standalone reconstruction
        if self._runVelo:
            # run FastVeloTracks and copy all tracks to NoPIDBest. We
            # need to deal with clones at some point.
            from Configurables import FastVeloTracking
            seq.Members += [FastVeloTracking(),
                            TrackContainerCopy('CopyVeloTracks',
                                               inputLocation = 'Rec/Track/Velo',
                                               outputLocation = 'Rec/Track/NoPIDBest')]
            
        tracksel = FavouriteTrackCocktail(Name ='NoPIDForAlignment',
                                          InputLocation = 'Rec/Track/NoPIDBest',
                                          Fitted = False)
        seq.Members += [ tracksel.algorithm(),
                         TrackContainerCopy('NoPIDForAlignmentCopy',
                                            inputLocation = tracksel.location(),
                                            outputLocation = tracksel.location() + 'Owned'),
                         configuredFitAndHitAdderSequence('NoPIDFit',
                                                          InputLocation = tracksel.location() + 'Owned',
                                                          OutputLocation = self.location() ) ]
        return seq
