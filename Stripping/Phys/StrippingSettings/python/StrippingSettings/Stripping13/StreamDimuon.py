#
#   Dimuon stripping selection
#
#   all physics channels with dimuon in the final state
#
#   @author Greig Cowan and Tom Blake 
#   @date 2011-03-01
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingSettings.Utils import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream('Dimuon')

stream.appendLines( lineBuilder('stripping13.tmp', 'BetaS').lines() )

