#!/usr/bin/env python
'''
Test suite for histogram.axis class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
sys.path.append('../python/PyAna')

from pyhistogram.histogram import Histogram, Axis

def reference_histogram() :
    return Histogram(Axis(100, -50, 50))

def test_instantiate_from_axis() :
    h = Histogram(axis=Axis(100,-50, 50, "My first axis"))

def test_instantiate_from_keywords() :
    h = Histogram(nbins=100, min=-50, max=50, label="My first axis")

def test_instantiate() :
    h = reference_histogram()

def test_empty_histogram() :
    h = Histogram(axis=Axis(100,-50, 50))
    assert h.entries() == 0.
    assert h.sigma() == 0
    assert h.mean() == 0
    assert h.overflow() == 0
    assert h.underflow() ==0

def test_fill():
    h = Histogram(axis=Axis(100, -50, 50))
    weight = 0.25
    for i in xrange(-50,50) :
        h.fill(i+0.5, weight)
    assert h.entries() == 100
    assert h.integral() == 100*weight

def test_fill_from_list():
    h = Histogram(axis=Axis(100, -50, 50))
    x = [float(i) for i in xrange(-49,50)]
    h.fill(x)
    assert h.entries() == 99

def test_fill_with_weight():
    import math
    h = Histogram(axis=Axis(100, -50, 50))
    for i in xrange(-50,50) :
        w = math.sin(i)
        h.fill(i, w)

def test_mean() :
    h0 = Histogram(axis=Axis(100, -50, 50))
    h1 = Histogram(axis=Axis(100, -50, 50))
    for x in xrange(-50, 50) :
        h0.fill(x + 0.5)
        if x + 0.5 > 0 :
            h1.fill(x+0.5)
    assert h1.mean() == 25.
    assert h0.mean() == 0

def test_underflow() :
    h = Histogram(axis=Axis(100, -50., 50.))
    assert h.underflow() == 0.
    h.fill(-100.)
    assert h.underflow() == 1.

def test_overflow() :
    h = Histogram(axis=Axis(100, -50., 50.))
    assert h.overflow() ==0
    h.fill(100.)
    h.fill(200.)
    h.fill(10.)
    assert h.overflow() == 2.

def test_binheight() :
    h = Histogram(axis=Axis(100, -50., 50.))
    weight = 0.5
    h.fill(-49.99, weight) # should be 0th bin
    h.fill(49.99, weight) # should be 100th bin, ie index 99
    h.fill(0.1, 2*weight)
    assert h.binHeight(0) == weight
    assert h.binHeight(99) == weight
    assert h.binHeight(50) == 2*weight

def test_out_of_range_binheight() :
    h = Histogram(axis=Axis(100, -50., 50.))
    try :
        h.binHeight(100)
    except IndexError :
        pass
    try :
        h.binHeight(-1)
    except IndexError :
        pass

def test_integral() :
    h = Histogram(axis=Axis(100, -50., 50.))
    weight = 0.5
    for x in xrange(10) :
        h.fill(-49.99, weight) # should be 0th bin
        h.fill(49.99, weight) # should be 100th bin, ie index 99
        h.fill(0.1, 2*weight)
    assert h.integral() == 10*4*weight

def test_integral_does_not_count_out_of_range_entries() :
    h = Histogram(axis=Axis(100, -50., 50.))
    h.fill(45.5, 10.)
    h.fill(100.)
    h.fill(-100)
    assert h.integral() == 10.

def test_entries() :
    h = Histogram(axis=Axis(100, -50., 50.))
    weight = 0.5
    for x in xrange(10) :
        h.fill(-49.99, weight) # should be 0th bin
        h.fill(49.99, weight) # should be 100th bin, ie index 99
        h.fill(0.1, 2*weight)
    assert h.entries() == 30

def test_entries_does_not_count_out_of_range_entries() :
    h = Histogram(axis=Axis(100, -50., 50.))
    h.fill(45.5, 10.)
    h.fill(100.)
    h.fill(-100)
    assert h.entries() == 3.

def test_add_histograms() :
    h0 = Histogram(axis=Axis(100, -50., 50., label = "axis 0"))
    weight = 0.5
    h0.fill([-49.99,49.99], weight) # should be 0th bin
    h0.fill(0.1, 2*weight)
    h0.fill(-75., 32.)
    h0.fill(100., 55)
    h1 = Histogram(axis=Axis(100, -50., 50., label = "axis 1"))
    h1.fill([-48.99,48.99], weight) # should be 0th bin
    h1.fill(0.1, 2*weight)
    h1.fill(-101.)
    h1.fill(200.,2)
    h2 = h0 + h1
    
    assert h2.axis == h0.axis
    assert h2.entries() == h0.entries() + h1.entries()
    assert h2.overflow() == h0.overflow() + h1.overflow()
    assert h2.underflow() == h0.underflow() + h1.underflow()

    for bin in xrange(h2.axis.nbins):
        assert h2.binHeight(bin) == h0.binHeight(bin) + h1.binHeight(bin)

def test_subtract_histograms() :
    h0 = Histogram(axis=Axis(100, -50., 50.))
    weight = 0.5
    h0.fill([-49.99,49.99,-48.99,48.99], weight) # should be 0th bin
    h0.fill(0.1, 4*weight)
    h0.fill(-75., 32.)
    h0.fill(100., 55)
    h1 = Histogram(axis=Axis(100, -50., 50.))
    h1.fill([-48.99,48.99], weight) # should be 0th bin
    h1.fill(0.1, 2*weight)
    h1.fill(-101.)
    h1.fill(200.,2)
    h2 = h0 - h1
    
    assert h2.axis == h0.axis
    assert h2.entries() == h0.entries() - h1.entries()
    assert h2.overflow() == h0.overflow() - h1.overflow()
    assert h2.underflow() == h0.underflow() - h1.underflow()

    for bin in xrange(h2.axis.nbins):
        assert h2.binHeight(bin) == h0.binHeight(bin) - h1.binHeight(bin)

def test_divide_histograms() :
    from random import gauss
    h0 = Histogram(axis=Axis(100, -50., 50.))
    h1 = Histogram(axis=Axis(100, -50., 50.))
    dist = [gauss(0.,10) for x in xrange(100)]
    h0.fill(dist)
    h1.fill(dist, 5.)
    h2 = h0/h1
    for bin in xrange(h2.axis.nbins):
        if h1.binHeight(bin) != 0 :
            assert h2.binHeight(bin) == h0.binHeight(bin) / h1.binHeight(bin)

def test_multiply_histograms() :
    from random import gauss
    h0 = Histogram(axis=Axis(100, -50., 50.))
    h1 = Histogram(axis=Axis(100, -50., 50.))
    dist = [gauss(0.,20) for x in xrange(100)]
    h0.fill(dist)
    h1.fill(dist, 2.5)
    h2 = h0*h1
    for bin in xrange(h2.axis.nbins):
        assert h2.binHeight(bin) == h0.binHeight(bin) * h1.binHeight(bin)

def test_axis_is_local() :
    h0 = Histogram(axis=Axis(100, -50., 50.))
    assert h0.axis.nbins == 100
    h1 = Histogram(h0.axis)
    h1.axis.nbins = 50
    assert h1.axis.nbins == 50
    assert h0.axis.nbins == 100

def test_axis_is_local_in_arithmetic_operation() :
    h0 = Histogram(axis=Axis(100, -50., 50.))
    h1 = Histogram(axis=Axis(100, -50., 50.))
    assert h0.axis.nbins == 100
    h2 = h0 + h1
    h2.axis.nbins = 50
    assert h2.axis.nbins == 50
    assert h0.axis.nbins == 100

def test_sigma() :
    h0 = Histogram(axis=Axis(100, -50., 50.))
    h0.fill([5.5,-5.5,100.5,-100.5])
    assert h0.sigma() == 5.5

def test_min_and_max() :
    h0 = Histogram(axis=Axis(20, -10., 10.))
    h0.fill([-5.5,6.5])
    assert h0.min() == -6.0
    assert h0.max() == 7.0
