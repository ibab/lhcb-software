from TrackSys.Configuration import *
from TrackFitter.ConfiguredFitters import *

from Configurables import ( PatVeloTT, PatVeloTTTool,
                            TrackMasterFitter, TrackMasterExtrapolator,
                            TrackKalmanFilter, TrackSimpleExtraSelector )

class PatVeloTTConf(object):
   def configureAlg(self, patVTT = PatVeloTT("PatVeloTT")):
      
      patVTT.InputUsedTracksNames = ["Rec/Track/Forward","Rec/Track/Match"]
      
      patVTT.addTool(TrackMasterFitter,"Fitter")
      patVTT.addTool(PatVeloTTTool, name="PatVeloTTTool")
      
      self.configureFitTool(patVTT.Fitter)
      self.configureVTTTool(patVTT.PatVeloTTTool)
      
      if TrackSys().fieldOff():
         patVTT.maxChi2 = 256.
         patVTT.fitTracks = False
      else:
         patVTT.maxChi2 = 5.
   
      
   def configureFitTool(self, fitTool):
      
      ConfiguredMasterFitter(fitTool)
      fitTool.NumberFitIterations = 1
      fitTool.MaxNumberOutliers = 1
      fitTool.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
      fitTool.StateAtBeamLine = False

   def configureVTTTool(self, VTTTool):
      
      if TrackSys().fieldOff():
         VTTTool.minMomentum = 5000.
         VTTTool.maxPseudoChi2 = 256
         VTTTool.DxGroupFactor = 0.0
      else:
         VTTTool.minMomentum = 800.
         VTTTool.maxPseudoChi2 = 10000.

