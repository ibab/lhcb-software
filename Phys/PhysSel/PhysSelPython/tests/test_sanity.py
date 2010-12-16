from py.test import raises

from PhysSelPython import __sanity__

from Configurables import FilterDesktop

def test_filterDesktop_properties() :
    fd0 = FilterDesktop('fd0', 
                        Code = 'TestCode0', 
                        InputLocations = ['a', 'b', 'c'],
                        OutputLevel=2)

    assert ( fd0.Code == 'TestCode0')
    assert ( fd0.InputLocations == ['a', 'b', 'c'])
    assert ( fd0.OutputLevel==2)

def test_duplicate_name_raises() :
    fd1 = FilterDesktop('fd1',
                        Code = 'TestCode1',
                        InputLocations = ['d', 'e', 'f'],
                        OutputLevel=2)

    raises(ValueError,
           FilterDesktop,
           'fd1',
           Code = 'TestCode1',
           InputLocations = ['d', 'e', 'f'],
           OutputLevel=2)
    
