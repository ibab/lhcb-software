# $Id: StreamMiniBias.py,v 1.1 2010-09-07 11:57:00 gcowan Exp $
#
#   Minbias stripping selection
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("MiniBias")

#
# Lumi Group
# Jaap Paanman, Yasmine Amhis, Plamen Hopchev
#
from StrippingSelections import StrippingMiniBias 
stream.appendLines( [ StrippingMiniBias.MBMicroBiasRateLimitedLine, 
                      StrippingMiniBias.MBMicroBiasNotRateLimitedLine,  
                      StrippingMiniBias.MBNoBiasLine,
                      StrippingMiniBias.MBMiniBiasLine,
                      ] )

from StrippingSelections import StrippingBeamGas
stream.appendLines( [ StrippingBeamGas.lineUnbiased ] )

