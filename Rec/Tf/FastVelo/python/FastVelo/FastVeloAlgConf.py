from TrackSys.Configuration import *
from Configurables import FastVeloTracking
from Configurables import TrackContainerCopy
from Configurables import TrackStateInitAlg, TrackStateInitTool, FastVeloFitLHCbIDs, TrackMasterExtrapolator, SimplifiedMaterialLocator

class FastVeloKalmanConf(object):
  '''Configure the fast Kalman in FastVelo'''
  
  def configureFastKalmanFit(self, init = TrackStateInitAlg("VeloOnlyInitAlg")):
    '''Configure the fast Kalman fit to fit the Velo tracks for the PV'''
    
    # fit with fast Kalman filter
    init.TrackLocation =  "Rec/Track/FittedHLT1VeloTracks"
    init.addTool(TrackStateInitTool, name="StateInitTool")
    init.StateInitTool.VeloFitterName = "FastVeloFitLHCbIDs"
    init.StateInitTool.addTool(TrackMasterExtrapolator, "Extrapolator")
    init.StateInitTool.Extrapolator.addTool(SimplifiedMaterialLocator,
                                            name = "MaterialLocator")
    init.StateInitTool.addTool(FastVeloFitLHCbIDs,name="FastVeloFitLHCbIDs")
    init.StateInitTool.FastVeloFitLHCbIDs.UseKalmanFit = True

    
  
        
 

