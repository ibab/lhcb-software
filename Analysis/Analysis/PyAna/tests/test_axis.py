#!/usr/bin/env python
'''
Test suite for histogram.axis class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python/PyAna')

from pyhistogram.histogram import Axis

def test_instantiate_axis() :
    ax = Axis(100, -50, 50, "My first axis")

def test_axis_parameters() :
    ax = Axis(100, -50, 50, "My first axis")
    assert ax.nbins == 100
    assert ax.min == -50.
    assert ax.max == 50.
    assert ax.label == "My first axis"
    assert ax.range == (-50,50)
def test_axis_bin_width() :
    ax = Axis(100, -50, 50, "My first axis")
    assert ax.binWidth() == 1.

def test_axis_bin_centre() :
    ax = Axis(20, -10, 10, "My first axis")
    reference = [x+0.5 for x in xrange(-10,10)]
    for i in xrange(10) :
        assert ax.binCentre(i) == reference[i]

def test_invalid_bin_center_raises_IndexError() :
    ax = Axis(20, -10, 10, "My first axis")
    try :
        ax.binCentre(100)
    except IndexError :
        print 'IndexError caught'

def test_invalid_range_raises_ValueError() :
    ax = Axis(20, -10, 10, "My first axis")
    try :
        ax.binIndex(11.)
    except ValueError :
        print 'ValueError caught'

def test_axis_equality() :
    ax0 = Axis(20, -10, 10, "My first axis")
    ax1 = Axis(20, -10, 10, "My first axis")
    assert ax0==ax1

def test_axis_inequality() :
    ax0 = Axis(20, -10, 10, "My first axis")
    ax1 = Axis(30, -10, 10, "My first axis")
    assert ax0!=ax1

def test_compare_to_Nonsense() :
    ax0 = Axis(20, -10, 10, "My first axis")
    assert ax0 != None
    assert not ax0==None
    assert ax0 != 1
    assert ax0 !=5.0
    assert ax0 != []
