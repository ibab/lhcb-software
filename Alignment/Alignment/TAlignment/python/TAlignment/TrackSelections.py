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
                                    inputLocation = self.inputLocation(),
                                    outputLocation = self.location())
        self.configureSelector( refiner )
        return refiner

    def inputLocation(self) : 
        return self._inputLocation

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
        a.Selector.MaxNTHoles = 1
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

# selection of Long tracks refitted with only Velo segment
class GoodLongRefittedVeloTracks(TrackSelection):
   # __slots__ = {
   #     "_inputLocation" : "" # Location of input track list"
   #     }
    
    def __init__( self, Name, InputLocation = "Rec/Track/Best" ) :  
        TrackSelection.__init__(self, "GoodLongRefittedVeloTracks")
        self._inputLocation = InputLocation
    def algorithm( self ):
        Name = self._name
        inputTrackLocation = self._inputLocation
        from Configurables import GaudiSequencer, HltTrackConverter, TrackContainerCopy, Escher
        from Configurables import TrackContainerCopy, TrackSelector
        trackRefitSeq = GaudiSequencer(self.name() + "Seq")# GaudiSequencer("TrackRefitSeq")

        # create a track list for tracks with velo hits
        velotrackselector = TrackContainerCopy("GoodLongRefittedVeloTracks",
                                               inputLocations = ["Rec/Track/Best"],
                                               outputLocation = "Rec/Track/GoodLongRefittedVeloTracks",
                                               Selector = TrackSelector())
        velotrackselector.Selector.MinNVeloRHits = 7
        velotrackselector.Selector.MinNVeloPhiHits = 6
        # refit the tracks in that list
        from TrackFitter.ConfiguredFitters import *
        velotrackrefitter = ConfiguredEventFitter(Name = "TracksWithVeloHitsFitter",
                                                  TracksInContainer = "Rec/Track/GoodLongRefittedVeloTracks",
                                                  FieldOff=True)
        velotrackrefitter.Fitter.MeasProvider.IgnoreIT = True
        velotrackrefitter.Fitter.MeasProvider.IgnoreOT = True
        velotrackrefitter.Fitter.MeasProvider.IgnoreTT = True
        velotrackrefitter.Fitter.MeasProvider.IgnoreMuon = True
        velotrackrefitter.Fitter.MakeNodes = True
        velotrackrefitter.Fitter.MakeMeasurements = True

        trackRefitSeq.Members += [velotrackselector, velotrackrefitter]
        return trackRefitSeq


