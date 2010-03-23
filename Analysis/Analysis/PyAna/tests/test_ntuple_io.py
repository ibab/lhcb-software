#!/usr/bin/env python
'''Test suite for NTuple writing and reading.
'''
__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

import sys
sys.path.append('../python/PyAna')

from pyntuple.ntuple import NTuple
from pyntuple.write import gzip_save, save

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
    import gzip
    ntuplefile = gzip.open('test_compressed_pickle.ntp', 'rb')
    import pickle
    nt = pickle.load(ntuplefile)
    ref_nt = reference_ntuple()
    assert nt.column('x') == ref_nt.column('x')
    assert nt.column('sin') == ref_nt.column('sin')
    assert nt.column('cos') == ref_nt.column('cos')

