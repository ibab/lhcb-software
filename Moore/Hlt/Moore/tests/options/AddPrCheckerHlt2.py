from Configurables import LoKi__Hybrid__MCTool
myFactory = LoKi__Hybrid__MCTool("MCHybridFactory")
myFactory.Modules = [ "LoKiMC.decorators" ]
from Moore.Configuration import Moore
Moore().OutputLevel = 3

def myActionHlt2() :   
    from Gaudi.Configuration import GaudiSequencer
    from Configurables import ( PrTrackAssociator, TrackResChecker, TrackOccupChecker,
                                PrChecker2, TrackSelector, MCReconstructible,
                                MCParticleSelector, PrLHCbID2MCParticle,
                                UnpackMCParticle, UnpackMCVertex, DebugTrackingLosses )

    from HltTracking.HltTrackNames import  HltSharedTrackLoc, HltSharedTrackRoot,Hlt2TrackRoot,Hlt2TrackLoc,HltDefaultFitSuffix
    

    PatCheck = GaudiSequencer("CheckPatSeq")

    PrChecker2("PrCheckerHlt2").TriggerNumbers = True
    PrChecker2("PrCheckerHlt2").Eta25Cut = True
    PrChecker2("PrCheckerHlt2").WriteForwardHistos = 1

    PrChecker2("PrCheckerHlt2Forward").TriggerNumbers = True
    PrChecker2("PrCheckerHlt2Forward").Eta25Cut = True
    PrChecker2("PrCheckerHlt2Forward").WriteForwardHistos = 1
    PrChecker2("PrCheckerHlt2Forward").ForwardTracks = Hlt2TrackLoc["ComplementForward"]
    #(N.B.: container base for velo tracks was renamed to Hlt1)
    # Figure out which HLT is run from HltConf
    PrChecker2("PrCheckerHlt2").SeedTracks = Hlt2TrackLoc["Seeding"]
    PrChecker2("PrCheckerHlt2").MatchTracks = Hlt2TrackLoc["Match"]
    PrChecker2("PrCheckerHlt2").DownTracks = Hlt2TrackLoc["Downstream"]
    PrChecker2("PrCheckerHlt2").ForwardTracks = Hlt2TrackLoc["Forward"]

    from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
    from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
    tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
    tracksDown = Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks()
    PrChecker2("PrCheckerHlt2").BestTracks = tracks.outputSelection()
    PrChecker2("PrCheckerHlt2Down").BestTracks = tracksDown.outputSelection()
    #PrChecker2("PrCheckerHlt2").DownTracks = tracksDown.outputSelection()

    PrTrackAssociator("AssocAllHlt2").RootOfContainers = Hlt2TrackRoot
    PrTrackAssociator("AssocFittedHlt2").RootOfContainers = Hlt2TrackRoot+HltDefaultFitSuffix
    print tracks.outputSelection()
    print Hlt2TrackRoot+HltDefaultFitSuffix
    print tracks.outputSelection()
    print Hlt2TrackRoot+HltDefaultFitSuffix
    print tracks.outputSelection()
    print Hlt2TrackRoot+HltDefaultFitSuffix


    PatCheck.Members += [ PrTrackAssociator("AssocAllHlt2"), PrTrackAssociator("AssocFittedHlt2") ]
    PatCheck.Members += [ PrChecker2("PrCheckerHlt2Forward") ]
    PatCheck.Members += [ PrChecker2("PrCheckerHlt2") ]
    PatCheck.Members += [ PrChecker2("PrCheckerHlt2Down") ]


    #EndMembers = GaudiSequencer("HltEndSequence").Members
    #EndMembers.insert(1,  PatCheck )
    #end of myAction

    
from GaudiKernel.Configurable import appendPostConfigAction
appendPostConfigAction( myActionHlt2 )








