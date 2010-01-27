import sys
sys.path.append('../python')
from PhysSelPython.Configurabloids import *

def test_instantiate_ConfigurableGenerator() :
    confMap = ConfigurableGenerator()

def test_ConfigurableGenerator_is_singleton() :
    confGen0 = ConfigurableGenerator()
    confGen1 = ConfigurableGenerator()
    assert confGen0.__dict__ == confGen1.__dict__


def test_make_new_configurable() :
    dummy = DummyAlgorithm('Dummy',
                           InputLocations = ['sel0', 'sel1'])
    assert dummy.name() == 'Dummy'
    assert dummy.InputLocations == ['sel0', 'sel1']
    assert type(dummy) == DummyAlgorithm

    
def test_change_DummyAlgorithm_InputLocations() :
    dummy0 = DummyAlgorithm('Dummy01',
                            InputLocations = ['sel0', 'sel1'])
    assert dummy0.name() == 'Dummy01'
    assert dummy0.InputLocations == ['sel0', 'sel1']
    assert type(dummy0) == DummyAlgorithm

    dummy0.InputLocations = ['sel2', 'sel3']
    assert dummy0.InputLocations == ['sel2', 'sel3']
    
def test_get_existing_DummyAlgorithm() :
    d = DummyAlgorithm('Dummy0', InputLocations = ['sel0', 'sel1'])
    dummy1 = DummyAlgorithm('Dummy0')
    assert dummy1.name() == 'Dummy0'
    assert dummy1.InputLocations == ['sel0', 'sel1']
    assert type(dummy1) == DummyAlgorithm

def test_get_existing_configurabloid_with_wrong_type_raises_TypeError() :
    confGen = ConfigurableGenerator()
    dummy1 = confGen('Dummy03', DummyAlgorithm)

    try :
        seq0 = confGen('Dummy03', DummySequencer)
    except TypeError :
        print "TypeError caught"

def test_get_existing_DummyAlgorithm_with_new_InputLocations() :
    confGen = ConfigurableGenerator()
    dummy = DummyAlgorithm('Dummy02',
                           InputLocations = ['sel0', 'sel1'])

    dummy1 = DummyAlgorithm('Dummy02',
                            InputLocations = ['clone0','clone1'])

    assert dummy1.name() == 'Dummy02'
    assert dummy1.InputLocations == ['clone0', 'clone1']
    assert type(dummy1) == DummyAlgorithm
