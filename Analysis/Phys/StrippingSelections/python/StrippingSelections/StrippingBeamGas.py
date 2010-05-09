from StrippingConf.StrippingLine import StrippingLine

#-----------------------------------------------------------------------------
# Strip Line taking all BeamGas triggers, eventually prescaled
# The Hlt requirement coincides with the definition of the BeamGas routing bit
#-----------------------------------------------------------------------------
lineUnbiased = StrippingLine( "BeamGasUnbiased"
                            , prescale = 1
                            , HLT = "HLT_PASS_SUBSTR('Hlt1BeamGas')"
                            , checkPV = False
                            , postscale = 1
                            )

