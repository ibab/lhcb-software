#
#   EW stripping selection
#
#   @author Greig Cowan and Tom Blake 
#   @date 2011-03-02
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingSettings.Utils import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream('Semileptonic')

# SLWG 
configs = [
	'Bd2DstarMuNu',
	'B0q2DplusMuX',
	'Bd2DstarTauNu',
	'B2XuMuNu',
	'B2DMuNuX'
	]

for config in configs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )


