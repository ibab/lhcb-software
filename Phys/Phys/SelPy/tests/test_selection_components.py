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

class SelectionTree(object) :
    sel000 = Selection('0.00000', ConfGenerator = DummyAlgorithm('Alg0.00000'),
                       RequiredSelections = [])
    sel001 = Selection('0.00001', ConfGenerator = DummyAlgorithm('Alg0.00001'),
                       RequiredSelections = [])
    sel002 = Selection('0.00002', ConfGenerator = DummyAlgorithm('Alg0.00002'),
                       RequiredSelections = [])
    sel003 = Selection('0.00003', ConfGenerator = DummyAlgorithm('Alg0.00003'),
                       RequiredSelections = [])
    
    sel100 = Selection('1.00000', ConfGenerator = DummyAlgorithm('Alg1.00000'),
                       RequiredSelections = [sel000, sel001])
    
    sel101 = Selection('1.00001', ConfGenerator = DummyAlgorithm('Alg1.00001'),
                       RequiredSelections = [sel002, sel003])
    
    sel200 = Selection('2.00000', ConfGenerator = DummyAlgorithm('Alg2.00000'),
                       RequiredSelections = [sel100, sel101])

    algos = flatAlgorithmList(sel200)
    alg_names = [a.name() for a in algos]


def test_NamedObject_name() :
    un = NamedObject('HelloWorld')
    assert un.name() == 'HelloWorld'

def test_uniquelyNamedObject_instantiaiton() :
    un = UniquelyNamedObject('Fred')

def test_UniquelyNamedObject_raises_on_duplocate_name() :
    un0 = UniquelyNamedObject('Bob')
    raises(NameError, UniquelyNamedObject, 'Bob')

def test_UniquelyNamedObject_name() :
    un = UniquelyNamedObject('HelloWorld')
    assert un.name() == 'HelloWorld'

def test_flatAlgorithmList_order_line() :

    
    sel000 = Selection('0.0000', ConfGenerator = DummyAlgorithm('Alg0.0000'),
                      RequiredSelections = [])
    sel001 = Selection('0.0001', ConfGenerator = DummyAlgorithm('Alg0.0001'),
                      RequiredSelections = [sel000])
    sel101 = Selection('1.0000', ConfGenerator = DummyAlgorithm('Alg1.0000'),
                      RequiredSelections = [sel001])
    sel102 = Selection('1.0001', ConfGenerator = DummyAlgorithm('Alg1.0001'),
                      RequiredSelections = [sel101])
    sel203 = Selection('2.0000', ConfGenerator = DummyAlgorithm('Alg2.000'),
                      RequiredSelections = [sel102])

    algos = flatAlgorithmList(sel203)
    assert [a.name() for a in algos] == ['0.0000', '0.0001', '1.0000', '1.0001', '2.0000']
    
def test_flatAlgorithmList_order_tree() :

    alg_names = SelectionTree.alg_names
    
    assert alg_names.index('0.00000') < alg_names.index('1.00000')
    assert alg_names.index('0.00001') < alg_names.index('1.00000')
    assert alg_names.index('0.00002') < alg_names.index('1.00001')
    assert alg_names.index('0.00003') < alg_names.index('1.00001')
    assert alg_names.index('1.00000') < alg_names.index('2.00000')
    assert alg_names.index('1.00001') < alg_names.index('2.00000')

def test_flatAlgorithmList_removes_duplicates() :
    
    sel000 = SelectionTree.sel000
    sel001 = SelectionTree.sel001
    sel002 = SelectionTree.sel002
    sel003 = SelectionTree.sel003

    sel100 = Selection('1.10000', ConfGenerator = DummyAlgorithm('Alg1.10000'),
                       RequiredSelections = [sel000, sel001, sel002])
    
    sel101 = Selection('1.10001', ConfGenerator = DummyAlgorithm('Alg1.10001'),
                       RequiredSelections = [sel001, sel002, sel003])
    
    sel200 = Selection('2.10000', ConfGenerator = DummyAlgorithm('Alg2.10000'),
                       RequiredSelections = [sel100, sel101])

    algos = flatAlgorithmList(sel200)
    alg_names = [a.name() for a in algos]
    assert len(algos) == 7
    assert alg_names.count('0.00000') == 1
    assert alg_names.count('0.00001') == 1
    assert alg_names.count('0.00002') == 1
    assert alg_names.count('0.00003') == 1
    assert alg_names.count('1.10000') == 1
    assert alg_names.count('1.10001') == 1
    assert alg_names.count('2.10000') == 1
    
    assert alg_names.index('0.00000') < alg_names.index('1.10000')
    assert alg_names.index('0.00001') < alg_names.index('1.10000')
    assert alg_names.index('0.00002') < alg_names.index('1.10000')
    assert alg_names.index('0.00001') < alg_names.index('1.10001')
    assert alg_names.index('0.00002') < alg_names.index('1.10001')
    assert alg_names.index('0.00003') < alg_names.index('1.10001')
    assert alg_names.index('1.10000') < alg_names.index('2.10000')
    assert alg_names.index('1.10001') < alg_names.index('2.10000')


def test_Selection_duplicate_name_raises() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelUniqueNameTest', ConfGenerator=alg)
    raises (NameError, Selection, 'SelUniqueNameTest', ConfGenerator=alg)

def test_Selection_name() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelNameTest', ConfGenerator=alg)
    assert sel.name()=='SelNameTest'

def test_Selection_outputLocaiton() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest0', ConfGenerator=alg)
    assert sel.outputLocation()=='Phys/SelOutputTest0/Particles'

def test_Selection_outputLocaiton_with_user_defined_branch_and_extension() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest1',
                    ConfGenerator=alg,
                    OutputBranch='Rec',
                    Extension = 'Tracks')
    assert sel.outputLocation()=='Rec/SelOutputTest1/Tracks'

def test_Selection_outputLocaiton_with_user_defined_branch_and_no_extension() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest2',
                    ConfGenerator=alg,
                    OutputBranch='Rec',
                    Extension = '')
    assert sel.outputLocation()=='Rec/SelOutputTest2'

def test_Selection_outputLocaiton_with_user_defined_extension_and_no_branch() :
    alg = DummyAlgorithm('selNameTest')
    sel = Selection('SelOutputTest3',
                    ConfGenerator=alg,
                    OutputBranch='',
                    Extension = 'Tracks')
    assert sel.outputLocation()=='SelOutputTest3/Tracks'

def test_Selection_outputLocaiton_with_bo_branch_and_no_extension() :
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
        
