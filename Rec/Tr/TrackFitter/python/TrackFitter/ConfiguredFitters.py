
# ====================================================================
#  Track fitting options
# ====================================================================

from TrackSys.Configuration import TrackSys

from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackKalmanFilter,
                            TrackProjectorSelector, TrajOTProjector, TrackMasterExtrapolator,
                            TrackSimpleExtraSelector, SimplifiedMaterialLocator, DetailedMaterialLocator)

def ConfiguredFitter( Name = "DefaultEventFitter",
                      TracksInContainer = "Rec/Tracks/Best",
                      FieldOff = TrackSys().getProp( "fieldOff" ),
                      SimplifiedGeometry = "simplifiedGeometry" in TrackSys().getProp("expertTracking"),
                      NoDriftTimes =  "noDrifttimes" in TrackSys().getProp("expertTracking"),
                      KalmanSmoother = "kalmanSmoother" in TrackSys().getProp("expertTracking") ):

    # set up the material locator
    if SimplifiedGeometry:
        materialLocator = SimplifiedMaterialLocator()
    else:
        materialLocator = DetailedMaterialLocator()
        
    # set up the private extrapolator. must make sure name is 'unique'
    myExtrapolator = TrackMasterExtrapolator( Name + "_Extrapolator" )
    
    # this is a public tool, so you configure it with just a name
    myExtrapolator.MaterialLocator = materialLocator.name()
    
    if FieldOff:
        myExtrapolator.addTool(TrackSimpleExtraSelector(), name="ExtraSelector")
        myExtrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
        myExtrapolator.ApplyEnergyLossCorr = False

    myProjectorSelector = TrackProjectorSelector(Name + "_ProjectorSelector")
    if NoDriftTimes:
        # set up the NoDriftTimeProjector in the toolsvc
        defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
        defaultOTNoDriftTimeProjector.UseDrift = False
        myProjectorSelector.OT = "TrajOTProjector/" + defaultOTNoDriftTimeProjector.name()

    # set up the default NodeFitter and the prefit NodeFitter
    myNodeFitter = TrackKalmanFilter(Name + "_NodeFitter")
    myNodeFitter.addTool(myProjectorSelector,name = "Projector")

    if KalmanSmoother:
        myNodeFitter.BiDirectionalFit = False
        
    # set up the default MasterFitter and the prefit MasterFitter
    myMasterFitter = TrackMasterFitter(Name + "_MasterFitter")
    myMasterFitter.addTool(myNodeFitter,name="NodeFitter")
    #myMasterFitter.addTool(myExtrapolator, name = "Extrapolator" )
    myMasterFitter.Extrapolator = myExtrapolator.clone()
    myMasterFitter.MaterialLocator = materialLocator.name()

    # finally create the eventfitter
    eventfitter = TrackEventFitter(Name)
    eventfitter.addTool(myMasterFitter,name = "Fitter")
    eventfitter.TracksInContainer = TracksInContainer
    return eventfitter

def ConfiguredPrefitter( Name = "DefaultEventFitter",
                         TracksInContainer = "Rec/Tracks/Best",
                         FieldOff = TrackSys().getProp( "fieldOff" ),
                         SimplifiedGeometry = "simplifiedGeometry" in TrackSys().getProp("expertTracking")):
    eventfitter = ConfiguredFitter(Name,TracksInContainer,FieldOff,SimplifiedGeometry,NoDriftTimes=True)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.ErrorY2 = 10000
    return eventfitter

def ConfiguredFitVelo( Name = "FitVelo",
                       TracksInContainer = "Rec/Track/PreparedVelo" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.ZPositions = []
    eventfitter.Fitter.ErrorP= [0.01, 5e-08]
    eventfitter.Fitter.ErrorX2 = 100
    eventfitter.Fitter.ErrorY2 = 100
    eventfitter.Fitter.ErrorP= [0,0.01]
    return eventfitter

def ConfiguredFitVeloTT( Name = "FitVeloTT",
                         TracksInContainer = "Rec/Track/VeloTT" ):
    eventfitter = ConfiguredFitter(Name,TracksInContainer)
    eventfitter.Fitter.NumberFitIterations = 2
    eventfitter.Fitter.ZPositions = [ 990., 2165. ]
    eventfitter.Fitter.ErrorP = [1.2, 5e-07]
    eventfitter.Fitter.MaxNumberOutliers = 1
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
