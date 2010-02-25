#!/usr/bin/env python
#$Id: test_selection_components.py,v 1.1 2010-02-25 14:08:47 jpalac Exp $
'''
Test suite for selection module.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from PhysSelPython.Configurabloids import DummyAlgorithm
from PhysSelPython.selection import FlatSelectionListBuilder, Selection, AutomaticData

class SelectionTree(object) :
    sel000 = Selection('0.00000', Algorithm = DummyAlgorithm('Alg0.00000'),
                       RequiredSelections = [])
    sel001 = Selection('0.00001', Algorithm = DummyAlgorithm('Alg0.00001'),
                       RequiredSelections = [])
    sel002 = Selection('0.00002', Algorithm = DummyAlgorithm('Alg0.00002'),
                       RequiredSelections = [])
    sel003 = Selection('0.00003', Algorithm = DummyAlgorithm('Alg0.00003'),
                       RequiredSelections = [])
    
    sel100 = Selection('1.00000', Algorithm = DummyAlgorithm('Alg1.00000'),
                       RequiredSelections = [sel000, sel001])
    
    sel101 = Selection('1.00001', Algorithm = DummyAlgorithm('Alg1.00001'),
                       RequiredSelections = [sel002, sel003])
    
    sel200 = Selection('2.00000', Algorithm = DummyAlgorithm('Alg2.00000'),
                       RequiredSelections = [sel100, sel101])

    algos = FlatSelectionListBuilder(sel200).selectionList
    alg_names = [a.name() for a in algos]


def test_FlatSelectionListBuilder_order_line() :

    
    sel000 = Selection('0.0000', Algorithm = DummyAlgorithm('Alg0.0000'),
                      RequiredSelections = [])
    sel001 = Selection('0.0001', Algorithm = DummyAlgorithm('Alg0.0001'),
                      RequiredSelections = [sel000])
    sel101 = Selection('1.0000', Algorithm = DummyAlgorithm('Alg1.0000'),
                      RequiredSelections = [sel001])
    sel102 = Selection('1.0001', Algorithm = DummyAlgorithm('Alg1.0001'),
                      RequiredSelections = [sel101])
    sel203 = Selection('2.0000', Algorithm = DummyAlgorithm('Alg2.000'),
                      RequiredSelections = [sel102])

    algos = FlatSelectionListBuilder(sel203).selectionList
    assert [a.name() for a in algos] == ['0.0000', '0.0001', '1.0000', '1.0001', '2.0000']
    
def test_FlatSelectionListBuilder_order_tree() :

    alg_names = SelectionTree.alg_names
    
    assert alg_names.index('0.00000') < alg_names.index('1.00000')
    assert alg_names.index('0.00001') < alg_names.index('1.00000')
    assert alg_names.index('0.00002') < alg_names.index('1.00001')
    assert alg_names.index('0.00003') < alg_names.index('1.00001')
    assert alg_names.index('1.00000') < alg_names.index('2.00000')
    assert alg_names.index('1.00001') < alg_names.index('2.00000')

def test_FlatSelectionListBuilder_removes_duplicates() :
    
    sel000 = SelectionTree.sel000
    sel001 = SelectionTree.sel001
    sel002 = SelectionTree.sel002
    sel003 = SelectionTree.sel003

    sel100 = Selection('1.10000', Algorithm = DummyAlgorithm('Alg1.10000'),
                       RequiredSelections = [sel000, sel001, sel002])
    
    sel101 = Selection('1.10001', Algorithm = DummyAlgorithm('Alg1.10001'),
                       RequiredSelections = [sel001, sel002, sel003])
    
    sel200 = Selection('2.10000', Algorithm = DummyAlgorithm('Alg2.10000'),
                       RequiredSelections = [sel100, sel101])

    algos = FlatSelectionListBuilder(sel200).selectionList
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

