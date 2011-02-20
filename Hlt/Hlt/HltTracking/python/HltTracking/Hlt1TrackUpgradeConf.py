from Gaudi import Configuration
from Configurables import ( PatSeedingTool,
                            PatForwardTool, 
                            MatchVeloMuon, HltTrackFit )
                            
from HltLine.HltLine import Hlt1Tool

__all__ = ( 'ConfiguredPatSeeding',
            'ConfiguredForward',
            'ConfiguredFastKalman',
            'ConfiguredMatchVeloMuon' )
            
def ConfiguredPatSeeding( minPSeed = 3000):
    # Add the option to define a minimum PT/P 
    # for the tracking to consider
    # Only relevant for the forward upgrade
    return Hlt1Tool( PatSeedingTool, MinMomentum = minPSeed )
        
_minPt =  800  #### MOVE TO 800 (used to be 500 )
_minP  = 8000 #### MOVE TO 8000 (used to be 5000 )
def ConfiguredForward( parent, name = None, minP = _minP, minPt = _minPt ) :
    if name == None: name = PatForwardTool.__name__
    return Hlt1Tool( PatForwardTool
                     , name
                     , SecondLoop = True
                     , MaxChi2 = 40
                     , MaxChi2Track = 40
                     , MinHits = 12
                     , MinOTHits = 14
                     , MinPt = minPt
                     , MinMomentum = minP ).createConfigurable( parent )
            
def ConfiguredFastKalman( parent = None, name = None ) :
    if name == None: name = HltTrackFit.__name__
    if parent :
        parent.addTool( HltTrackFit, name )
        parent = getattr( parent, name )
    else :  # make a public instance...
        parent = HltTrackFit()
    from Configurables import TrackMasterFitter
    parent.addTool( TrackMasterFitter, name = 'Fit')
    from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
    fitter = ConfiguredHltFitter( getattr(parent,'Fit') )

def ConfiguredMatchVeloMuon( parent, name = None, minP = _minP ) :
    if name == None: name = MatchVeloMuon.__name__
    return Hlt1Tool( MatchVeloMuon
                     , name
                     , MaxChi2DoFX = 10
                     , XWindow = 200
                     , YWindow = 200
                     ## Set this according to PatForward
                     , MinMomentum = minP - 500 ).createConfigurable( parent )

