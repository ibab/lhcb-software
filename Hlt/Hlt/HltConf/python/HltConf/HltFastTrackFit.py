from Gaudi import Configuration
from Configurables import HltTrackUpgradeTool, HltTrackFit, TrackMasterFitter
from Configurables import MeasurementProvider,TrackKalmanFilter, TrackMasterExtrapolator, TrackDistanceExtraSelector

# why is this not automatically the case if RecoName == FitTrack ???
def setupHltFastTrackFit( fit ) :
    fit = Configuration.allConfigurables[fit]
    if fit.getType() is 'HltTrackUpgrade' or fit.getType() is 'HltVertexUpgrade' :
        if getattr(fit,'RecoName') != 'FitTrack' : print 'huh?'
        fit.addTool(HltTrackUpgradeTool())
        fit = getattr(fit,'HltTrackUpgradeTool')
    if fit.getType() is 'HltTrackUpgradeTool' :
        fit.addTool(HltTrackFit())
        fit = getattr(fit,'HltTrackFit')
    if fit.getType() is 'HltTrackFit' :
        fit.addTool( TrackMasterFitter('Fit') )
        fit = getattr(fit,'Fit')

    for k,v in { 'MaxNumberOutliers'   : 0
               , 'NumberFitIterations' : 1
               , 'MaterialLocator'     : 'SimplifiedMaterialLocator'
               , 'ZPositions'          : [] 
               }.iteritems() :
       setattr(fit,k,v)

    fit.addTool( TrackKalmanFilter('NodeFitter', BiDirectionalFit = False, Smooth = False ) )
    fit.addTool( TrackMasterExtrapolator('Extrapolator', MaterialLocator = fit.MaterialLocator ) )
