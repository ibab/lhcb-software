from TrackSys.Configuration import *
from TrackFitter.ConfiguredFitters import *

from Configurables import ( PatVeloTT, PatVeloTTTool,
                            TrackMasterFitter, TrackMasterExtrapolator,
                            TrackKalmanFilter, TrackSimpleExtraSelector,
                            VeloPixLiteMeasurementProvider)

patVTT = PatVeloTT("PatVeloPixTT")
### patVTT.InputUsedTracksNames = ["Rec/Track/Forward","Rec/Track/Match"]
patVTT.InputUsedTracksNames = []
print "call the tool from patvelopixtt"
patVTT.addTool(ConfiguredMasterFitter( "Fitter",
                            FieldOff = None,
                            SimplifiedGeometry = False,
                            NoDriftTimes       = None,
                            KalmanSmoother     = None,
                            LiteClusters       = True,
                            ApplyMaterialCorrections = None,
                            StateAtBeamLine = True,
                            MaxNumberOutliers = 2 ))
patVTT.Fitter.NumberFitIterations = 1
patVTT.Fitter.MaxNumberOutliers = 1
patVTT.Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
patVTT.Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, "ExtraSelector")
patVTT.Fitter.NodeFitter.addTool(TrackMasterExtrapolator, "Extrapolator")
patVTT.Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
patVTT.Fitter.StateAtBeamLine = False
patVTT.Fitter.MeasProvider.VeloPixProvider = VeloPixLiteMeasurementProvider()
patVTT.Fitter.MeasProvider.IgnoreVelo = True
patVTT.Fitter.MeasProvider.IgnoreVeloPix = False

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
