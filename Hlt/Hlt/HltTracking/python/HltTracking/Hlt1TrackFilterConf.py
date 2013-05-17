__all__ = ( 'ValidateWithTT' )
# =============================================================================
## Symbols for streamer users
# =============================================================================
import HltLine.HltDecodeRaw

def ConfiguredTrackFilter( toolName, parent = None,  **args ):
    from Configurables import ToolSvc
    if not parent : parent = ToolSvc()
    t,n = toolName.split( '/' )
    import Configurables
    t = getattr( Configurables, t )
    from HltLine.HltLine import Hlt1Tool
    return Hlt1Tool( t, n , **args ).createConfigurable( parent )

## Configure tool, set the match chi2
validate = "FastTTValidationTool/ValidateWithTT"
ConfiguredTrackFilter( validate, MaxTTProj = 3 )

## Streamer symbol
ValidateWithTT = "ValidateWithTT = execute( decodeTT ) * TC_FILTER_TR( '', '%s' )" \
                 % validate
