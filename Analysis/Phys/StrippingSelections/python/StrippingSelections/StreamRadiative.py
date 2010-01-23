#
#   Stripping selections 
#
#   @author F. Soomro
#   @date 2009-12-18
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream


from StrippingSelections.StrippingB2XGamma import StrippingB2XGammaConf

stream = StrippingStream("Radiative")
stream.appendLines( [
    StrippingB2XGammaConf().phigamma()
    ,StrippingB2XGammaConf().phigammaWideBMass()
    ,StrippingB2XGammaConf().phigammaLooseDira()
    ,StrippingB2XGammaConf().kstgamma()
    ,StrippingB2XGammaConf().kstgammaWideBMass()
    ,StrippingB2XGammaConf().kstgammaLooseDira()
    ,StrippingB2XGammaConf().kstgammaWideKstMass()
    ] )
