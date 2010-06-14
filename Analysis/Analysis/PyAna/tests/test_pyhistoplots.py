#!/usr/bin/env python
'''
Test suite for histogram.axis class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

import math
from random import gauss
from PyAna.pyntuple.ntuple import NTuple
from matplotlib import pyplot
from PyAna.pyhistoplots import ntuple_plot, histo_plot, ntuple_column_histo

mu_p = 15.
mu_pt = 5.
sigma_p = 10.
sigma_pt = 5.

nt = NTuple('x', 'p', 'pt')

for x in xrange(10000) :
    nt.fill('x',x)
    nt.fill('p', gauss(mu_p, sigma_p))
    nt.fill('pt', gauss(mu_pt, sigma_pt))
    nt.write()

def test_ntuple_plot() :
    pt_plot = ntuple_plot(nt, 'pt', show = False)

def test_ntuple_column_histo():
    h_pt = ntuple_column_histo(nt, 'pt')

def test_histo_plot() :
    h_pt = ntuple_column_histo(nt, 'pt')
    histo_plot(h_pt, color='blue', show = False)


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
