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

BetaSconfigs = [
	'BetaS', 
	'BetaSBs2JpsieePhi',
	'BetaSQ2B',
	'BetaSBs2PhiPhi',
	'BetaSBs2KstKst',
	'BetaSBs2PhiKst',
	'BetaSBs2EtacPhi',
	'BetaSBs2ChicPhi_Chic2KKPiPi',
	'BetaSBs2ChicPhi_Chic2PiPiPiPi'
	]

for config in BetaSconfigs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )

