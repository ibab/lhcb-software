#!/usr/bin/env python
#$Id: test_selection_components.py,v 1.2 2010/02/25 14:13:33 jpalac Exp $
'''
Test suite for selection module.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from py.test import raises
from SelPy.configurabloids import MockConfGenerator
from SelPy.selection import EventSelection, NameError

    
def test_EventSelection_duplicate_name_raises() :
    alg = MockConfGenerator()
    es = EventSelection('EvtUniqueSelNameTest', ConfGenerator = alg)
    raises (NameError, EventSelection, 'EvtUniqueSelNameTest', ConfGenerator = alg)

def test_EventSelection_name() :
    alg = MockConfGenerator()
    sel = EventSelection('EvtSelNameTest', ConfGenerator=alg)
    assert sel.name()=='EvtSelNameTest'

def test_EventSelection_outputLocaiton() :
    alg = MockConfGenerator()
    sel = EventSelection('EvtSelOutputTest', ConfGenerator=alg)
    assert sel.outputLocation()==''

    
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
        
