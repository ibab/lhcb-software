#!/usr/bin/env python
from py.test import raises

def test_checkConfig_raises_KeyError_for_missing_parameter() :
    from StrippingUtils.Utils import checkConfig
    
    ref_params = ('A', 'B', 'C', 'D')

    config = { 'A' :1, 'B' : 2, 'C' : 3}

    raises(KeyError, checkConfig, ref_params, config)

def test_checkConfig_raises_KeyError_for_too_many_parameters() :
    from StrippingUtils.Utils import checkConfig
    
    ref_params = ('A', 'B', 'C', 'D')

    config = { 'A' :1, 'B' : 2, 'C' : 3, 'D' : 4, 'E' : 5}

    raises(KeyError, checkConfig, ref_params, config)

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
        
