#!/usr/bin/env python
#
# test Selection-like classes
#
import sys
from py.test import raises
sys.path.append('../python')
from PhysSelPython.Wrappers import ( Selection,
                                     AutomaticData,
                                     MergedSelection,
                                     VoidEventSelection,
                                     EventSelection,
                                     NameError,
                                     NonEmptyInputLocations,
                                     IncompatibleInputLocations )
from SelPy.configurabloids import DummyAlgorithm, DummySequencer
from Configurables import FilterDesktop

def test_automatic_data() :
    sel00 = AutomaticData(Location = 'Phys/Sel00x',
                          Extension='')
    assert sel00.name() == 'Phys_Sel00x'
    assert sel00.algorithm().name() == 'SelFilterPhys_Sel00x'
    assert sel00.outputLocation() == 'Phys/Sel00x'

    sel00 = AutomaticData(Location = 'Phys/Sel00x/Particles',
                          Extension='Particles')
    assert sel00.name() == 'Phys_Sel00x_Particles'
    assert sel00.algorithm().name() == 'SelFilterPhys_Sel00x_Particles'
    assert sel00.outputLocation() == 'Phys/Sel00x/Particles'


def test_VoidEventSelection() :
     sel00 = AutomaticData(Location = 'Phys/Sel00x')
     ves00 = VoidEventSelection('VES00',
                                Code = 'Test (<Location>) > X',
                                RequiredSelection = sel00)
     assert sel00.outputLocation() == ves00.outputLocation()
     assert ves00.outputLocation() == 'Phys/Sel00x/Particles'
     assert ves00.algorithm().Code == "Test ('Phys/Sel00x/Particles') > X"

def test_VoidEventSelection_code_integrity() :

    sel00 = AutomaticData(Location = 'Phys/Sel00x')

    ves01 = VoidEventSelection('VES01',
                               Code = 'Test (<Location>) > X',
                               RequiredSelection = sel00)
    coderef =  "Test ('Phys/Sel00x/Particles') > X"

    assert ves01.algorithm().Code == coderef

    ves02 = VoidEventSelection('VES02',
                               Code = "Test (<Location>) > X",
                               RequiredSelection = sel00)
    assert ves02.algorithm().Code == coderef

    ves03 = VoidEventSelection('VES03',
                               Code = "Test ('<Location>') > X",
                               RequiredSelection = sel00)
    assert ves03.algorithm().Code == coderef

    ves04 = VoidEventSelection('VES04',
                               Code = 'Test (\'<Location>\') > X',
                               RequiredSelection = sel00)
    assert ves04.algorithm().Code == coderef


def test_VoidEventSelection_with_existing_name_raises() :
    raises(NameError, VoidEventSelection, 'VES00', Code = 'blah', RequiredSelection = AutomaticData(Location='blah/blah'))
    
def test_VoidEventSelection_with_existing_configurable_name_raises() :

    selFilter = FilterDesktop('SelFilter0')
    sel = AutomaticData(Location = 'Phys/Sel')

    raises(NameError, VoidEventSelection, 'SelFilter0',
           Code = '<Location>',
           RequiredSelection = sel)
    
def test_automatic_data_does_not_accept_more_than_one_ctor_argument() :
    raises(TypeError, AutomaticData, 'name', Location = 'Phys/Sel00')

def test_selection_with_existing_configurable_name_raises() :
    _fd0 = FilterDesktop('_fd0')
    selFilter = FilterDesktop('SelFilter1')
    raises(NameError, Selection, 'SelFilter1', Algorithm=_fd0)

def test_mergedselection_with_existing_configurable_name_raises() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    selFilter = FilterDesktop('MergeFilter')
    raises(NameError, MergedSelection, 'MergeFilter',
           RequiredSelections = [sel00, sel01, sel02, sel03] )

def test_automatic_data_with_no_location_raises() :
    raises(Exception, AutomaticData)

def test_merged_selection() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    ms = MergedSelection('Merge00And01', RequiredSelections = [sel00, sel01])
    assert ms.name() == 'Merge00And01'
    assert ms.requiredSelections() == [] # should not export its required selections. Algos contained internally.
    assert ms.outputLocation() == 'Phys/Merge00And01/Particles'
    assert [alg.name() for alg in ms._algos] == ['SelFilterPhys_Sel00_Particles', 'SelFilterPhys_Sel01_Particles', 'Merge00And01']
    assert ms._algos == [sel00.algorithm(), sel01.algorithm(), ms._sel.algorithm()]

def test_clone_merged_selection() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    ms = MergedSelection('Merge00And01Original', RequiredSelections = [sel00, sel01])
    msClone = ms.clone(name='Merge00And01Clone')
    assert msClone.name() == 'Merge00And01Clone'
    assert msClone.requiredSelections() == [] # should not export its required selections. Algos contained internally.
    assert msClone.outputLocation() == 'Phys/Merge00And01Clone/Particles'
    assert [alg.name() for alg in msClone._algos] == ['SelFilterPhys_Sel00_Particles', 'SelFilterPhys_Sel01_Particles', 'Merge00And01Clone']
    assert msClone._algos == [sel00.algorithm(), sel01.algorithm(), msClone._sel.algorithm()]

