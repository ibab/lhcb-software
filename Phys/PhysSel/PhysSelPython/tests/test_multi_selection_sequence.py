#!/usr/bin/env python
#$Id$
'''
Test suite for MultiSelectionSequence class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
from py.test import raises
sys.path.append('../python')

from PhysSelPython.Wrappers import ( Selection,
                                     SelectionSequence,
                                     MultiSelectionSequence,
                                     AutomaticData,
                                     NameError)

from SelPy.configurabloids import ( MockConfGenerator,
                                    DummySequencer  )

from Configurables import FilterDesktop

def test_instantiate_dataondemand_multi_sequencer() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    seq00 = SelectionSequence('Seq00x', TopSelection = sel00)
    seq01 = SelectionSequence('Seq01x', TopSelection = sel01)
    seq = MultiSelectionSequence('MultiSeq00x', Sequences = [seq00, seq01])

def test_call_important_methods() :
    sel00 = AutomaticData(Location = 'Phys/Sel00/Particles')
    sel01 = AutomaticData(Location = 'Phys/Sel01/Particles')
    seq00 = SelectionSequence('Seq00y', TopSelection = sel00)
    seq01 = SelectionSequence('Seq01y', TopSelection = sel01)
    seq = MultiSelectionSequence('MultiSeq00y', Sequences = [seq00, seq01])
    assert seq.outputLocations() == ['Phys/Sel00/Particles',
                                     'Phys/Sel01/Particles']
    algs = seq.algorithms()
    seq = seq.sequence()
    
def test_instantiate_multi_sequencer() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    alg00 = MockConfGenerator()
    alg01 = MockConfGenerator()
    sel00 = Selection('100011', Algorithm = alg00,
                     RequiredSelections = [sel00])
    sel01 = Selection('100012', Algorithm = alg01,
                     RequiredSelections = [sel01])
    seq00 = SelectionSequence('Seq000', TopSelection = sel00)
    seq01 = SelectionSequence('Seq001', TopSelection = sel01)
    seq = MultiSelectionSequence('MultiSeq00z', Sequences = [seq00, seq01])

def test_multi_sequencer_sequences() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel00_01 = Selection('0000110', Algorithm = MockConfGenerator(),
                         RequiredSelections = [_sel00, _sel01])
    sel02_03 = Selection('0000111', Algorithm = MockConfGenerator(),
                         RequiredSelections = [_sel02, _sel03])
    selA = Selection('000112A', Algorithm = MockConfGenerator(),
                     RequiredSelections = [ sel00_01, sel02_03])

    _sel04 = AutomaticData(Location = 'Phys/Sel04')
    _sel05 = AutomaticData(Location = 'Phys/Sel05')
    _sel06 = AutomaticData(Location = 'Phys/Sel06')
    _sel07 = AutomaticData(Location = 'Phys/Sel07')    
    sel04_05 = Selection('0000112', Algorithm = MockConfGenerator(),
                         RequiredSelections = [_sel04, _sel05])
    sel06_07 = Selection('0000113', Algorithm = MockConfGenerator(),
                         RequiredSelections = [_sel06, _sel07])
    selB = Selection('000112B', Algorithm = MockConfGenerator(),
                     RequiredSelections = [ sel04_05, sel06_07])

    presel0 = MockConfGenerator()
    presel1 = MockConfGenerator()
    presel2 = MockConfGenerator()
    presel3 = MockConfGenerator()
    
    postsel0 = MockConfGenerator()
    postsel1 = MockConfGenerator()
    postsel2 = MockConfGenerator()
    postsel3 = MockConfGenerator()

    preselsA =  [presel0, presel1]
    postselsA = [postsel0, postsel1]
    preselsB =  [presel2, presel3]
    postselsB = [postsel2, postsel3]

    seqA = SelectionSequence('Seq0002A',
                             TopSelection = selA,
                             EventPreSelector = preselsA,
                             PostSelectionAlgs =postselsA)
    seqB = SelectionSequence('Seq0002B',
                             TopSelection = selB,
                             EventPreSelector = preselsB,
                             PostSelectionAlgs =postselsB)

    multiSeq = MultiSelectionSequence('MultiSeqAB',
                                      Sequences = [seqA, seqB],
                                      sequencerType=DummySequencer)

    assert multiSeq.outputLocations() == [selA.outputLocation(),
                                          selB.outputLocation()]

    print 'algos in seqA', len(seqA.algorithms())
    print 'algos in seqB', len(seqB.algorithms())
    
    seqAlgos = multiSeq.sequence().Members
    seqAlgosA = seqAlgos[0].Members
    seqAlgosB = seqAlgos[1].Members
    assert len(seqAlgos) == len(multiSeq._sequences)
    assert len(multiSeq.algorithms()) == len(seqAlgosA)+len(seqAlgosB)
    
    ref_algosA = [presel0,
                  presel1,
                  _sel00.algorithm(),
                  _sel01.algorithm(),
                  _sel02.algorithm(),
                  _sel03.algorithm(),
                  sel02_03.algorithm(),
                  sel00_01.algorithm(),
                  selA.algorithm(),
                  postsel0,
                  postsel1]

    ref_algosB = [presel2,
                  presel3,
                  _sel04.algorithm(),
                  _sel05.algorithm(),
                  _sel06.algorithm(),
                  _sel07.algorithm(),
                  sel06_07.algorithm(),
                  sel04_05.algorithm(),
                  selB.algorithm(),
                  postsel2,
                  postsel3]


    assert len(seqAlgos) == 2

    assert preselsA == seqAlgosA[:len(preselsA)]
    assert preselsB == seqAlgosB[:len(preselsB)]
    # selA must be just before postsels
    assert selA.algorithm() == ref_algosA[len(ref_algosA)-(len(postselsA)+1)]
    assert postselsA == seqAlgosA[len(ref_algosA)-len(postselsA):]
    assert postselsB == seqAlgosB[len(ref_algosB)-len(postselsB):]
    # selB must be just before postsels
    assert selB.algorithm() == ref_algosB[len(ref_algosB)-(len(postselsB)+1)]

    # order doesn't matter
    for sel in [sel04_05, sel06_07]:
        assert sel.algorithm() in ref_algosB[len(preselsB):len(ref_algosB)-len(postselsB)]
    for sel in [sel00_01, sel02_03]:
        assert sel.algorithm() in ref_algosA[len(preselsA):len(ref_algosA)-len(postselsA)]


def test_multiselectionsequence_with_existing_configurable_name_raises() :

    fd = FilterDesktop('MultiSeq00')
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    seq00 = SelectionSequence('SelSeq00x', TopSelection = sel00)
    seq01 = SelectionSequence('SelSeq01x', TopSelection = sel01)
    
    raises(NameError,
           MultiSelectionSequence,
           'MultiSeq00',
           Sequences = [seq00,seq01] )

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
        
