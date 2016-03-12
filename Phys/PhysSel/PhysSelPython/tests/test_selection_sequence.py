#!/usr/bin/env python
#$Id$
'''
Test suite for SelectionSequence class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
from py.test import raises

sys.path.append('../python')

from PhysSelPython.Wrappers import ( Selection,
                                     SelectionSequence,
                                     AutomaticData,
                                     NameError         )
from SelPy.configurabloids import ( MockConfGenerator,
                                    DummySequencer  )

from Configurables import FilterDesktop

class SelectionTree(object) :
    sel000 = Selection('0.00000', Algorithm = MockConfGenerator(),
                       RequiredSelections = [])
    sel001 = Selection('0.00001', Algorithm = MockConfGenerator(),
                       RequiredSelections = [])
    sel002 = Selection('0.00002', Algorithm = MockConfGenerator(),
                       RequiredSelections = [])
    sel003 = Selection('0.00003', Algorithm = MockConfGenerator(),
                       RequiredSelections = [])
    
    sel100 = Selection('1.00000', Algorithm = MockConfGenerator(),
                       RequiredSelections = [sel000, sel001])
    
    sel101 = Selection('1.00001', Algorithm = MockConfGenerator(),
                       RequiredSelections = [sel002, sel003])
    
    sel200 = Selection('2.00000', Algorithm = MockConfGenerator(),
                       RequiredSelections = [sel100, sel101])

    selSeq = SelectionSequence('Seq06', TopSelection = sel200)
    algs = selSeq.algorithms()
    alg_names = [a.name() for a in algs]

def test_instantiate_sequencer() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    alg0 = MockConfGenerator()
    sel = Selection('00011', Algorithm = alg0,
                    RequiredSelections = [sel00, sel01])
    seq = SelectionSequence('Seq00', TopSelection = sel)

def test_instantiate_dataondemand_sequencer() :
    sel00 = AutomaticData(Location = 'Phys/Sel00/Particles')
    seq = SelectionSequence('Seq00DOD', TopSelection = sel00)
    assert seq.outputLocation() == 'Phys/Sel00/Particles'
    
def test_sequencer_algos() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel01 = Selection('00100', Algorithm = MockConfGenerator(),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00101', Algorithm = MockConfGenerator(),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00102', Algorithm = MockConfGenerator(),
                      RequiredSelections = [ sel01, sel02])
    seq = SelectionSequence('Seq01',
                            TopSelection = sel03)

    seqAlgos = seq.algorithms()

    assert len(seqAlgos) == 7
    for sel in [sel01, sel02, sel03] :
        assert sel.algorithm() in seqAlgos

def test_sequencer_sequence() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel01 = Selection('000110', Algorithm = MockConfGenerator(),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('000111', Algorithm = MockConfGenerator(),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('000112', Algorithm = MockConfGenerator(),
                      RequiredSelections = [ sel01, sel02])

    presel0 = MockConfGenerator()
    presel1 = MockConfGenerator()
    postsel0 = MockConfGenerator()
    postsel1 = MockConfGenerator()

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]
    seq = SelectionSequence('Seq0002',
                            TopSelection = sel03,
                            EventPreSelector = presels,
                            PostSelectionAlgs = postsels,
                            sequencerType=DummySequencer)

    seqAlgos = seq.sequence().Members

    ref_algos = [presel0,
                 presel1,
                 _sel00.algorithm(),
                 _sel01.algorithm(),
                 _sel02.algorithm(),
                 _sel03.algorithm(),
                 sel02.algorithm(),
                 sel01.algorithm(),
                 sel03.algorithm(),
                 postsel0,
                 postsel1]

    assert len(seqAlgos) == 11

    assert presels == ref_algos[:len(presels)]

    assert postsels == ref_algos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

def test_clone_sequence() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel01 = Selection('00120', Algorithm = MockConfGenerator(),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00121', Algorithm = MockConfGenerator(),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00122', Algorithm = MockConfGenerator(),
                      RequiredSelections = [ sel01, sel02])

    presel0 = MockConfGenerator()
    presel1 = MockConfGenerator()
    postsel0 = MockConfGenerator()
    postsel1 = MockConfGenerator()

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]

    seq = SelectionSequence('Seq03',
                            TopSelection = sel03,
                            EventPreSelector = presels,
                            PostSelectionAlgs = postsels,
                            sequencerType=DummySequencer)
    
    clone = seq.clone(name='clone')

    seqAlgos = clone.sequence().Members

    ref_algos = [presel0,
                 presel1,
                 _sel00.algorithm(),
                 _sel01.algorithm(),
                 _sel02.algorithm(),
                 _sel03.algorithm(),
                 sel02.algorithm(),
                 sel01.algorithm(),
                 sel03.algorithm(),
                 postsel0,
                 postsel1]

    assert len(seqAlgos) == len(ref_algos)
    assert presels == seqAlgos[:len(presels)]
    assert postsels == seqAlgos[len(ref_algos)-len(postsels):]
    # sel03 must come just before the post selection algos.
    assert sel03.algorithm() == ref_algos[len(ref_algos)-(len(presels)+1)]
    # order of sel01 and sel02 doesn't matter.
    for sel in [sel01, sel02]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

    seqAlgos = seq.algorithms()

    assert len(seqAlgos) == len(ref_algos)
    assert presels == seqAlgos[:len(presels)]
    assert postsels == seqAlgos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

def test_order_line() :

    
    sel000 = Selection('0.0000', Algorithm = MockConfGenerator(),
                      RequiredSelections = [])
    sel001 = Selection('0.0001', Algorithm = MockConfGenerator(),
                      RequiredSelections = [sel000])
    sel101 = Selection('1.0000', Algorithm = MockConfGenerator(),
                      RequiredSelections = [sel001])
    sel102 = Selection('1.0001', Algorithm = MockConfGenerator(),
                      RequiredSelections = [sel101])
    sel203 = Selection('2.0000', Algorithm = MockConfGenerator(),
                      RequiredSelections = [sel102])

    selSeq = SelectionSequence('Seq04', TopSelection = sel203)
    algs = selSeq.algorithms()
    assert [a.name() for a in algs] == ['0.0000', '0.0001', '1.0000', '1.0001', '2.0000']

def test_order_tree() :

    alg_names = SelectionTree.alg_names
    
    assert alg_names.index('0.00000') < alg_names.index('1.00000')
    assert alg_names.index('0.00001') < alg_names.index('1.00000')
    assert alg_names.index('0.00002') < alg_names.index('1.00001')
    assert alg_names.index('0.00003') < alg_names.index('1.00001')
    assert alg_names.index('1.00000') < alg_names.index('2.00000')
    assert alg_names.index('1.00001') < alg_names.index('2.00000')


def test_remove_duplicates() :
    
    sel000 = SelectionTree.sel000
    sel001 = SelectionTree.sel001
    sel002 = SelectionTree.sel002
    sel003 = SelectionTree.sel003

    sel100 = Selection('1.10000', Algorithm = MockConfGenerator(),
                       RequiredSelections = [sel000, sel001, sel002])
    
    sel101 = Selection('1.10001', Algorithm = MockConfGenerator(),
                       RequiredSelections = [sel001, sel002, sel003])
    
    sel200 = Selection('2.10000', Algorithm = MockConfGenerator(),
                       RequiredSelections = [sel100, sel101])

    selSeq =  SelectionSequence('Seq07', TopSelection = sel200)
    algs = selSeq.algorithms()
    alg_names = [a.name() for a in algs]

    assert len(algs) == 7
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

def test_selectionsequence_with_existing_configurable_name_raises() :

    fd = FilterDesktop('SelSeq00')
    
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    raises(NameError, SelectionSequence, 'SelSeq00', TopSelection = sel00 )

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
        
