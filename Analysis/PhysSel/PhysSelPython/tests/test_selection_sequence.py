#!/usr/bin/env python
#$Id: test_selection_sequence.py,v 1.14 2010-02-25 14:13:58 jpalac Exp $
'''
Test suite for SelectionSequence class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from PhysSelPython.Wrappers import ( Selection,
                                     SelectionSequence,
                                     AutomaticData,
                                     NameError         )
from PhysSelPython.Configurabloids import ( DummyAlgorithm,
                                            DummySequencer  )


def test_instantiate_sequencer() :
    sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg000')
    sel = Selection('00011', Algorithm = alg0,
                    RequiredSelections = [sel00, sel01])
    seq = SelectionSequence('Seq00', TopSelection = sel)


def test_sequencer_algos() :
    _sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    _sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    _sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    _sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')

    sel01 = Selection('00100', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00101', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00102', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])
    seq = SelectionSequence('Seq01',
                            TopSelection = sel03)

    seqAlgos = seq.algos

    assert len(seqAlgos) == 3
    for sel in [sel01, sel02, sel03] :
        assert sel.algorithm() in seqAlgos

def test_sequencer_sequence() :
    _sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    _sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    _sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    _sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')

    sel01 = Selection('000110', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('000111', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('000112', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])

    presel0 = DummyAlgorithm('Presel0')
    presel1 = DummyAlgorithm('Presel1')
    postsel0 = DummyAlgorithm('Postsel0')
    postsel1 = DummyAlgorithm('Postsel1')

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]
    seq = SelectionSequence('Seq0002',
                            TopSelection = sel03,
                            EventPreSelector = presels,
                            PostSelectionAlgs = postsels)

    seqAlgos = seq.sequence(sequencerType=DummySequencer).Members

    ref_algos = [presel0,
                 presel1,
                 sel02.algorithm(),
                 sel01.algorithm(),
                 sel03.algorithm(),
                 postsel0,
                 postsel1]

    assert len(seqAlgos) == 7

    assert presels == ref_algos[:len(presels)]

    assert postsels == ref_algos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

def test_clone_sequence() :
    _sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    _sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    _sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    _sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')

    sel01 = Selection('00120', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00121', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00122', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])

    presel0 = DummyAlgorithm('Presel00')
    presel1 = DummyAlgorithm('Presel01')
    postsel0 = DummyAlgorithm('Postsel00')
    postsel1 = DummyAlgorithm('Postsel01')

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]

    seq = SelectionSequence('Seq03',
                            TopSelection = sel03,
                            EventPreSelector = presels,
                            PostSelectionAlgs = postsels)
    
    clone = seq.clone('clone')

    seqAlgos = clone.sequence(sequencerType=DummySequencer).Members

    ref_algos = [presel0,
                 presel1,
                 sel02.algorithm(),
                 sel01.algorithm(),
                 sel03.algorithm(),
                 postsel0,
                 postsel1]

    assert len(seqAlgos) == 7
    assert presels == ref_algos[:len(presels)]
    assert postsels == ref_algos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

    seqAlgos = seq.algos

    assert len(seqAlgos) == 7
    assert presels == ref_algos[:len(presels)]
    assert postsels == ref_algos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

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
        
