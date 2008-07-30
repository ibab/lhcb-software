
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

from TrackSys.Configuration import *
 
# Forward fit
TrackEventFitter("FitForward").TracksInContainer = "Rec/Track/Forward"
TrackEventFitter("FitForward").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitForward").Fitter.NumberFitIterations = 2
TrackEventFitter("FitForward").Fitter.MakeNodes = False
TrackEventFitter("FitForward").Fitter.addTool(TrackMasterExtrapolator,name="Extrapolator")
TrackEventFitter("FitForward").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("FitForward").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if TrackSys().getProp( "fieldOff" ):
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
    TrackEventFitter("FitForward").Fitter.Extrapolator.addTool(materialLocator)
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.addTool(materialLocator)
    
if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitForward").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("FitForward").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("FitForward").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("FitForward").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("FitForward").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"

if "noDrifttimes" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitForward").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
    TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False


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
TrackEventFitter("PreFitForward").Fitter.addTool(TrackMasterExtrapolator,name="Extrapolator")
TrackEventFitter("PreFitForward").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("PreFitForward").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("PreFitForward").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("PreFitForward").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("PreFitForward").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("PreFitForward").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("PreFitForward").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"




# Seed fit
TrackEventFitter("FitSeed").TracksInContainer          = "Rec/Track/Seed"
TrackEventFitter("FitSeed").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitSeed").Fitter.ZPositions          = [ 7500., 9450., 11900. ]
TrackEventFitter("FitSeed").Fitter.StateAtBeamLine     = False
TrackEventFitter("FitSeed").Fitter.ErrorP              = [ 0.04, 5e-8]
TrackEventFitter("FitSeed").Fitter.NumberFitIterations = 2
TrackEventFitter("FitSeed").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("FitSeed").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
TrackEventFitter("FitSeed").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("FitSeed").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if TrackSys().getProp("fieldOff") :
   TrackEventFitter("FitSeed").Fitter.Extrapolator.ApplyEnergyLossCorr = False;
   TrackEventFitter("FitSeed").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
   TrackEventFitter("FitSeed").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";
   TrackEventFitter("FitSeed").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
   TrackEventFitter("FitSeed").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitSeed").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("FitSeed").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("FitSeed").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("FitSeed").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("FitSeed").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("FitSeed").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"

if "noDrifttimes" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitSeed").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
    TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False

      
# Match fit
TrackEventFitter("FitMatch").TracksInContainer = "Rec/Track/Match"
TrackEventFitter("FitMatch").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitMatch").Fitter.NumberFitIterations = 2
TrackEventFitter("FitMatch").Fitter.MakeNodes = False
TrackEventFitter("FitMatch").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("FitMatch").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("FitMatch").Fitter.addTool(TrackMasterExtrapolator,name="Extrapolator")

if TrackSys().getProp("fieldOff") :    
    TrackEventFitter("FitMatch").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
    TrackEventFitter("FitMatch").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
    TrackEventFitter("FitMatch").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
    TrackEventFitter("FitMatch").Fitter.Extrapolator.ApplyEnergyLossCorr = False
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitMatch").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("FitMatch").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("FitMatch").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("FitMatch").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"

if "noDrifttimes" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitMatch").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
    TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False
    
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
TrackEventFitter("PreFitMatch").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("PreFitMatch").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("PreFitMatch").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("PreFitMatch").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("PreFitMatch").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("PreFitMatch").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("PreFitMatch").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("PreFitMatch").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"

# Downstream fit
TrackEventFitter("FitDownstream").TracksInContainer = "Rec/Track/Downstream"
TrackEventFitter("FitDownstream").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitDownstream").Fitter.ZPositions      = [ 990., 2165., 9450. ]
TrackEventFitter("FitDownstream").Fitter.NumberFitIterations = 2
TrackEventFitter("FitDownstream").Fitter.MakeNodes = False
TrackEventFitter("FitDownstream").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("FitDownstream").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("FitDownstream").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitDownstream").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("FitDownstream").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("FitDownstream").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("FitDownstream").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("FitDownstream").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("FitDownstream").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"

if "noDrifttimes" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitDownstream").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
    TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False

TrackEventFitter("PreFitDownstream").TracksInContainer = "Rec/Track/Downstream"
TrackEventFitter("PreFitDownstream").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("PreFitDownstream").Fitter.ZPositions      = [ 990., 2165., 9450. ]
TrackEventFitter("PreFitDownstream").Fitter.NumberFitIterations = 2
TrackEventFitter("PreFitDownstream").Fitter.MaxNumberOutliers = 0
TrackEventFitter("PreFitDownstream").Fitter.ErrorX2 = 40.0
TrackEventFitter("PreFitDownstream").Fitter.ErrorTx2 = 1e-3
TrackEventFitter("PreFitDownstream").Fitter.ErrorTy2 = 2e-3
TrackEventFitter("PreFitDownstream").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("PreFitDownstream").Fitter.NodeFitter.addTool(TrackProjectorSelector, name="Projector")
TrackEventFitter("PreFitDownstream").Fitter.NodeFitter.Projector.OT = "TrajOTProjector/OTNoDrifttimesProjector"
TrajOTProjector("OTNoDrifttimesProjector").UseDrift = False
TrackEventFitter("PreFitDownstream").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("PreFitDownstream").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("PreFitDownstream").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("PreFitDownstream").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("PreFitDownstream").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("PreFitDownstream").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("PreFitDownstream").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("PreFitDownstream").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    

# Velo-TT fit
TrackEventFitter("FitVeloTT").TracksInContainer = "Rec/Track/VeloTT"
TrackEventFitter("FitVeloTT").addTool( TrackMasterFitter, name = "Fitter" )
TrackEventFitter("FitVeloTT").Fitter.ZPositions          = [ 990., 2165. ]
TrackEventFitter("FitVeloTT").Fitter.ErrorP              = [ 1.2, 5e-7]
TrackEventFitter("FitVeloTT").Fitter.NumberFitIterations = 2
TrackEventFitter("FitVeloTT").Fitter.MaxNumberOutliers   = 1
TrackEventFitter("FitVeloTT").Fitter.addTool(TrackMasterExtrapolator, name="Extrapolator")
TrackEventFitter("FitVeloTT").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
TrackEventFitter("FitVeloTT").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("FitVeloTT").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, name="Extrapolator")

if TrackSys().getProp( "fieldOff") :
   TrackEventFitter("FitVeloTT").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
   TrackEventFitter("FitVeloTT").Fitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator"
   TrackEventFitter("FitVeloTT").Fitter.Extrapolator.ApplyEnergyLossCorr = False
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.ApplyEnergyLossCorr = False;
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.addTool(TrackSimpleExtraSelector, name="ExtraSelector")
   TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.ExtraSelector.ExtrapolatorName = "TrackLinearExtrapolator";  

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitVeloTT").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("FitVeloTT").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("FitVeloTT").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("FitVeloTT").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("FitVeloTT").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"


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
TrackEventFitter("FitVelo").Fitter.addTool(TrackKalmanFilter, name="NodeFitter")
TrackEventFitter("FitVelo").Fitter.NodeFitter.addTool(TrackMasterExtrapolator,name="Extrapolator")

if "simplifiedGeometry" in TrackSys().getProp("expertTracking"):
    TrackEventFitter("FitVelo").Fitter.MaterialLocator = "SimplifiedMaterialLocator"
    TrackEventFitter("FitVelo").Fitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
    TrackEventFitter("FitVelo").Fitter.NodeFitter.Extrapolator.MaterialLocator="SimplifiedMaterialLocator"
else:
    TrackEventFitter("FitVelo").Fitter.MaterialLocator = "DetailedMaterialLocator"
    TrackEventFitter("FitVelo").Fitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
    TrackEventFitter("FitVelo").Fitter.NodeFitter.Extrapolator.MaterialLocator="DetailedMaterialLocator"
