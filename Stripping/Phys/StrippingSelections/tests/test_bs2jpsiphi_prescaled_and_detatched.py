#!/usr/bin/env python

from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingBs2JpsiPhiPrescaledAndDetatched import Bs2JpsiPhiPrescaledAndDetatchedConf as builder

config_params = {'PhiPT' : 500.,
                 'BsMassWin' : 300.,
                 'BsLTFITCHI2' : 14.,
                 'BsVCHI2' : 20.,
                 'DetatchedBsLTIME' : 15.,
                 'PrescaledLinePrescale' : 1.0,
                 'PrescaledLinePostscale' : 0.1,
                 'DetatchedLinePrescale' : 1.0,
                 'DetatchedLinePostscale' : 1.0}

bs2jpsiphi = builder('Bs2JpsiPhi', config_params)

from StrippingUtils import LineBuilderTests

def test_make_many_instances() :
    LineBuilderTests.test_make_many_instances(builder,
                                              config_params)

def test_default_raises() :
    LineBuilderTests.test_default_raises(builder)

def test_single_constructor_argument_raises() :
    LineBuilderTests.test_single_constructor_argument_raises(builder,
                                                             config_params)


def test_lines() :
    LineBuilderTests.test_lines(bs2jpsiphi)

def test_line_locations() :
    LineBuilderTests.test_line_locations(bs2jpsiphi)

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
        
