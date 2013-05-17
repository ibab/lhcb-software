import PyCintex
_global   = PyCintex.makeNamespace('')
cpp      = _global
LHCb     = cpp.LHCb

__all__ = ( 'MatchVeloL0Calo' )
# =============================================================================
## Symbols for streamer users
# =============================================================================
import HltLine.HltDecodeRaw
from Configurables import Hlt__MatchVeloL0Calo, ToolSvc
from HltLine.HltLine import Hlt1Tool

## Helper functions
def to_name( conf ):
    return conf.mTool().split( '/' )[ -1 ].split( '::' )[ -1 ].split( ':' )[ 0 ]

def ConfiguredVeloL0Calo( parent, name = None, chi2 = 9 ):
    if name == None: name = Hlt__MatchVeloL0Calo.__name__
    return Hlt1Tool( Hlt__MatchVeloL0Calo
                     , name
                     , MaxMatchChi2 = chi2 ).createConfigurable( parent )

## Configure tool, set the match chi2
import Hlt1StreamerConf as Conf
ConfiguredVeloL0Calo( ToolSvc(), to_name( Conf.VeloL0Calo ), 9 )

## Streamer symbol
def MatchVeloL0Calo( selection ):
    return "MatchVeloL0Calo = TC_MATCHFLTR( '%s', HltTracking.Hlt1StreamerConf.VeloL0Calo )" \
           % selection
