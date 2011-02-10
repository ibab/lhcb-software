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
from SelPy.selection import PassThroughSelection, AutomaticData, NameError


def test_PassThroughSelection_duplicate_name_raises() :
    alg = DummyAlgorithm('selNameTest')
    reqSel = AutomaticData(Location='PassThroughDOD')
    es = PassThroughSelection('PTUniqueSelNameTest',
                              ConfGenerator = alg,
                              RequiredSelection = reqSel)
    raises (NameError,
            PassThroughSelection,
            'PTUniqueSelNameTest',
            ConfGenerator = alg,
            RequiredSelection=reqSel)

def test_PassThroughSelection_name() :
    alg = DummyAlgorithm('selNameTest')
    reqSel = AutomaticData(Location='PassThroughDOD')
    sel = PassThroughSelection('PTSelNameTest',
                               ConfGenerator=alg,
                               RequiredSelection=reqSel)
    assert sel.name()=='PTSelNameTest'

def test_PassThroughSelection_outputLocation() :
    alg = DummyAlgorithm('selNameTest')
    reqSel = AutomaticData(Location='Pass/Through/DOD')
    sel = PassThroughSelection('PTSelOutputTest',
                               ConfGenerator=alg,
                               RequiredSelection=reqSel)
    assert sel.outputLocation()=='Pass/Through/DOD'

def test_PassThroughSelection_outputLocation_with_user_defined_datasetter() :
    alg = DummyAlgorithm('selDataSetterTest')
    reqSel = AutomaticData(Location='Pass/Through/DOD')
    sel = PassThroughSelection('PTSelInputTest',
                               ConfGenerator=alg,
                               RequiredSelection=reqSel,
                               InputDataSetter='TESTINPUTS')
    assert sel.algorithm().TESTINPUTS==[reqSel.outputLocation()]

def test_clone_PassThroughSelection() :
    alg = DummyAlgorithm('selCloneTest')
    reqSel = AutomaticData(Location='Pass/Through/DOD')
    sel = PassThroughSelection('PTSelCloneTest',
                               ConfGenerator=alg,
                               RequiredSelection=reqSel,
                               InputDataSetter='INPUTS')
    selClone = sel.clone(name='PTSelCloneTestClone')
    assert selClone.outputLocation() == sel.outputLocation()
    assert selClone.requiredSelections() == sel.requiredSelections()
    assert selClone.algorithm().INPUTS == sel.algorithm().INPUTS
    assert selClone.algorithm() != sel.algorithm()
    
def test_clone_PassThroughSelection_with_used_name_raises():
    alg = DummyAlgorithm('selCloneTest')
    reqSel = AutomaticData(Location='Pass/Through/DOD')
    sel = PassThroughSelection('PTSelCloneTest2',
                               ConfGenerator=alg,
                               RequiredSelection=reqSel,
                               InputDataSetter='INPUTS')
    raises(NameError, sel.clone,name='PTSelCloneTest2')

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
        
