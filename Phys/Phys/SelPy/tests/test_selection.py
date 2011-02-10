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
from SelPy.selection import (flatAlgorithmList,
                             Selection,
                             EventSelection,
                             PassThroughSelection,
                             AutomaticData,
                             NamedObject,
                             UniquelyNamedObject,
                             NameError)


def test_Selection_duplicate_name_raises() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelUniqueNameTest', ConfGenerator=alg)
    raises (NameError, Selection, 'SelUniqueNameTest', ConfGenerator=alg)

def test_Selection_name() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelNameTest', ConfGenerator=alg)
    assert sel.name()=='SelNameTest'

def test_Selection_outputLocation() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest0', ConfGenerator=alg)
    assert sel.outputLocation()=='Phys/SelOutputTest0/Particles'

def test_Selection_outputLocation_with_user_defined_branch_and_extension() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest1',
                    ConfGenerator=alg,
                    OutputBranch='Rec',
                    Extension = 'Tracks')
    assert sel.outputLocation()=='Rec/SelOutputTest1/Tracks'

def test_Selection_outputLocation_with_user_defined_branch_and_no_extension() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest2',
                    ConfGenerator=alg,
                    OutputBranch='Rec',
                    Extension = '')
    assert sel.outputLocation()=='Rec/SelOutputTest2'

def test_Selection_outputLocation_with_user_defined_extension_and_no_branch() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest3',
                    ConfGenerator=alg,
                    OutputBranch='',
                    Extension = 'Tracks')
    assert sel.outputLocation()=='SelOutputTest3/Tracks'

def test_Selection_outputLocation_with_no_branch_and_no_extension() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest4',
                    ConfGenerator=alg,
                    OutputBranch='',
                    Extension = '')
    assert sel.outputLocation()=='SelOutputTest4'
    
def test_EventSelection_duplicate_name_raises() :
    alg = DummyAlgorithm('selNameTest')
    es = EventSelection('EvtUniqueSelNameTest', ConfGenerator = alg)
    raises (NameError, EventSelection, 'EvtUniqueSelNameTest', ConfGenerator = alg)

def test_EventSelection_name() :
    alg = DummyAlgorithm('selNameTest')
    sel = EventSelection('EvtSelNameTest', ConfGenerator=alg)
    assert sel.name()=='EvtSelNameTest'

def test_EventSelection_outputLocaiton() :
    alg = DummyAlgorithm('selNameTest')
    sel = EventSelection('EvtSelOutputTest', ConfGenerator=alg)
    assert sel.outputLocation()==''

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

def test_PassThroughSelection_outputLocaiton() :
    alg = DummyAlgorithm('selNameTest')
    reqSel = AutomaticData(Location='Pass/Through/DOD')
    sel = PassThroughSelection('PTSelOutputTest',
                               ConfGenerator=alg,
                               RequiredSelection=reqSel)
    assert sel.outputLocation()=='Pass/Through/DOD'

    
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
        
