from Gaudi import Configuration
from Configurables import HltTrackUpgradeTool, HltTrackFit, TrackMasterFitter
from Configurables import TrackHerabExtrapolator
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter

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
        fit.addTool( ConfiguredFastFitter( 'Fit' ) )
    TrackHerabExtrapolator().extrapolatorID = 4
