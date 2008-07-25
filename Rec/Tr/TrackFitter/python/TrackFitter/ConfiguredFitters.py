
# ====================================================================
#  Track fitting options
# ====================================================================

# --------------------------------------------------------------------
# NOTE:
# These options assume the following conventions:
#
# 1. the instances of the TrackEventFitter are named as
#    Fit<track_container_name>
#
# 2. the track containers considered are:
#    Forward, Seed, Match, KsTrack, VeloTT, PreparedVelo
# --------------------------------------------------------------------

from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackKalmanFilter,
                            TrackProjectorSelector, TrajOTProjector, TrackMasterExtrapolator,
                            TrackSimpleExtraSelector, SimplifiedMaterialLocator, DetailedMaterialLocator)

_fitterdictionary = {}

def createConfiguredFitters( FieldOff = False, SimplifiedGeometry = False, NoDriftTimes = False ):
    
    # set up the material locator
    if SimplifiedGeometry:
        defaultMaterialLocator = SimplifiedMaterialLocator()
    else:
        defaultMaterialLocator = DetailedMaterialLocator()
        
    # set up the master extrapolator
    defaultMasterExtrapolator = TrackMasterExtrapolator()
    if FieldOff:
        defaultExtraSelector = TrackSimpleExtraSelector()
        #    defaultMasterExtrapolator.ExtraSelector = "TrackSimpleExtraSelector"
        defaultMasterExtrapolator.addTool(defaultExtraSelector, name="ExtraSelector")
        defaultMasterExtrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
        defaultMasterExtrapolator.ApplyEnergyLossCorr = False
        defaultMasterExtrapolator.addTool(defaultMaterialLocator, name = MaterialLocator)

    # set up the NoDriftTimeProjector
    defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
    defaultOTNoDriftTimeProjector.UseDrift = False

    # set up the projector selectors. note: these take projectors from toolsvc!
    defaultProjectorSelector = TrackProjectorSelector("DefaultProjectorSelector")
    if NoDriftTimes:
        defaultProjectorSelector.OT = "TrajOTProjector/" + defaultOTNoDriftTimeProjector.name()
    prefitProjectorSelector = TrackProjectorSelector("PreFitProjectorSelector")
    prefitProjectorSelector.OT = "TrajOTProjector/" + defaultOTNoDriftTimeProjector.name()

    # set up the default NodeFitter and the prefit NodeFitter
    defaultNodeFitter = TrackKalmanFilter("DefaultNodeFitter")
    defaultNodeFitter.addTool(defaultProjectorSelector,name = "Projector")
    prefitNodeFitter = TrackKalmanFilter("PreFitNodeFitter")
    prefitNodeFitter.addTool(prefitProjectorSelector,name = "Projector")

    # set up the default MasterFitter and the prefit MasterFitter
    defaultMasterFitter = TrackMasterFitter("DefaultMasterFitter")
    defaultMasterFitter.addTool(defaultMasterExtrapolator,name="Extrapolator")
    defaultMasterFitter.addTool(defaultMaterialLocator,name="MaterialLocator")
    defaultMasterFitter.addTool(defaultNodeFitter,name="NodeFitter")

    prefitMasterFitter = defaultMasterFitter.clone("PreFitMasterFitter")
    prefitMasterFitter.addTool(prefitNodeFitter,name="NodeFitter")
    prefitMasterFitter.NumberFitIterations = 2
    prefitMasterFitter.MaxNumberOutliers = 0

    # now set of the default and prefit event fitter
    defaultTrackEventFitter = TrackEventFitter("DefaultTrackEventFitter")
    defaultTrackEventFitter.addTool(defaultMasterFitter,name = "Fitter")
    prefitTrackEventFitter = TrackEventFitter("PreFitTrackEventFitter")
    prefitTrackEventFitter.addTool(prefitMasterFitter,name = "Fitter")
    
    # now create all fitters
    configuredPreFitForward = prefitTrackEventFitter.clone("PreFitForward",TracksInContainer = "Rec/Track/Forward")
    configuredFitForward = defaultTrackEventFitter.clone("FitForward",TracksInContainer = "Rec/Track/Forward")

    configuredPreFitMatch = prefitTrackEventFitter.clone("PreFitMatch",TracksInContainer = "Rec/Track/Match")
    configuredFitMatch = defaultTrackEventFitter.clone("FitMatch",TracksInContainer = "Rec/Track/Match")

    configuredPreFitDownstream = prefitTrackEventFitter.clone("PreFitDownstream",TracksInContainer = "Rec/Track/Downstream")
    configuredPreFitDownstream.Fitter.ZPositions = [ 990., 2165., 9450. ]
    configuredFitDownstream = defaultTrackEventFitter.clone("FitDownstream",TracksInContainer = "Rec/Track/Downstream")
    configuredFitDownstream.Fitter.ZPositions = [ 990., 2165., 9450. ]

    configuredFitSeed = defaultTrackEventFitter.clone("FitSeed",TracksInContainer = "Rec/Track/Seed")
    configuredFitSeed.Fitter.StateAtBeamLine = False
    configuredFitSeed.Fitter.ZPositions = [ 7500., 9450., 11900. ]
    configuredFitSeed.Fitter.ErrorP = [0.04, 5e-08]

    configuredFitVeloTT = defaultTrackEventFitter.clone("FitVeloTT",TracksInContainer = "Rec/Track/VeloTT")
    configuredFitVeloTT.Fitter.ZPositions = [ 990., 2165. ]
    configuredFitVeloTT.Fitter.ErrorP = [1.2, 5e-07]

    configuredFitVelo = defaultTrackEventFitter.clone("FitVelo",TracksInContainer = "Rec/Track/PreparedVelo")
    configuredFitVelo.Fitter.ZPositions = []
    configuredFitVelo.Fitter.ErrorP= [0.01, 5e-08]

    # add them to the dictionary
    _fitterdictionary[configuredPreFitForward.name()]    = configuredPreFitForward
    _fitterdictionary[configuredFitForward.name()]       = configuredFitForward
    _fitterdictionary[configuredPreFitMatch.name()]      = configuredPreFitMatch
    _fitterdictionary[configuredFitMatch.name()]         = configuredFitMatch
    _fitterdictionary[configuredPreFitDownstream.name()] = configuredPreFitDownstream
    _fitterdictionary[configuredFitDownstream.name()]    = configuredFitDownstream
    _fitterdictionary[configuredFitSeed.name()]          = configuredFitSeed
    _fitterdictionary[configuredFitVeloTT.name()]        = configuredFitVeloTT
    _fitterdictionary[configuredFitVelo.name()]          = configuredFitVelo

    return _fitterdictionary
