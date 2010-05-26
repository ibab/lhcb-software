#!/usr/bin/env python
'''
 Test suite for StrippingLine configuration
'''
__author__ = 'Juan Palacios juan.palacios@nikhef.nl'

from StrippingConf.Configuration import StrippingLine, StrippingStream, StrippingConf

from PhysSelPython.Wrappers import AutomaticData

def test_instantiate_empty_line() :
    line = StrippingLine('EmptyLine')
    assert line.name() == 'StrippingEmptyLine'
    assert line.subname() == 'EmptyLine'
    assert line.outputSelection() == None
    assert line.isAppended() == False
    assert line.outputLocation() == None
    
def test_instantiate_from_Selection_object() :
    selection = AutomaticData(Location = 'Phys/Selection')
    line = StrippingLine('SelectionLine', algos = [selection])
    assert line.name() == 'StrippingSelectionLine'
    assert line.subname() == 'SelectionLine'
    assert line.outputSelection() == 'Selection'
    assert line.isAppended() == False
    assert line.outputLocation() == 'Phys/Selection'
    
def test_duplicate_name_raises_ValueError() :
    line0 = StrippingLine('DuplicateLine')
    try :
        line1 = StrippingLine('DuplicateLine')
        assert(False)
    except ValueError :
        pass

def test_filterMembers() :
    '''
    It is necessary to construct a StrippingStream and a StrippingConf
    to test the filterMembers of the line. The line by itself is useless.
    '''
    selection = AutomaticData(Location = 'Phys/Selection')
    name = 'FilterMembersTest'
    line = StrippingLine('FilterMembersTest', algos = [selection])
    assert line.name() == 'Stripping' + name
    assert line.subname() == name
    assert line.outputSelection() == 'Selection'
    assert line.isAppended() == False
    assert line.outputLocation() == 'Phys/Selection'
    stream = StrippingStream('stream', Lines = [line])
    conf = StrippingConf('conf', Streams = [stream])
    assert  [m.name() for m in line.filterMembers()] == ['checkPV', 'Selection']

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
