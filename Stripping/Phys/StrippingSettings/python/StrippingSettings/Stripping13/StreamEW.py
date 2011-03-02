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

stream = StrippingStream('EW')

# FWG
configs = [
	'Z02MuMu',
	'WMu',
	'DY2MuMu',
	'Z02ee',
	'We',
	'DY2ee',
	'MuMuSS',
	'LowMult',
	'DiPhotonDiMuon',
	'SingleTrackTIS'
	]

# Exotics
#configs += [
#	'DisplVertices'
#	]
# What about high PT jets?

for config in configs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )


