#!/usr/bin/env python
'''Test suite for NTuple writing and reading.
'''
__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

import sys
sys.path.append('../python')

from PyAna.pyntuple.ntuple import NTuple
from PyAna.pyntuple.write import gzip_save, save

def reference_ntuple() :
    nt = NTuple('x', 'sin', 'cos')
    import math
    for x in xrange(-50,51) :
        nt.fill('x',x)
        nt.fill('sin', math.sin(x))
        nt.fill('cos', math.cos(x))
        nt.write()
    return nt

def test_pickle() :
    nt = reference_ntuple()
    save(nt, 'test_pickle.ntp')

def test_unpickle() :
    ntuplefile = open('test_pickle.ntp', 'r')
    import pickle
    nt = pickle.load(ntuplefile)
    ref_nt = reference_ntuple()
    assert nt.column('x') == ref_nt.column('x')
    assert nt.column('sin') == ref_nt.column('sin')
    assert nt.column('cos') == ref_nt.column('cos')

def test_compressed_pickle() :
    nt = reference_ntuple()
    gzip_save(nt, 'test_compressed_pickle.ntp')

def test_compressed_unpickle() :
    test_compressed_pickle()
    import gzip
    ntuplefile = gzip.open('test_compressed_pickle.ntp', 'rb')
    import pickle
    nt = pickle.load(ntuplefile)
    ref_nt = reference_ntuple()
    assert nt.column('x') == ref_nt.column('x')
    assert nt.column('sin') == ref_nt.column('sin')
    assert nt.column('cos') == ref_nt.column('cos')


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
