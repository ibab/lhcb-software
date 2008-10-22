from TrackSys.Configuration import *

from Configurables import ( PatVeloTT, PatVeloTTTool,
                            TrackMasterFitter, TrackMasterExtrapolator,
                            TrackKalmanFilter, TrackSimpleExtraSelector )

patVTT = PatVeloTT("PatVeloTT")
patVTT.InputUsedTracksNames = ["Rec/Track/Forward","Rec/Track/Match"]
patVTT.addTool(TrackMasterFitter, name="Fitter")
patVTT.Fitter.ZPositions        = [ 990.0, 2165.0 ]
patVTT.Fitter.ErrorP            = [1.2, 5e-8]
patVTT.Fitter.NumberFitIterations = 1
patVTT.Fitter.MaxNumberOutliers = 1
patVTT.Fitter.addTool(TrackMasterExtrapolator, "Extrapolator")
patVTT.Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
patVTT.Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, "ExtraSelector")
patVTT.Fitter.addTool(TrackKalmanFilter, "NodeFitter")
patVTT.Fitter.NodeFitter.addTool(TrackMasterExtrapolator, "Extrapolator")
patVTT.Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
patVTT.Fitter.StateAtBeamLine = False

patVTT.addTool(PatVeloTTTool, name="PatVeloTTTool")
if TrackSys().fieldOff():
   patVTT.PatVeloTTTool.minMomentum = 5000.
   patVTT.PatVeloTTTool.maxPseudoChi2 = 256
   patVTT.maxChi2 = 256.
   patVTT.PatVeloTTTool.DxGroupFactor = 0.0
   patVTT.fitTracks = False
else:
   patVTT.PatVeloTTTool.minMomentum = 800.
   patVTT.PatVeloTTTool.maxPseudoChi2 = 10000.
   patVTT.maxChi2 = 5.
