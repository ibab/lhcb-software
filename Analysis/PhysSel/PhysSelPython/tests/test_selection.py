#!/usr/bin/env python
#
# test Selection-like classes
#
import sys
sys.path.append('../python')
from PhysSelPython.Wrappers import Selection, AutomaticData, NameError
from PhysSelPython.Configurabloids import DummyAlgorithm, DummySequencer

def test_instantiate_tree(selID='0000') :
    sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg000')
    sel0 = Selection(selID, Algorithm = alg0,
                     RequiredSelections = [sel00, sel01])
    return sel0

def test_tree_InputLocations_propagated() :
    
    sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg001')
    sel0 = Selection('Sel001', Algorithm = alg0,
                     RequiredSelections = [sel00, sel01])
    assert len(alg0.InputLocations) == 0
    assert len(sel0.alg.InputLocations) == 2
    assert sel0.alg.InputLocations.count('Phys/Sel00') ==1
    assert sel0.alg.InputLocations.count('Phys/Sel01') ==1

def test_tree_InputLocations_not_duplicated() :
    
    sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg002', InputLocations = ['Phys/Sel00', 'Phys/Sel01'])
    sel0 = Selection('Sel002', Algorithm = alg0,
                     RequiredSelections = [sel00, sel01])
    print "algo length ", len(alg0.InputLocations)
    assert len(alg0.InputLocations) == 2
    assert len(sel0.alg.InputLocations) == 2
    assert sel0.alg.InputLocations.count('Phys/Sel00') ==1
    assert sel0.alg.InputLocations.count('Phys/Sel01') ==1

'''
#This requires the algorithm to raise. Gaudi algorithms do not seen to do this.
def test_selection_with_existing_algo_name_raises() :
    
    sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg003', InputLocations = ['Phys/Sel00', 'Phys/Sel01'])
    try :
        sel0 = Selection('Alg003', Algorithm = alg0,
                         RequiredSelections = [sel02, sel03])
        assert alg0.InputLocations == ['Phys/Sel00', 'Phys/Sel01']
    except NameError :
        print "NameError caught"
'''
def test_selection_with_existing_selection_name_raises() :
    
    sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg003', InputLocations = ['Phys/Sel00', 'Phys/Sel01'])
    try :
        sel0 = Selection('Sel003', Algorithm = alg0,
                         RequiredSelections = [sel02, sel03])
        sel1 = Selection('Sel003', Algorithm = alg0,
                         RequiredSelections = [sel02, sel03])
        assert sel0.alg.InputLocations == ['Phys/Sel00', 'Phys/Sel01']
        assert sel1.alg.InputLocations == ['Phys/Sel02', 'Phys/Sel03']
    except NameError :
        print "NameError caught"

def test_clone_selection_with_existing_selection_name_raises() :
    
    sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg003', InputLocations = ['Phys/Sel00', 'Phys/Sel01'])
    try :
        sel0 = Selection('Sel003', Algorithm = alg0,
                         RequiredSelections = [sel02, sel03])
        sel1 = sel0.clone('Sel003', Algorithm = alg0,
                          RequiredSelections = [sel02, sel03])
        assert sel0.alg.InputLocations == ['Phys/Sel00', 'Phys/Sel01']
        assert sel1.alg.InputLocations == ['Phys/Sel02', 'Phys/Sel03']
    except NameError :
        print "NameError caught"
        
def test_clone_selection_with_new_alg() :
    sel0 = test_instantiate_tree('0001')
    alg1 = DummyAlgorithm('Alg1')
    selClone = sel0.clone('sel0_clone0', Algorithm = alg1)

    assert alg1.name() == 'Alg1'
    assert len(alg1.InputLocations) == 0

    assert len(sel0.alg.InputLocations) == 2
    assert sel0.alg.InputLocations.count('Phys/Sel00') ==1
    assert sel0.alg.InputLocations.count('Phys/Sel01') ==1

def test_clone_selection_with_new_InputLocations() :
    sel = test_instantiate_tree('0002')
    clone00 = AutomaticData('Clone00', Location = 'Phys/Clone00')
    clone01 = AutomaticData('Clone01', Location = 'Phys/Clone01')
    selClone = sel.clone('sel0_clone1', RequiredSelections = [clone00, clone01])
    assert len(selClone.alg.InputLocations) == 2
    assert selClone.alg.InputLocations.count('Phys/Clone00') ==1
    assert selClone.alg.InputLocations.count('Phys/Clone01') ==1



def test_clone_selection_with_cloned_alg() :
    pass

if '__main__' == __name__ :

    def compare_length(x,y) :
        if len(x) < len(y)  : return -1
        if len(x) > len(y)  : return  1
        if len(x) == len(y) : return  0

    import sys

    test_names = filter(lambda k : k.count('test_') > 0, locals().keys())

    __tests = filter( lambda x : x[0] in test_names, locals().items())

    message = ''
    summary = '\n'
    length = len(sorted(test_names, cmp = compare_length, reverse = True)[0]) +2
    
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
