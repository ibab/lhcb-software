#
#   Bhadron stripping selection
#
#   all physics channels with B hadrons
#
#   @author Greig Cowan and Tom Blake 
#   @date 2011-03-02
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingSettings.Utils import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream('Bhadron')

# beta_s WG
configs = [
	'BetaSQ2B',
        'BetaSBs2PhiPhi',
        'BetaSBs2KstKst',
        'BetaSBs2PhiKst',
        'BetaSBs2EtacPhi',
        'BetaSBs2ChicPhi_Chic2KKPiPi',
        'BetaSBs2ChicPhi_Chic2PiPiPiPi'
	]

# gamma from trees
configs += [
	'B2hhLTUnbiased',
	'B2twobody'
	]

# FWG
configs += [
	]

# Rare decays WG
configs += [
	]

for config in configs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )


