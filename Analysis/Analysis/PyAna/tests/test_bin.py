#!/usr/bin/env python
'''
Test suite for histogram.bin class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python')

from PyAna.pyhistogram.histogram import Bin

def test_bin_contents() :
    bin = Bin(entries = 100, height = 75.)
    assert bin.entries == 100
    assert bin.height == 75.

def test_bin_addition() :
    bin0 = Bin(entries = 50, height = 75.)
    bin1 = Bin(entries = 50, height = 75.)
    bin2 = bin0+bin1
    assert bin2.entries == 100 and bin2.height == 150.

def test_bin_subtraction() :
    bin0 = Bin(entries = 100, height = 200.)
    bin1 = Bin(entries = 50, height = 75.)
    bin2 = bin0-bin1
    assert bin2.entries == 50 and bin2.height == 125.

def test_bin_division() :
    bin0 = Bin(entries = 100, height = 200.)
    bin1 = Bin(entries = 2, height = 100.)
    bin2 = bin0/bin1
    assert bin2.entries == 50 and bin2.height == 2.

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
