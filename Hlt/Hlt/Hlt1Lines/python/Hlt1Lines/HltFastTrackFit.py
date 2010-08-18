from inspect import isclass
from Gaudi import Configuration

# why is this not automatically the case if RecoName == FitTrack ???
def setupHltFastTrackFit( fit ) :
    if type(fit) == str: fit = Configuration.allConfigurables[fit]
    if fit.getType() is not 'HltTrackUpgrade' and fit.getType() is not 'HltVertexUpgrade' :
           raise AttributeError, "The argument '%s' is of type %s, should be either HltTrackUpgrade or HltVertexUpgrade"%(fit,fit.getType())
    if fit.RecoName != 'FitTrack' : 
           raise AttributeError, "The argument '%s' has RecoName %s, should be 'FitTrack' when calling setupFastTrackFit"%(fit,fit.RecoName)
    from Configurables import HltTrackUpgradeTool, HltTrackFit
    for i in [ HltTrackUpgradeTool, HltTrackFit ] :
        fit.addTool( i )
        fit = getattr( fit, i.__name__ )
    from Configurables import TrackMasterFitter
    fit.addTool( TrackMasterFitter, name = 'Fit')
    from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
    fitter = ConfiguredFastFitter( getattr(fit,'Fit') )
    fitter.NodeFitter.BiDirectionalFit    = True
    fitter.NodeFitter.Smooth        = True
    #fitter.AddDefaultReferenceNodes = True
    fitter.NumberFitIterations = 1
    fitter.MaxNumberOutliers = 2
    fitter.UpdateTransport = False
