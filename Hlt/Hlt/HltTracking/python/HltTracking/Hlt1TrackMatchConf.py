import PyCintex
_global   = PyCintex.makeNamespace('')
cpp      = _global
LHCb     = cpp.LHCb

__all__ = ( 'MatchVeloElectron' )
# =============================================================================
## Symbols for streamer users
# =============================================================================
import HltLine.HltDecodeRaw
from Configurables import Hlt__L0Calo2Candidate
cc = Hlt__L0Calo2Candidate( 'Hlt1L0CaloCandidates' )

MatchVeloElectron = "MatchVeloElectron = ( ( execute( DecodeL0CALO ) & execute( %s ) ) * TC_MATCHFLTR( '', 'Hlt1L0CaloCandidates', HltTracking.Hlt1StreamerConf.VeloElectron )" % cc.getFullName()
