
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

def ConfiguredMasterFitter( Name,
                            FieldOff = TrackSys().fieldOff(),
                            SimplifiedGeometry = TrackSys().simplifiedGeometry(),
                            NoDriftTimes       = TrackSys().noDrifttimes(),
                            KalmanSmoother     = TrackSys().kalmanSmoother() ):

    # add the tools that need to be modified
    fitter = TrackMasterFitter(Name)
    fitter.addTool( TrackMasterExtrapolator(), name = "Extrapolator" )
    fitter.addTool( TrackKalmanFilter(), name = "NodeFitter" )

    # set up the material locator
    if SimplifiedGeometry:
        fitter.MaterialLocator = "SimplifiedMaterialLocator"
        fitter.Extrapolator.MaterialLocator = "SimplifiedMaterialLocator"

    # special settings for field-off
    if FieldOff:
        fitter.Extrapolator.addTool(TrackSimpleExtraSelector(), name="ExtraSelector")
        fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
        fitter.Extrapolator.ApplyEnergyLossCorr = False
        fitter.ApplyEnergyLossCorr = False

    # change the smoother
    if KalmanSmoother:
        fitter.NodeFitter.BiDirectionalFit = False

    # don't use drift times
    if NoDriftTimes:
        # set up the NoDriftTimeProjector in the toolsvc
        defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
        defaultOTNoDriftTimeProjector.UseDrift = False
        fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
        fitter.NodeFitter.Projector.OT = defaultOTNoDriftTimeProjector

    return fitter


def ConfiguredEventFitter( Name,
                           TracksInContainer,
                           FieldOff = TrackSys().fieldOff(),
                           SimplifiedGeometry = TrackSys().simplifiedGeometry(),
                           NoDriftTimes       = TrackSys().noDrifttimes(),
                           KalmanSmoother     = TrackSys().kalmanSmoother() ):
    
    # start with the event fitter
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    # add the tools that need to be modified
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredMasterFitter( fittername,
                                                 FieldOff,
                                                 SimplifiedGeometry,
                                                 NoDriftTimes,
                                                 KalmanSmoother ), name = "Fitter")
    return eventfitter

def ConfiguredPrefitter( Name = "DefaultEventFitter",
                         TracksInContainer = "Rec/Tracks/Best",
                         FieldOff = TrackSys().fieldOff(),
                         SimplifiedGeometry = TrackSys().simplifiedGeometry()):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,FieldOff,SimplifiedGeometry,NoDriftTimes=True)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.ErrorY2 = 10000
    return eventfitter

def ConfiguredFitVelo( Name = "FitVelo",
                       TracksInContainer = "Rec/Track/PreparedVelo",
                       FieldOff = True):
    # note that we ignore curvatue in velo. in the end that seems the
    # most sensible thing to do.
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
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
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.ZPositions = [ 990., 2165. ]
    eventfitter.Fitter.ErrorP = [1.2, 5e-07]
    eventfitter.Fitter.MaxNumberOutliers = 1
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    return eventfitter

def ConfiguredFitSeed( Name = "FitSeed",
                       TracksInContainer = "Rec/Track/Seed" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.ZPositions = [ 7500., 9450., 11900. ]
    eventfitter.Fitter.ErrorP = [0.04, 5e-08]
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter

def ConfiguredFitForward( Name = "FitForward",
                         TracksInContainer = "Rec/Track/Forward" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter

def ConfiguredFitMatch( Name = "FitMatch",
                        TracksInContainer = "Rec/Track/Match" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    return eventfitter
 
def ConfiguredFitDownstream( Name = "FitDownstream",
                             TracksInContainer = "Rec/Track/Downstream" ):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer)
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

def ConfiguredFastFitter( Name ):
    
    fitter = ConfiguredMasterFitter(Name, SimplifiedGeometry = True)
    fitter.NumberFitIterations = 1
    fitter.MaxNumberOutliers = 0
    fitter.NodeFitter.BiDirectionalFit = False
    fitter.NodeFitter.Smooth = False
    # at some point, need to switch to analytic evaluation
    # TrackHerabExtrapolator().extrapolatorID = 4
    return fitter


def ConfiguredFastEventFitter( Name, TracksInContainer ):
    eventfitter = TrackEventFitter(Name)
    eventfitter.TracksInContainer = TracksInContainer
    fittername = Name + ".Fitter"
    eventfitter.addTool( ConfiguredFastMasterFitter( Name = fittername ), name = "Fitter")
    return eventfitter


def ConfiguredFastVeloOnlyEventFitter( Name, TracksInContainer ):
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
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,FieldOff=True)
    eventfitter.Fitter.ApplyMaterialCorrections = False
    eventfitter.Fitter.Extrapolator.ApplyMultScattCorr = False
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    eventfitter.Fitter.Extrapolator.ApplyElectronEnergyLossCorr = False
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.ZPositions = [ ]
    return eventfitter

