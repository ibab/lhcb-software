
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
                            TrackSimpleExtraSelector)

from Brunel.Configuration import *

# Forward fit
TrackEventFitter("FitForward").TracksInContainer = "Rec/Track/Forward"
TrackEventFitter("FitForward").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitForward").Fitter.NumberFitIterations = 2
TrackEventFitter("FitForward").Fitter.MakeNodes = False
if Brunel().getProp( "fieldOff" ) :
    TrackEventFitter("FitForward").Fitter.addTool(TrackMasterExtrapolator,name="Extrapolator")
    TrackEventFitter("FitForward").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
    TrackEventFitter("FitForward").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
    TrackEventFitter("FitForward").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
    TrackEventFitter("FitForward").Fitter.Extrapolator.ApplyEnergyLossCorr = False
    TrackEventFitter("FitForward").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
    TrackEventFitter("FitForward").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";  

TrackEventFitter("PreFitForward").TracksInContainer = "Rec/Track/Forward"
TrackEventFitter("PreFitForward").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("PreFitForward").Fitter.NumberFitIterations = 2
TrackEventFitter("PreFitForward").Fitter.MaxNumberOutliers = 0
TrackEventFitter("PreFitForward").Fitter.ErrorX2 = 40.0
TrackEventFitter("PreFitForward").Fitter.ErrorTx2 = 1e-3
TrackEventFitter("PreFitForward").Fitter.ErrorTy2 = 2e-3
TrackEventFitter("PreFitForward").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("PreFitForward").Fitter.NodeFitter.addTool(TrackProjectorSelector, name="Projector")
TrackEventFitter("PreFitForward").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False

# Seed fit
TrackEventFitter("FitSeed").TracksInContainer          = "Rec/Track/Seed"
TrackEventFitter("FitSeed").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitSeed").Fitter.ZPositions          = [ 7500., 9450., 11900. ]
TrackEventFitter("FitSeed").Fitter.StateAtBeamLine     = False
TrackEventFitter("FitSeed").Fitter.ErrorP              = [ 0.04, 5e-8]
TrackEventFitter("FitSeed").Fitter.NumberFitIterations = 2
TrackEventFitter("FitSeed").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("FitSeed").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
if Brunel().getProp("fieldOff") :
   TrackEventFitter("FitSeed").Fitter.Extrapolator.ApplyEnergyLossCorr = False;
   TrackEventFitter("FitSeed").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
   TrackEventFitter("FitSeed").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";
   TrackEventFitter("FitSeed").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
   TrackEventFitter("FitSeed").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
   TrackEventFitter("FitSeed").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
   TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";  
   
# Match fit
TrackEventFitter("FitMatch").TracksInContainer = "Rec/Track/Match"
TrackEventFitter("FitMatch").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitMatch").Fitter.NumberFitIterations = 2
TrackEventFitter("FitMatch").Fitter.MakeNodes = False
if Brunel().getProp("fieldOff") :
    TrackEventFitter("FitMatch").Fitter.addTool(TrackMasterExtrapolator,name="Extrapolator")
    TrackEventFitter("FitMatch").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
    TrackEventFitter("FitMatch").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
    TrackEventFitter("FitMatch").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
    TrackEventFitter("FitMatch").Fitter.Extrapolator.ApplyEnergyLossCorr = False
    TrackEventFitter("FitMatch").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
    TrackEventFitter("FitMatch").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";  

TrackEventFitter("PreFitMatch").TracksInContainer = "Rec/Track/Match"
TrackEventFitter("PreFitMatch").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("PreFitMatch").Fitter.NumberFitIterations = 2
TrackEventFitter("PreFitMatch").Fitter.MaxNumberOutliers = 0
TrackEventFitter("PreFitMatch").Fitter.ErrorX2 = 40.0
TrackEventFitter("PreFitMatch").Fitter.ErrorTx2 = 1e-3
TrackEventFitter("PreFitMatch").Fitter.ErrorTy2 = 2e-3
TrackEventFitter("PreFitMatch").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("PreFitMatch").Fitter.NodeFitter.addTool(TrackProjectorSelector, name="Projector")
TrackEventFitter("PreFitMatch").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False

# Downstream fit
TrackEventFitter("FitDownstream").TracksInContainer = "Rec/Track/Downstream"
TrackEventFitter("FitDownstream").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitDownstream").Fitter.ZPositions      = [ 990., 2165., 9450. ]
TrackEventFitter("FitDownstream").Fitter.NumberFitIterations = 2
TrackEventFitter("FitDownstream").Fitter.MakeNodes = False

TrackEventFitter("PreFitDownstream").TracksInContainer = "Rec/Track/Downstream"
TrackEventFitter("PreFitDownStream").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("PreFitDownStream").Fitter.ZPositions      = [ 990., 2165., 9450. ]
TrackEventFitter("PreFitDownStream").Fitter.NumberFitIterations = 2
TrackEventFitter("PreFitDownStream").Fitter.MaxNumberOutliers = 0
TrackEventFitter("PreFitDownStream").Fitter.ErrorX2 = 40.0
TrackEventFitter("PreFitDownStream").Fitter.ErrorTx2 = 1e-3
TrackEventFitter("PreFitDownStream").Fitter.ErrorTy2 = 2e-3
TrackEventFitter("PreFitDownStream").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("PreFitDownStream").Fitter.NodeFitter.addTool(TrackProjectorSelector, name="Projector")
TrackEventFitter("PreFitDownStream").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False

# Velo-TT fit
TrackEventFitter("FitVeloTT").TracksInContainer = "Rec/Track/VeloTT"
TrackEventFitter("FitVeloTT").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitVeloTT").Fitter.ZPositions          = [ 990., 2165. ]
TrackEventFitter("FitVeloTT").Fitter.ErrorP              = [ 1.2, 5e-7]
TrackEventFitter("FitVeloTT").Fitter.NumberFitIterations = 2
TrackEventFitter("FitVeloTT").Fitter.MaxNumberOutliers   = 1
TrackEventFitter("FitVeloTT").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("FitVeloTT").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
if Brunel().getProp( "fieldOff") :
   TrackEventFitter("FitVeloTT").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
   TrackEventFitter("FitVeloTT").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
   TrackEventFitter("FitVeloTT").Fitter.Extrapolator.ApplyEnergyLossCorr = False
   TrackEventFitter("FitVeloTT").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";  


# Velo prefit and fit
TrackEventFitter("FitVelo").TracksInContainer = "Rec/Track/PreparedVelo"
TrackEventFitter("FitVelo").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitVelo").Fitter.ZPositions          = []
TrackEventFitter("FitVelo").Fitter.ErrorP              = [0.01,5e-8]
TrackEventFitter("FitVelo").Fitter.NumberFitIterations = 2
TrackEventFitter("FitVelo").Fitter.addTool(TrackMasterExtrapolator,name="Extrapolator")
TrackEventFitter("FitVelo").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
TrackEventFitter("FitVelo").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
TrackEventFitter("FitVelo").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
TrackEventFitter("FitVelo").Fitter.Extrapolator.ApplyEnergyLossCorr = False
