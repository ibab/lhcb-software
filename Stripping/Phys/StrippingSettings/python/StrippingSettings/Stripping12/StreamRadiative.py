# $Id: StreamRadiative.py,v 1.1 2010-09-07 11:57:00 gcowan Exp $
#
#   Radiative stripping selection
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Radiative")

#
# Lines from RD WG
# Ulrik Egede, Fatima Soomro 
#
from StrippingSelections.StrippingB2XGamma import StrippingB2XGammaConf
stream.appendLines( [ StrippingB2XGammaConf().phigamma()
    		    , StrippingB2XGammaConf().phigammaLooseDira()
    		    , StrippingB2XGammaConf().phigammaWideBMass()
    		    , StrippingB2XGammaConf().kstgamma()
    		    , StrippingB2XGammaConf().kstgammaWideBMass()
    		    , StrippingB2XGammaConf().kstgammaLooseDira()
    		    , StrippingB2XGammaConf().kstgammaWideKstMass() ] )