def test_merged_selection_with_existing_selection_name_raises() :
    
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')

    sel0 = MergedSelection('MergedSel001',
                           RequiredSelections = [sel00, sel01])

    raises(NameError, 
           MergedSelection, 'MergedSel001',
           RequiredSelections = [sel00])

    
def test_instantiate_tree(selID='0000') :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg000')
    sel0 = Selection(selID, Algorithm = alg0,
                     RequiredSelections = [sel00, sel01])
    return sel0

def test_outputLocation() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    assert sel00.outputLocation() == 'Phys/Sel00/Particles'
    alg0 = DummyAlgorithm('Alg0001')
    sel0 = Selection('SomeName001', Algorithm = alg0)
    assert sel0.outputLocation() == 'Phys/SomeName001/Particles'
    sel1 = Selection('SomeName002', OutputBranch = 'HLT2', Algorithm = alg0,
                     Extension='HltParticles')
    assert sel1.outputLocation() == 'HLT2/SomeName002/HltParticles'
    
def test_tree_InputLocations_propagated() :
    
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg001')
    sel0 = Selection('Sel001', Algorithm = alg0,
                     RequiredSelections = [sel00, sel01])
    assert len(alg0.InputLocations) == 0
    assert len(sel0.algorithm().InputLocations) == 2
    assert sel0.algorithm().InputLocations.count('Phys/Sel00/Particles') ==1
    assert sel0.algorithm().InputLocations.count('Phys/Sel01/Particles') ==1


'''
#This requires the algorithm to raise. Gaudi algorithms do not seen to do this.
def test_selection_with_existing_algo_name_raises() :
    
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg003', InputLocations = ['Phys/Sel00', 'Phys/Sel01'])
    try :
        sel0 = Selection('Alg003', Algorithm = alg0,
                         RequiredSelections = [sel02, sel03])
        assert alg0.InputLocations == ['Phys/Sel00', 'Phys/Sel01']
    except NameError :
        print "NameError caught"
'''
def test_selection_with_existing_selection_name_raises() :
    
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg003')

    sel0 = Selection('Sel003', Algorithm = alg0,
                     RequiredSelections = [sel02, sel03])
    raises(NameError, Selection, 'Sel003', Algorithm = alg0,
                         RequiredSelections = [sel02, sel03])

    assert sel0.algorithm().InputLocations == ['Phys/Sel02/Particles',
                                               'Phys/Sel03/Particles']

def test_clone_selection_with_existing_selection_name_raises() :
    
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg004')
    sel0 = Selection('Sel004', Algorithm = alg0,
                     RequiredSelections = [sel02, sel03])
    

    raises( NameError, sel0.clone, name = 'Sel004', Algorithm = alg0,
            RequiredSelections = [sel02, sel03])
    assert sel0.algorithm().InputLocations == ['Phys/Sel02/Particles',
                                       'Phys/Sel03/Particles']
        
def test_clone_selection_with_new_alg() :
    sel0 = test_instantiate_tree('0001')
    alg1 = DummyAlgorithm('Alg1')
    selClone = sel0.clone(name='sel0_clone0', Algorithm = alg1)

    assert alg1.name() == 'Alg1'
    assert len(alg1.InputLocations) == 0

    assert len(sel0.algorithm().InputLocations) == 2
    assert sel0.algorithm().InputLocations.count('Phys/Sel00/Particles') ==1
    assert sel0.algorithm().InputLocations.count('Phys/Sel01/Particles') ==1

def test_clone_selection_with_new_InputLocations() :
    sel = test_instantiate_tree('0002')
    clone00 = AutomaticData(Location = 'Phys/Clone00')
    clone01 = AutomaticData(Location = 'Phys/Clone01')
    selClone = sel.clone(name='sel0_clone1',
                         RequiredSelections = [clone00, clone01])
    assert len(selClone.algorithm().InputLocations) == 2
    assert selClone.algorithm().InputLocations.count('Phys/Clone00/Particles') ==1
    assert selClone.algorithm().InputLocations.count('Phys/Clone01/Particles') ==1

def test_selection_with_name_overlap_doesnt_raise() :
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg005')
    sel0 = Selection('Sel005', Algorithm = alg0)
    sel1 = Selection('Sel005Loose', Algorithm = alg0)
    assert sel0.outputLocation() == 'Phys/Sel005/Particles'
    assert sel1.outputLocation() == 'Phys/Sel005Loose/Particles'

def test_selection_with_different_InputLocations_set_in_algo_raises() :
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg006',
                          InputLocations = ['Phys/Sel00', 'Phys/Sel01'])
    raises(IncompatibleInputLocations, Selection, 'Sel006', Algorithm = alg0,
           RequiredSelections = [sel02, sel03])

def test_selection_with_same_InputLocations_set_in_algo() :
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = DummyAlgorithm('Alg007',
                          InputLocations = ['Phys/Sel02/Particles',
                                            'Phys/Sel03/Particles'])
    sel0 = Selection('Sel007', Algorithm = alg0,
                     RequiredSelections = [sel02, sel03])
        
def test_clone_selection_with_cloned_alg() :
    pass

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
        
