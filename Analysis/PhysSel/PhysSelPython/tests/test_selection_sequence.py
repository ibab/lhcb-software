#$Id: test_selection_sequence.py,v 1.1 2010-01-27 14:42:10 jpalac Exp $
'''
Test suite for SelectionSequence class.
'''

__author__ "Juan PALACIOS juan.palacios@nikhef.nl"
import sys
sys.path.append('../python')
from PhysSelPython.Wrappers import Selection, SelectionSequence, AutomaticData, NameError
from PhysSelPython.Configurabloids import DummyAlgorithm, DummySequencer

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
                            TopSelection = sel03,
                            SequencerType = DummySequencer)

    seqAlgos = seq.algos

    assert len(seqAlgos) == 3
    for sel in [sel01, sel02, sel03] :
        assert sel.algorithm() in seqAlgos

def test_sequencer_sequence() :
    _sel00 = AutomaticData('Sel00', Location = 'Phys/Sel00')
    _sel01 = AutomaticData('Sel01', Location = 'Phys/Sel01')
    _sel02 = AutomaticData('Sel02', Location = 'Phys/Sel02')
    _sel03 = AutomaticData('Sel03', Location = 'Phys/Sel03')

    sel01 = Selection('00110', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00111', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00112', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])
    seq = SelectionSequence('Seq02',
                            TopSelection = sel03,
                            SequencerType = DummySequencer)

    seqAlgos = seq.sequence().Members

    assert len(seqAlgos) == 3
    for sel in [sel01, sel02, sel03] :
        assert sel.algorithm() in seqAlgos
