#!/usr/bin/env python

from StrippingConf.StrippingStream import StrippingStream

def test_bs2jpsiphi() :
    from StrippingSelections.StrippingBs2JpsiPhi_dev import Bs2JpsiPhiConf as conf
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
                       "BsVCHI2Loose" 	: 20.0,
                       "BsReFitPVs"         : True
                       }

    bs2jpsiphi = conf(config_params)
    bs2JpsiPhiStream = StrippingStream('StreamBs2JpsiPhi', 
                                   Lines = bs2jpsiphi.lines)
    print '\n'
    for line in bs2JpsiPhiStream.lines :
        print 'Line', line.name(), 'Location', line.outputLocation()

def test_bs2jpsiphiprescaled() :
    from StrippingSelections.StrippingBs2JpsiPhiPrescaled import Bs2JpsiPhiPrescaledConf as conf
    config_params = {'PhiPT' : 500.,
                     'BsMassWin' : 300.,
                     'BsLTFITCHI2' : 14.,
                     'BsVCHI2' : 20.,
                     'LinePrescale' : 1.0,
                     'LinePostscale' : 0.1}

    bs2jpsiphi = conf('Bs2JpsiPhiPrescTest', config_params)

    bs2JpsiPhiStream = StrippingStream('StreamBs2JpsiPhi', 
                                       Lines = bs2jpsiphi.lines)
    print '\n'
    for line in bs2JpsiPhiStream.lines :
        print 'Line', line.name(), 'Location', line.outputLocation()


if __name__ == '__main__' :

    import sys

    test_names = filter(lambda k : k.count('test_') > 0, locals().keys())

    __tests = filter( lambda x : x[0] in test_names, locals().items())
    

    message = ''
    summary = '\n'
    length = len(sorted(test_names,
                        cmp = lambda x,y : cmp(len(y),len(x)))[0]) +2
    
    for test in __tests :
        try :
            test[1]()
            message = 'PASS'
        except :
            message = "FAIL"
        summary += test[0].ljust(length) + ':' + message.rjust(10) + '\n'

    if summary.count('FAIL') > 0 :
        message = 'FAIL'
        wr = sys.stderr.write
    else :
        message = 'PASS'
        wr = sys.stdout.write

    summary += 'Global'.ljust(length) + ':' + message.rjust(10) + '\n\n'
    wr(summary)
        
