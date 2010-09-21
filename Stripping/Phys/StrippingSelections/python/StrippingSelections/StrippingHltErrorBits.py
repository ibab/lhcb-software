from StrippingConf.StrippingLine import StrippingLine

__all__ = ('lineErrors', 'lineSlow', 'lineErrorsNotSlow', 'lines')

#-----------------------------------------------------------------------------
## line to pick up the events with Hlt error bits
#-----------------------------------------------------------------------------
lineErrors = StrippingLine( "HltErrorBits"
                            , prescale = 1
                            , HLT = " HLT_COUNT_ERRORBITS_RE('Hlt1.*Decision', 0xf ) > 0 "
                            , checkPV = False
                            , postscale = 1
                            )

#-----------------------------------------------------------------------------
## line to pick up all 'slow' events
#-----------------------------------------------------------------------------
lineSlow = StrippingLine( "HltSlowEvents"
                            , prescale = 1
                            , HLT = " HLT_COUNT_ERRORBITS_RE('Hlt1.*Decision', 0x1 ) > 0 "
                            , checkPV = False
                            , postscale = 1
                            )

#-----------------------------------------------------------------------------
## line to pick up all 'slow' events
#-----------------------------------------------------------------------------
lineErrorsNotSlow = StrippingLine( "HltErrorBitsNotSlow"
                            , prescale = 1
                            , HLT =" HLT_COUNT_ERRORBITS_RE('Hlt1.*Decision', 0x6 ) > 0 "
                            , checkPV = False
                            , postscale = 1
                            )


lines = [lineErrors, lineSlow, lineErrorsNotSlow]
