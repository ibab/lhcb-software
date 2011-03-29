import PyCintex
_global   = PyCintex.makeNamespace('')
cpp      = _global
LHCb     = cpp.LHCb

__all__ = ( 'MatchVeloElectron' )
# =============================================================================
## Symbols for streamer users
# =============================================================================
import HltLine.HltDecodeRaw
from Configurables import Hlt__L0Calo2Candidate, Hlt__MatchVeloElectron, ToolSvc
from HltLine.HltLine import Hlt1Tool


## Helper functions
def to_name( conf ):
    return conf.mTool().split( '/' )[ -1 ].split( '::' )[ -1 ].split( ':' )[ 0 ]

def ConfiguredMVE( parent, name = None, chi2 = 25 ):
    if name == None: name = Hlt__MatchVeloElectron.__name__
    return Hlt1Tool( Hlt__MatchVeloElectron
                     , name
                     , MaxMatchChi2 = chi2 ).createConfigurable( parent )

## Configure tool, set the match chi2
import Hlt1StreamerConf as Conf
ConfiguredMVE( ToolSvc(), to_name( Conf.VeloElectron ), 25 )

## L0Calo -> Candidate
cc = Hlt__L0Calo2Candidate( 'Hlt1L0CaloCandidates' )

## Streamer symbol
MatchVeloElectron = "MatchVeloElectron = ( execute( decodeL0CALO ) & execute( '%s' ) ) \
                     * TC_MATCHFLTR( 'Hlt1L0CaloCandidates', HltTracking.Hlt1StreamerConf.VeloElectron )" \
                    % cc.getFullName()
