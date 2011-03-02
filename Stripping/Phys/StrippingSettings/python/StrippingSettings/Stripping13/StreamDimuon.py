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

# beta_s WG
configs = [
	'BetaS', 
	'BetaSPsi2S',
        'BetaSPsi2SMuMu',
	'BetaSBs2JpsieePhi',
	'BetaSQ2B',
	'BetaSBs2PhiPhi',
	'BetaSBs2KstKst',
	'BetaSBs2PhiKst',
	'BetaSBs2EtacPhi',
	'BetaSBs2ChicPhi_Chic2KKPiPi',
	'BetaSBs2ChicPhi_Chic2PiPiPiPi'
	]

# FWG
configs += [
	'FullDSTDiMuon',
	'Bc2JpsiH',
	'Bc2JpsiHDetached',
	'JpsiMuMuforD0MuMu',
	'BuToKX3872',
	'NeuroBayesMuMu'
	]

# Rare decays WG
configs += [
	'Bs2MuMuPhi',
	'Bd2KstarMuMu',
	'B2XMuMuSS',
	'Bs2MuMuLines',
	'B2MuMuMuMuLines',
	'TriMuon'
	]

for config in configs:
	stream.appendLines( lineBuilder('stripping13.tmp', config).lines() )


