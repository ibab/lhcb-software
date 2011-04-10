__all__ = ( 'ValidateWithTT' )
# =============================================================================
## Symbols for streamer users
# =============================================================================
import HltLine.HltDecodeRaw
from Configurables import ToolSvc
from HltLine.HltLine import Hlt1Tool

def ConfiguredTrackFilter( toolName, **args ):
    parent = ToolSvc()
    import Configurables
    toolType = getattr( Configurables, toolName.split( '/' )[ 0 ] )
    return Hlt1Tool( toolType
                     , toolName
                     , **args ).createConfigurable( parent )

## Configure tool, set the match chi2
validate = "FastTTValidationTool/ValidateWithTT"
ConfiguredTrackFilter( validate, MaxTTProj = 7 )

## Streamer symbol
ValidateWithTT = "ValidateWithTT = execute( decodeTT ) * TC_FILTER_TR( '', '%s' )" \
                 % validate
