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
                                     EventSelection,
                                     NameError,
                                     NonEmptyInputLocations,
                                     IncompatibleInputLocations )
from PhysSelPython.MockStrippingLine import MockStrippingLine as dummy

from SelPy.configurabloids import DummyAlgorithm, DummySequencer

def test_dummy_automaticdata() :
    data = AutomaticData(Location='Phys/Data0')
    newData = dummy('NewData0', data)
    assert newData.selection().name() == 'NewData0'
    assert newData.outputLocation() == newData.selection().outputLocation() == 'Phys/NewData0/Particles'
    assert len(newData.members()) == 1
    print newData.members()[0].Members
    assert len(newData.members()[0].Members) == 2
    
def test_dummy_mergedselection() :
    data0 = AutomaticData(Location='Phys/Data0')
    data1 = AutomaticData(Location='Phys/Data1')
    mergedData = MergedSelection('MergedData01', RequiredSelections = [data0, data1])
    newData = dummy('NewMergedData', mergedData)
    assert newData.selection().name() == 'NewMergedData'
    assert newData.outputLocation() == newData.selection().outputLocation() == 'Phys/NewMergedData/Particles'
    assert len(newData.members()) == 1
    assert len(newData.members()[0].Members) == 3
    
def test_dummy_selection() :
    data0 = AutomaticData(Location='Phys/Data0')
    data1 = AutomaticData(Location='Phys/Data1')
    algo = DummyAlgorithm('_sel01')
    sel = Selection('Sel01', Algorithm = algo, RequiredSelections = [data0, data1])
    newData = dummy('NewSel01', sel)
    assert newData.selection().name() == 'NewSel01'
    assert newData.outputLocation() == newData.selection().outputLocation() == 'Phys/NewSel01/Particles'
    assert len(newData.members()) == 3

def test_eventSelection_with_no_output():
    alg = DummyAlgorithm('evtSel')
    evtSel = EventSelection(alg)
    newSel = dummy('NewEvtSel', evtSel)
    assert newSel.selection().name() == 'evtSel'
    assert newSel.outputLocation() == ''
    assert len(newSel.members()) == 1

def test_eventSelection_with_output():
    alg = DummyAlgorithm('evtSel')
    data0 = AutomaticData(Location='Phys/Data0')
    evtSel = EventSelection(alg, RequiredSelection = data0)
    newSel = dummy('NewEvtSel', evtSel)
    assert newSel.selection().name() == 'NewEvtSel'
    assert newSel.outputLocation() == 'Phys/NewEvtSel/Particles'
    assert len(newSel.members()) == 1
                            

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
        