# selection of Velo backward tracks
class VeloBackwardTracks( TrackRefiner ):
    def __init__( self, Name = "VeloBackwardTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.TrackTypes = ["Backward"]
        a.Selector.MinNVeloRHits = 6
        a.Selector.MinNVeloPhiHits = 6
        a.Selector.MaxNVeloHoles = 0
        if self._fitted:
            a.Selector.MaxChi2Cut = 5

# selection of Velo backward tracks
class VeloOnlyTracks( TrackRefiner ):
    def __init__( self, Name = "VeloOnlyTracks", InputLocation = "Rec/Track/Best", Fitted = True ) :
        TrackRefiner.__init__(self, Name = Name, InputLocation = InputLocation, Fitted = Fitted)
    def configureSelector( self, a ):
        from Configurables import TrackSelector
        a.Selector = TrackSelector()
        a.Selector.TrackTypes = ["Velo","Backward"]
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
        a.Selector.MinHitsASide = 3
        a.Selector.MinHitsCSide = 3
        a.Selector.MinNVeloRHits = 7
        a.Selector.MinNVeloPhiHits = 7
        a.Selector.MaxNVeloHoles = 1
        a.Selector.TrackTypes = ["Velo","Backward"]
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
        from Configurables import GaudiSequencer, HltTrackConverter, TrackContainerCopy, Escher
        from TAlignment.Utils import configuredFitAndHitAdderSequence
        seq = GaudiSequencer(self.name() + "Seq")

        # configure algorithm to revive HLT tracks. it seems that the
        # Hlt2Global decision is the most useful, because it has TT
        # hits on the tracks. adding Hlt1Global does rather
        # little. (if you do, make sure to put it after Hlt2Global,
        # since the clone rejection takes the first track.)
        hltTrackConvAll = HltTrackConverter("HltTrackConvAll",TrackDestination = 'Rec/Track/NoPIDBest')
        hltTrackConvAll.HltLinesToUse += ['Hlt2Global','Hlt1Global']
        
        if Escher().DataType in ['2015']: hltTrackConvAll.SelReportsLocation = 'Hlt1/SelReports'
        seq.Members += [ hltTrackConvAll ]
        # configure algorithm to run Velo standalone reconstruction
        if self._runVelo:
            # run FastVeloTracks and copy all tracks to NoPIDBest. We
            # need to deal with clones at some point.
            from Configurables import FastVeloTracking
            seq.Members += [FastVeloTracking(),
                            TrackContainerCopy('CopyVeloTracks',
                                               inputLocations = ['Rec/Track/Velo'],
                                               outputLocation = 'Rec/Track/NoPIDBest')]
            
        tracksel = FavouriteTrackCocktail(Name ='NoPIDForAlignment',
                                          InputLocation = 'Rec/Track/NoPIDBest',
                                          Fitted = False)
        seq.Members += [ tracksel.algorithm(),
                         TrackContainerCopy('NoPIDForAlignmentCopy',
                                            inputLocations = [tracksel.location()],
                                            outputLocation = tracksel.location() + 'Owned'),
                         configuredFitAndHitAdderSequence('NoPIDFit',
                                                          InputLocation = tracksel.location() + 'Owned',
                                                          OutputLocation = self.location() ) ]
        return seq


# This configures a complete sequence to get the tracks from the HLT
# event. It puts them in a different location than usual.
class BestMuonTracks(TrackSelection):    
   # __slots__ = {
   #     "_inputLocation" : "" # Location of input track list"
   #     }
    
    def __init__( self, Name, InputLocation = "Rec/Track/Best" ) :  # ????  where is defined _runVelo ??        
        TrackSelection.__init__(self, "BestMuonTracks")
        self._inputLocation = InputLocation

    def algorithm( self ):
        from Configurables import TrackSys
        TrackSys().GlobalCuts = {'Velo':4000, 'OT':8000} #### to remove busy events
        Name = self._name
        inputTrackLocation = self._inputLocation
        from Configurables import GaudiSequencer, HltTrackConverter, TrackContainerCopy, Escher
        from TAlignment.Utils import configuredFitAndHitAdderSequence
        seq = GaudiSequencer(self.name() + "Seq")
        ## Here starts Stefania's code ========================
        from Configurables import MuonNNetRec, MuonPadRec, MuonClusterRec

        muonTrackTool = MuonNNetRec(name="MuonNNetRec",
                                    AssumeCosmics = False,
                                    PhysicsTiming = False,
                                    AssumePhysics = True, ## assume that tracks come from IP (only forward)
                                    AddXTalk      = True,
                                    XtalkRadius   = 1.5,
                                    SkipStation   = 0,
                                    #DecodingTool = "MuonMonHitDecode", # default is "MuonHitDecode"
                                    ClusterTool   = "MuonClusterRec" # default is "MuonFakeClustering"
                                    #OutputLevel   = 2
                                    )
        
        clusterTool = MuonClusterRec(name="MuonClusterRec",OutputLevel=5, MaxPadsPerStation=500) #default 1500
        padRecTool  = MuonPadRec(name = "PadRecTool",OutputLevel=5)

        from Configurables import MakeMuonTracks
        copy  = MakeMuonTracks( name='MakeMuonTracks',
                                OutputLevel     = 5,
                                SkipBigClusters = True )
        copy.addTool( muonTrackTool, name = 'MuonRecTool' )
        # ------------------------------------------------------------------
        seq.Members +=[ copy ]          # Copy MuonTracks to LHCb tracks
        # ------------------------------------------------------------------
        from TrackFitter.ConfiguredFitters import ConfiguredForwardStraightLineEventFitter #, ConfiguredStraightLineFitter
        from Configurables import MeasurementProvider,TrackKalmanFilter

        muonTrackFit =  ConfiguredForwardStraightLineEventFitter("MuonTrackFit",TracksInContainer = "Rec/Track/Muon")
        #muonTrackFit =  ConfiguredStraightLineFitter( 'MuonTrackFit' , TracksInContainer = 'Rec/Track/Muon' ) 

        muonTrackFit.Fitter.OutputLevel             = 5
        muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
        muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
        

        muonTrackFit.Fitter.MeasProvider.MuonProvider.clusterize = True  #default False
        muonTrackFit.Fitter.MeasProvider.MuonProvider.OutputLevel = 5
        muonTrackFit.Fitter.ErrorX                  = 1000
        muonTrackFit.Fitter.ErrorY                  = 10000
        muonTrackFit.Fitter.MaxNumberOutliers       = 0        
        # ------------------------------------------------------------------
        seq.Members+=[ muonTrackFit ]
        # ------------------------------------------------------------------
        from Configurables import TAlignment,GetElementsToBeAligned
        from Configurables import TrackFilterAlg
        trackFilterAlg = TrackFilterAlg(name="FilterTracks",
                                        OutputLevel = 5,
                                        TracksInputContainer  = "Rec/Track/Muon",         # MuonTrackRec
                                        TracksOutputContainer = "Rec/Track/SelectedMuon",  # Filtered tracks
                                        MuonFilter   = True,   ###------------------------------ MUONTRACKSELECTOR
                                        MuonPcut     = 6000.,   ## 6 GeV
                                        MuonChisquareCut = 5,
                                        inCaloAcceptance = True,
                                        noOverlap        = False,
                                        #TheRegion       = 10
                                        minHitStation    = 3)  # >3
        
        ####???????
        trackFilterAlg.addTool(GetElementsToBeAligned (OutputLevel = 5,
                                                       Elements  = TAlignment().ElementsToAlign ),
                               name = "GetElementsToBeAligned")                               
        # ------------------------------------------------------------------
        seq.Members += [trackFilterAlg]
        # ------------------------------------------------------------------
        from Configurables import TrackMuonMatching, TrackMasterExtrapolator, TrackChi2Calculator
        #, TrajOTCosmicsProjector, TrajOTProjector, TrackProjectorSelector
        matching = TrackMuonMatching(name="TrackMuonMatching",
                                     AllCombinations     = False, # default true
                                     MatchAtFirstMuonHit = True,  # default false
                                     FullDetail          = True,  #????
                                     OutputLevel         = 5,
                                     TTracksLocation     = inputTrackLocation, # Best container + chisquare < 5
                                     MuonTracksLocation  = 'Rec/Track/SelectedMuon', # MuonNNet
                                     TracksOutputLocation= 'Rec/Track/Best/TMuon',
                                     MatchChi2Cut        = 20.0,
                                     WriteNtuple         = False )
        matching.addTool( TrackMasterExtrapolator, name         = 'Extrapolator' )
        matching.Extrapolator.ApplyMultScattCorr                = True
        matching.Extrapolator.ApplyEnergyLossCorr               = False
        matching.Extrapolator.ApplyElectronEnergyLossCorr       = False
        matching.addTool( TrackChi2Calculator, name             = 'Chi2Calculator' )
        # ------------------------------------------------------------------
        seq.Members+= [ matching ]     # Track-Muon  Matching
        # ------------------------------------------------------------------

        from TrackFitter.ConfiguredFitters import ConfiguredEventFitter        
        #tmuonfit = ConfiguredFastEventFitter( 'TMuonFit', TracksInContainer = 'Rec/Track/Best/TMuon') # contains Best+Muon
        tmuonfit = ConfiguredEventFitter("TMuonFit",   
                                         TracksInContainer = 'Rec/Track/Best/TMuon') # contains Best+Muon
                                        
        tmuonfit.Fitter.MakeNodes                               = True
        tmuonfit.Fitter.MakeMeasurements                        = True
        tmuonfit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
        tmuonfit.Fitter.ErrorX                                 = 1000
        tmuonfit.Fitter.ErrorY                                 = 10000
        tmuonfit.Fitter.NumberFitIterations                    = 5
        tmuonfit.Fitter.MaxDeltaChiSqConverged                 = 0.1
        tmuonfit.Fitter.MaxNumberOutliers                       = 0
        tmuonfit.Fitter.addTool( MeasurementProvider(), name    = 'MeasProvider')
        tmuonfit.Fitter.MeasProvider.MuonProvider.clusterize    = True 
        #tmuonfit.Fitter.MeasProvider.MuonProvider.OutputLevel   = 2
        tmuonfit.Fitter.MeasProvider.IgnoreMuon = False              
        # ------------------------------------------------------------------
        seq.Members+= [ tmuonfit ]   # Fit TMuon tracks
        # ------------------------------------------------------------------
        from Configurables import TrackSelector,TrackContainerCopy
        tmuonselectoralg  = TrackContainerCopy(name = "TmuonSelection",
                                               inputLocations  =   ['Rec/Track/Best/TMuon'],
                                               outputLocation =   self.location())  
        tmuonselectoralg.addTool( TrackSelector, name = "Selector")
        tmuonselectoralg.Selector.MaxChi2Cut = 10
        #tmuonselectoralg.Selector.OutputLevel = 2
        # ------------------------------------------------------------------
        seq.Members+= [tmuonselectoralg ] # selects good TMuon tracks
        # ------------------------------------------------------------------

        return seq
