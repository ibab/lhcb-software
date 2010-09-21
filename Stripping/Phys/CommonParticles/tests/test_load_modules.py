#!/usr/bin/env python
#$Id: test_load_modules.py,v 1.1 2010-04-22 14:44:53 jpalac Exp $
'''
Test loading various CommonParticles in the Analysis environment. Needs to be extended.
'''
__author__ = 'Juan Palacios juan.palacios@nikhef.nl'


import sys
sys.path.append('../python')


def test_load_utils() :
    from CommonParticles import Utils

def test_load_standard_basic_neutral() :
    from CommonParticles import StandardBasicNeutral

def test_load_standard_basic_charged() :
    from CommonParticles import StandardBasicCharged

def test_load_all() :
    from CommonParticles import *

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
        
