#!/usr/bin/env python
#$Id: test_selection_components.py,v 1.2 2010/02/25 14:13:33 jpalac Exp $
'''
Test suite for selection module.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from py.test import raises
from SelPy.configurabloids import DummyAlgorithm, MockConfGenerator
from SelPy.utils import (IncompatibleInputLocations,
                         IncompatibleOutputLocation)
from SelPy.selection import (flatAlgorithmList,
                             Selection,
                             EventSelection,
                             PassThroughSelection,
                             AutomaticData,
                             NamedObject,
                             UniquelyNamedObject,
                             NameError)


def test_Selection_duplicate_name_raises() :
    alg = MockConfGenerator()
    sel = Selection('SelUniqueNameTest', ConfGenerator=alg)
    raises (NameError, Selection, 'SelUniqueNameTest', ConfGenerator=alg)

def test_Selection_name() :
    alg = MockConfGenerator()
    sel = Selection('SelNameTest', ConfGenerator=alg)
    assert sel.name()=='SelNameTest'

def test_Selection_outputLocation() :
    alg = MockConfGenerator()
    sel = Selection('SelOutputTest0', ConfGenerator=alg)
    assert sel.outputLocation()=='Phys/SelOutputTest0/Particles'
    assert sel.outputLocation()==sel.algorithm().Output

def test_Selection_outputLocation_with_user_defined_branch_and_extension() :
    alg = MockConfGenerator()
    sel = Selection('SelOutputTest1',
                    ConfGenerator=alg,
                    OutputBranch='Rec',
                    Extension = 'Tracks')
    assert sel.outputLocation()=='Rec/SelOutputTest1/Tracks'
    assert sel.outputLocation()==sel.algorithm().Output
    
def test_Selection_outputLocation_with_user_defined_branch_and_no_extension() :
    alg = MockConfGenerator()
    sel = Selection('SelOutputTest2',
                    ConfGenerator=alg,
                    OutputBranch='Rec',
                    Extension = '')
    assert sel.outputLocation()=='Rec/SelOutputTest2'
    assert sel.outputLocation()==sel.algorithm().Output
    
def test_Selection_outputLocation_with_user_defined_extension_and_no_branch() :
    alg = MockConfGenerator()
    sel = Selection('SelOutputTest3',
                    ConfGenerator=alg,
                    OutputBranch='',
                    Extension = 'Tracks')
    assert sel.outputLocation()=='SelOutputTest3/Tracks'
    assert sel.outputLocation()==sel.algorithm().Output
    
def test_Selection_outputLocation_with_no_branch_and_no_extension() :
    alg = MockConfGenerator()
    sel = Selection('SelOutputTest4',
                    ConfGenerator=alg,
                    OutputBranch='',
                    Extension = '')
    assert sel.outputLocation()=='SelOutputTest4'
    assert sel.outputLocation()==sel.algorithm().Output
    
def test_Selection_input_data_setter() :
    alg = MockConfGenerator()
    sel0 = AutomaticData('Phys/Hello/World/0')
    sel1 = AutomaticData('Phys/Hello/World/1')
    sel = Selection('SelOutputTest5',
                    ConfGenerator=alg,
                    RequiredSelections = [sel0,sel1])
    assert sel.algorithm().Inputs==[sel0.outputLocation(),
                                    sel1.outputLocation()]

def test_Selection_output_data_setter() :
    alg = MockConfGenerator()
    sel0 = AutomaticData('Phys/Hello/World/0')
    sel1 = AutomaticData('Phys/Hello/World/1')
    sel = Selection('SelOutputTest6',
                    ConfGenerator=alg,
                    RequiredSelections = [sel0,sel1])
    assert sel.algorithm().Output==sel.outputLocation()

def test_Selection_with_user_defined_input_data_setter() :
    alg = MockConfGenerator()
    sel0 = AutomaticData('Phys/Hello/World/0')
    sel1 = AutomaticData('Phys/Hello/World/1')
    sel = Selection('SelOutputTest7',
                    ConfGenerator=alg,
                    RequiredSelections = [sel0,sel1],
                    InputDataSetter='TESTINPUTS')
    assert sel.algorithm().TESTINPUTS==[sel0.outputLocation(),
                                        sel1.outputLocation()]

def test_Selection_with_user_defined_output_data_setter() :
    alg = MockConfGenerator()
    sel0 = AutomaticData('Phys/Hello/World/0')
    sel1 = AutomaticData('Phys/Hello/World/1')
    sel = Selection('SelOutputTest8',
                    ConfGenerator=alg,
                    RequiredSelections = [sel0,sel1],
                    OutputDataSetter='TESTOUTPUTS')
    assert sel.algorithm().TESTOUTPUTS==sel.outputLocation()

def test_Selection_does_not_modify_generator() :
    alg = MockConfGenerator()
    sel0 = AutomaticData('Phys/Hello/World/0')
    sel1 = AutomaticData('Phys/Hello/World/1')
    sel = Selection('SelGenTest',
                    ConfGenerator=alg,
                    RequiredSelections = [sel0,sel1])
    assert sel.algorithm().Inputs==[sel0.outputLocation(),
                                    sel1.outputLocation()]
    assert hasattr(alg, 'Output') == False

def test_selection_with_incompatible_Inputs_set_in_algo_raises() :
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg0 = MockConfGenerator(Inputs = ['Phys/Sel00', 'Phys/Sel01'])
    raises(IncompatibleInputLocations, Selection, 'SelInputCompatTest',
           ConfGenerator = alg0,
           RequiredSelections = [sel02, sel03])

def test_selection_with_incompatible_Output_set_in_algo_raises() :
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    alg = MockConfGenerator(Output = 'Hello/World')
    raises(IncompatibleOutputLocation,
           Selection,
           'SelOutputCompatTest',
           ConfGenerator = alg,
           RequiredSelections = [sel02, sel03])

    
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
        
