#!/usr/bin/env python
#$Id: test_multi_selection_sequence.py,v 1.1 2010-05-04 13:55:00 jpalac Exp $
'''
Test suite for SelectionSequence class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from PhysSelPython.Wrappers import ( Selection,
                                     SelectionSequence,
                                     MultiSelectionSequence,
                                     AutomaticData )

from SelPy.configurabloids import ( DummyAlgorithm,
                                    DummySequencer  )

def test_instantiate_dataondemand_multi_sequencer() :
    sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    seq00 = SelectionSequence('Seq00', TopSelection = sel00)
    seq01 = SelectionSequence('Seq01', TopSelection = sel01)
    seq = MultiSelectionSequence('Seq00', Sequences = [seq00, seq01])
    assert seq.outputLocations() == ['Phys/Sel00', 'Phys/Sel01']

def test_instantiate_sequencer() :
    sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    alg00 = DummyAlgorithm('Alg000')
    alg01 = DummyAlgorithm('Alg001')
    sel00 = Selection('100011', Algorithm = alg00,
                     RequiredSelections = [sel00])
    sel01 = Selection('100012', Algorithm = alg01,
                     RequiredSelections = [sel01])
    seq00 = SelectionSequence('Seq000', TopSelection = sel00)
    seq01 = SelectionSequence('Seq001', TopSelection = sel01)
    seq = MultiSelectionSequence('MultiSeq00', Sequences = [seq00, seq01])

def test_multi_sequencer_sequences() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')
    _sel04 = AutomaticData(Location = 'Phys/Sel04')
    _sel05 = AutomaticData(Location = 'Phys/Sel05')
    _sel06 = AutomaticData(Location = 'Phys/Sel06')
    _sel07 = AutomaticData(Location = 'Phys/Sel07')

    sel00_01 = Selection('000110', Algorithm = DummyAlgorithm('Alg00_01'),
                         RequiredSelections = [_sel00, _sel01])
    sel02_03 = Selection('000111', Algorithm = DummyAlgorithm('Alg02_03'),
                         RequiredSelections = [_sel02, _sel03])
    sel04_05 = Selection('000112', Algorithm = DummyAlgorithm('Alg04_05'),
                         RequiredSelections = [_sel04, _sel05])
    sel06_07 = Selection('000113', Algorithm = DummyAlgorithm('Alg06_07'),
                         RequiredSelections = [_sel06, _sel07])

    selA = Selection('000112A', Algorithm = DummyAlgorithm('Alg001A'),
                     RequiredSelections = [ sel00_01, sel02_03])
    selB = Selection('000112B', Algorithm = DummyAlgorithm('Alg001B'),
                     RequiredSelections = [ sel04_05, sel06_07])

    presel0 = DummyAlgorithm('Presel0')
    presel1 = DummyAlgorithm('Presel1')
    presel2 = DummyAlgorithm('Presel2')
    presel3 = DummyAlgorithm('Presel3')
    
    postsel0 = DummyAlgorithm('Postsel0')
    postsel1 = DummyAlgorithm('Postsel1')
    postsel2 = DummyAlgorithm('Postsel2')
    postsel3 = DummyAlgorithm('Postsel3')

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]
    seqA = SelectionSequence('Seq0002A',
                             TopSelection = selA,
                             EventPreSelector = [presel0,presel1],
                             PostSelectionAlgs =[postsel0, postsel1])
    seqB = SelectionSequence('Seq0002B',
                             TopSelection = selB,
                             EventPreSelector = [presel0,presel1],
                             PostSelectionAlgs =[postsel2, postsel3])

    multiSeq = MultiSelectionSequence('MultiSeqAB', Sequences = [seqA, seqB])

    assert multiSeq.outputLocations() == [selA.outputLocation(),
                                          selB.outputLocation()]

    print 'algos in seqA', len(seqA.algos)
    print 'algos in seqB', len(seqB.algos)
    
    seqAlgos = multiSeq.sequence(sequencerType=DummySequencer).Members
    assert len(seqAlgos) == len(multiSeq.sequences)

    '''
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
    '''

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
        
