#!/usr/bin/env python
#$Id: test_selection_components.py,v 1.2 2010/02/25 14:13:33 jpalac Exp $
'''
Test suite for selection module.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from py.test import raises
from SelPy.configurabloids import DummyAlgorithm
from SelPy.selection import AutomaticData, SelectionSequence, NameError, SelectionBase

alg = AutomaticData(Location='Test/SelSeq/Location')
    
def test_SelectionSequence_duplicate_name_raises() :
    ss = SelectionSequence('SelSeqUniqueNameTest', TopSelection = alg)
    raises (NameError, SelectionSequence, 'SelSeqUniqueNameTest', TopSelection = alg)

def test_SelectionSequence_name() :
    sel = SelectionSequence('SelSeqNameTest', TopSelection=alg)
    assert sel.name()=='SelSeqNameTest'

def test_SelectionSelection_outputLocaiton() :
    sel = SelectionSequence('SelSeqOutputTest', TopSelection=alg)
    assert sel.outputLocation()==alg.outputLocation()

def test_SelectionSequence_NoneTypes_not_in_algorithms() :
    sel = SelectionBase(algorithm=None, outputLocation='TestLocation', requiredSelections=[])
    ss = SelectionSequence('SelSeqTestNoneTypes', TopSelection=sel)
    assert None not in ss.algorithms()
    
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
        
