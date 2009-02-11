from Gaudi import Configuration
from Configurables import HltTrackUpgradeTool, HltTrackFit
from Configurables import TrackHerabExtrapolator
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter

# why is this not automatically the case if RecoName == FitTrack ???
def setupHltFastTrackFit( fit ) :
    fit = Configuration.allConfigurables[fit]
    if fit.getType() is not 'HltTrackUpgrade' and fit.getType() is not 'HltVertexUpgrade' :
           raise AttributeError, "The argument '%s' is of type %s, should be either HltTrackUpgrade or HltVertexUpgrade"%(fit,fit.getType())
    if fit.RecoName != 'FitTrack' : 
           raise AttributeError, "The argument '%s' has RecoName %s, should be 'FitTrack'"%(fit,fit.RecoName)
    for i in [ HltTrackUpgradeTool(),HltTrackFit(),ConfiguredFastFitter('Fit')] :
        fit.addTool( i )
        fit = getattr( fit, i.getName().split('.')[-1] )
    # for good measure, we throw this one in as well...
    TrackHerabExtrapolator().extrapolatorID = 4
