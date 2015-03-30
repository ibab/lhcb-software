__all__ = ( 'FilterVeloL0Calo', 'MatchVeloL0CaloCands' , 'FilterTrackL0Calo' )
# =============================================================================
## Symbols for streamer users
# =============================================================================
import HltLine.HltDecodeRaw
from Configurables import Hlt__MatchVeloL0Calo, ToolSvc
from Configurables import Hlt__Track2L0CaloMatch
from HltLine.HltLine import Hlt1Tool

## Helper functions
from Hlt1TrackNames import Hlt1Tools
def to_name( key ):
    return Hlt1Tools[key]

def ConfiguredVeloL0Calo( parent, name = None, chi2 = 9 ):
    if name == None: name = Hlt__MatchVeloL0Calo.__name__
    return Hlt1Tool( Hlt__MatchVeloL0Calo
                     , name
                     , MaxMatchChi2 = chi2 ).createConfigurable( parent )

## Configure tool, set the match chi2
ConfiguredVeloL0Calo( ToolSvc(), to_name( "VeloL0Calo" ), 9 )

## Streamer symbol
def FilterVeloL0Calo( selection ):
    return "FilterVeloL0Calo = TC_MATCHFLTR( '%s', HltTracking.Hlt1StreamerConf.VeloL0Calo )" \
           % selection

def MatchVeloL0CaloCands( selection ):
    return "MatchVeloL0CaloCands = TC_MATCHCANDS( '%s', HltTracking.Hlt1StreamerConf.VeloL0Calo  )" \
           % selection


def ConfiguredTrackL0Calo( parent, name = None, chi2 = 9 ):
    if name == None: name = Hlt__Track2L0CaloMatch.__name__
    return Hlt1Tool( Hlt__Track2L0CaloMatch
                     , name
                     #, MaxMatchChi2 = chi2 
                     ).createConfigurable( parent )

## Configure tool, set the match chi2
ConfiguredTrackL0Calo( ToolSvc(), to_name( "TrackL0Calo" ), 9 )

## Streamer symbol
def FilterTrackL0Calo( selection ):
    return "FilterTrackL0Calo = TC_MATCHFLTR( '%s', HltTracking.Hlt1StreamerConf.TrackL0Calo )" \
           % selection

