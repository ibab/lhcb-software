#!/usr/bin/env python
'''Test suite for NTuple class
'''
__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

import sys
sys.path.append('../python')

from PyAna.pyntuple.ntuple import NTuple

def test_instantiate() :
    nt = NTuple('A', 'B')

def test_fill_column() :
    nt = NTuple('A', 'B')
    nt.fill('A', 5)

def test_access_row() :
    nt = NTuple('A','B')
    nt.fill('A', 5)
    rw = nt.row(0)
    assert rw.A == 5
    nt.fill('B',6)
    assert rw.A==5 and rw.B == 6

def test_illegal_tag_fails() :
    nt = NTuple('A','B')
    try :
        nt.fill('C', 5)
    except KeyError :
        pass

def test_fill_column_twice() :
    nt = NTuple('A', 'B')
    nt.fill('A', 5)
    nt.fill('A', 7)

def test_access_latest_row() :
    nt = NTuple('A', 'B')
    nt.fill('A', 5)
    rw = nt.row(0)
    assert rw.A == 5
    nt.fill('A', 7)
    rw = nt.row(0)
    assert rw.A == 7
    nt.write()
    nt.fill('A', 22)
    rw = nt.row(1)
    assert rw.A == 22

def test_get_column() :
    nt = NTuple('A', 'B')
    for x in range(100) :
        nt.fill('A', x)
        nt.fill('B', x*100)
        nt.write()
    A = nt.column('A')
    assert A == range(100)
    B = nt.column('B')
    assert B == [ x*100 for x in range(100)]

def test_get_with_cut() :
    nt = NTuple('A', 'B')
    for x in range(100) :
        nt.fill('A', x)
        nt.fill('B', x*100)
        nt.write()
    col = nt.column('A',lambda x : x.A > 50 )
    assert col == range(51, 100)

def test_get_with_cross_cut() :
    nt = NTuple('A', 'B')
    for x in range(100) :
        nt.fill('A', x)
        nt.fill('B', x*100)
        nt.write()
    col = nt.column('B',lambda x : x.A > 50 and x.B < 8500 )
    assert col == [x*100 for x in xrange(51, 85)]

def test_add_ntuples() :
    nt0 = reference_ntuple()
    nt1 = reference_ntuple()
    assert len(nt0.column('sin')) == 101
    assert len(nt1.column('sin')) == 101
    nt2 = nt0+nt1
    assert len(nt2.column('sin')) == 202
    assert len(nt2.column('cos')) == 202

    assert len(nt0.column('sin')) == 101
    assert len(nt1.column('sin')) == 101

def test_illegal_add_raises_KeyError() :
    nt0 = NTuple('A', 'B')
    nt1 = NTuple('C', 'D')
    try :
        nt2 = nt0 + nt1
    except KeyError :
        pass

def reference_ntuple() :
    nt = NTuple('x', 'sin', 'cos')
    import math
    for x in xrange(-50,51) :
        nt.fill('x',x)
        nt.fill('sin', math.sin(x))
        nt.fill('cos', math.cos(x))
        nt.write()
    return nt

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
