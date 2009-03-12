from inspect import isclass
from Gaudi import Configuration

# why is this not automatically the case if RecoName == FitTrack ???
def setupHltFastTrackFit( name ) :
    fit = Configuration.allConfigurables[name]
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
    ConfiguredFastFitter( getattr(fit,'Fit') )
    # for good measure, we throw this one in as well...
    from Configurables import TrackHerabExtrapolator
    TrackHerabExtrapolator().extrapolatorID = 4
