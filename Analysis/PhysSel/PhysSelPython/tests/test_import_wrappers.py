#!/usr/bin/env python
'''
Test loading of modules
'''

import sys
sys.path.append('../python')

def test_import_physselpython_wrappers() :
    import PhysSelPython.Wrappers

def test_import_physselpython_selection() :
    import PhysSelPython.selection


if '__main__' == __name__ :

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
        
