#
#   Dielectron stripping selection
#
#   all physics channels with dielectron in the final state
#
#   @author Greig Cowan and Tom Blake 
#   @date 2011-03-02
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingSettings.Utils import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream('Dielectron')

# beta_s WG
configs = [
	'BetaSBs2JpsieePhi',
	]

# FWG
configs += [
	'FullDSTDiElectron'
	]

# Rare decays WG
configs += [
	'Bd2eeKstar',
	]

for config in configs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )


