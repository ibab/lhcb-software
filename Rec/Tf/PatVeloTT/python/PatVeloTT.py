from Brunel.Configuration import *

from Configurables import ( PatVeloTT, PatVeloTTTool )

PatVeloTT("PatVeloTT").InputUsedTracksNames = ["Rec/Track/Forward","Rec/Track/Match"]
PatVeloTT("PatVeloTT").addTool(TrackMasterFitter, name="Fitter")
PatVeloTT("PatVeloTT").Fitter.ZPositions        = [ 990.0, 2165.0 ];
PatVeloTT("PatVeloTT").Fitter.ErrorP            = [1.2, 5e-8];
PatVeloTT("PatVeloTT").Fitter.NumberFitIterations = 1
PatVeloTT("PatVeloTT").Fitter.MaxNumberOutliers = 1
PatVeloTT("PatVeloTT").Fitter.addTool(TrackMasterExtrapolator, "Extrapolator")
PatVeloTT("PatVeloTT").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
PatVeloTT("PatVeloTT").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, "ExtraSelector")
PatVeloTT("PatVeloTT").Fitter.addTool(TrackKalmanFilter, "NodeFitter")
PatVeloTT("PatVeloTT").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, "Extrapolator")
PatVeloTT("PatVeloTT").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
PatVeloTT("PatVeloTT").Fitter.StateAtBeamLine = False;

PatVeloTT("PatVeloTT").addTool(PatVeloTTTool, name="PatVeloTTTool")
if Brunel().getProp("fieldOff"):
   PatVeloTT("PatVeloTT").PatVeloTTTool.minMomentum = 5000.;
   PatVeloTT("PatVeloTT").PatVeloTTTool.maxPseudoChi2 = 256;
   PatVeloTT("PatVeloTT").maxChi2 = 256.;
   PatVeloTT("PatVeloTT").PatVeloTTTool.DxGroupFactor = 0.0;
   PatVeloTT("PatVeloTT").fitTracks = False;
else:
   PatVeloTT("PatVeloTT").PatVeloTTTool.minMomentum = 800.;
   PatVeloTT("PatVeloTT").PatVeloTTTool.maxPseudoChi2 = 10000.;
   PatVeloTT("PatVeloTT").maxChi2 = 5.;
