from GaudiConf.Configuration import *
from StrippingConf.StrippingStream import StrippingStream
from StrippingConf.Configuration import StrippingConf
from StrippingSelections import StrippingBs2JpsiPhi_dev as stripping

config_params =  { "MuonTRCHI2Loose"	: 10.0,
                   "JpsiMassWinLoose"	: 80.0,
                   "JpsiVCHI2Loose"	: 20.0,
                   "KaonTRCHI2Loose"	: 10.0,
                   "KaonPIDK"	        : -5.0,
                   "PhiMassWin"	        : 15.0,
                   "PhiMassWinLoose"	: 30.0,
                   "PhiPT" 	        : 1000.0,
                   "PhiPTLoose" 	: 500.0,
                   "PhiVCHI2" 	        : 20.0,
                   "PhiVCHI2Loose" 	: 20.0,
                   "BsMassWin"	        : 300.0,
                   "BsMassWinLoose"	: 300.0,
                   "BsVCHI2" 	        : 5.0,
                   "BsVCHI2Loose" 	: 20.0
                   }

bs2jpsiphi = stripping.StrippingBs2JpsiPhiConf(config_params)
bs2JpsiPhiStream = StrippingStream('StreamBs2JpsiPhi', Lines = bs2jpsiphi.lines)
print '\n'
for line in bs2JpsiPhiStream.lines :
    print 'Line', line.name(), 'Location', line.outputLocation()
