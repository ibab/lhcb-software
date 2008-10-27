
# ====================================================================
#  Track fitting options
# ====================================================================

from TrackSys.Configuration import TrackSys

from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackKalmanFilter,
                            TrackProjectorSelector, TrajOTProjector, TrackMasterExtrapolator,
                            TrackSimpleExtraSelector, TrackDistanceExtraSelector,
                            TrackHerabExtrapolator,
                            SimplifiedMaterialLocator, DetailedMaterialLocator,
                            MeasurementProvider)

def ConfiguredFitter( Name = "DefaultEventFitter",
                      TracksInContainer = "Rec/Tracks/Best",
                      FieldOff = TrackSys().fieldOff(),
                      SimplifiedGeometry = TrackSys().simplifiedGeometry(),
                      NoDriftTimes = TrackSys().noDrifttimes(),
                      KalmanSmoother = TrackSys().kalmanSmoother() ):

    # start with the event fitter
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer

    # add the tools that need to be modified
    eventfitter.addTool(TrackMasterFitter(),name = "Fitter")

    eventfitter.Fitter.addTool( TrackMasterExtrapolator(), name = "Extrapolator" )
    eventfitter.Fitter.addTool( TrackKalmanFilter(), name = "NodeFitter" )

    # set up the material locator
    if SimplifiedGeometry:
        eventfitter.Fitter.MaterialLocator = "SimplifiedMaterialLocator"
        eventfitter.Fitter.Extrapolator.MaterialLocator = "SimplifiedMaterialLocator"

    # special settings for field-off
    if FieldOff:
        eventfitter.Fitter.Extrapolator.addTool(TrackSimpleExtraSelector(), name="ExtraSelector")
        eventfitter.Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
        eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
        eventfitter.Fitter.ApplyEnergyLossCorr = False

    # change the smoother
    if KalmanSmoother:
        eventfitter.Fitter.NodeFitter.BiDirectionalFit = False

    # don't use drift times
    if NoDriftTimes:
        # set up the NoDriftTimeProjector in the toolsvc
        defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
        defaultOTNoDriftTimeProjector.UseDrift = False
        eventfitter.Fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
        eventfitter.Fitter.NodeFitter.Projector.OT = defaultOTNoDriftTimeProjector

    return eventfitter

def ConfiguredPrefitter( Name = "DefaultEventFitter",
                         TracksInContainer = "Rec/Tracks/Best",
                         FieldOff = TrackSys().fieldOff(),
                         SimplifiedGeometry = TrackSys().simplifiedGeometry() ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer,FieldOff,SimplifiedGeometry,NoDriftTimes=True)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.ErrorY2 = 10000
    return eventfitter

def ConfiguredFitVelo( Name = "FitVelo",
                       TracksInContainer = "Rec/Track/PreparedVelo",
                       FieldOff = True):
    # note that we ignore curvatue in velo. in the end that seems the
    # most sensible thing to do.
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.ZPositions = []
    #eventfitter.Fitter.ErrorP= [0.01, 5e-08]
    #eventfitter.Fitter.ErrorX2 = 100
    #eventfitter.Fitter.ErrorY2 = 100
    #eventfitter.Fitter.ErrorP= [0,0.01]
    #eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    return eventfitter

def ConfiguredFitVeloTT( Name = "FitVeloTT",
                         TracksInContainer = "Rec/Track/VeloTT" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.ZPositions = [ 990., 2165. ]
    eventfitter.Fitter.ErrorP = [1.2, 5e-07]
    eventfitter.Fitter.MaxNumberOutliers = 1
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    return eventfitter

def ConfiguredFitSeed( Name = "FitSeed",
                       TracksInContainer = "Rec/Track/Seed" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.ZPositions = [ 7500., 9450., 11900. ]
    eventfitter.Fitter.ErrorP = [0.04, 5e-08]
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter

def ConfiguredFitForward( Name = "FitForward",
                         TracksInContainer = "Rec/Track/Forward" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter

def ConfiguredFitMatch( Name = "FitMatch",
                        TracksInContainer = "Rec/Track/Match" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter
 
def ConfiguredFitDownstream( Name = "FitDownstream",
                             TracksInContainer = "Rec/Track/Downstream" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.ZPositions = [ 990., 2165., 9450. ]
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter

def ConfiguredPreFitForward( Name = "PreFitForward",
                             TracksInContainer = "Rec/Track/Forward" ):
    return ConfiguredPrefitter(Name,TracksInContainer)

def ConfiguredPreFitMatch( Name = "PreFitMatch",
                           TracksInContainer = "Rec/Track/Match" ):
    return ConfiguredPrefitter(Name,TracksInContainer)

def ConfiguredPreFitDownstream( Name = "PreFitDownstream",
                                TracksInContainer = "Rec/Track/Downstream" ):
    eventfitter = ConfiguredPrefitter(Name,TracksInContainer)
    eventfitter.Fitter.ZPositions = [ 990., 2165., 9450. ]
    return eventfitter

def ConfiguredFastFit( Name, TracksInContainer ):
    FieldOff = TrackSys().getProp( "fieldOff" )
    eventfitter = ConfiguredFitter(Name,TracksInContainer,
                                   FieldOff,
                                   SimplifiedGeometry = True)
    eventfitter.Fitter.NumberFitIterations = 1
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.NodeFitter.BiDirectionalFit = False
    eventfitter.Fitter.NodeFitter.Smooth = False
    # at some point, need to switch to analytic evaluation
    # TrackHerabExtrapolator().extrapolatorID = 4
    return eventfitter

def ConfiguredFastVeloOnlyFit( Name, TracksInContainer ):
    eventfitter = ConfiguredFastFit( Name, TracksInContainer )
    eventfitter.Fitter.ZPositions = []
    eventfitter.Fitter.ErrorP = [0,1e-8] # choose something very small such that momentum not changed
    eventfitter.Fitter.ApplyEnergyLossCorr = False
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    eventfitter.Fitter.Extrapolator.ApplyElectronEnergyLossCorr = False
    eventfitter.Fitter.addTool( MeasurementProvider(), name = 'MeasProvider')
    eventfitter.Fitter.MeasProvider.IgnoreIT = True
    eventfitter.Fitter.MeasProvider.IgnoreOT = True
    eventfitter.Fitter.MeasProvider.IgnoreTT = True
    eventfitter.Fitter.MeasProvider.IgnoreMuon = True
    return eventfitter

def ConfiguredStraightLineFit( Name, TracksInContainer ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer,FieldOff=True)
    eventfitter.Fitter.ApplyMaterialCorrections = False
    eventfitter.Fitter.Extrapolator.ApplyMultScattCorr = False
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    eventfitter.Fitter.Extrapolator.ApplyElectronEnergyLossCorr = False
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.ZPositions = [ ]
    return eventfitter

