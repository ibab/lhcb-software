"""
Classes and helper functions for histogramming data.

Copyright (c) 2010 Juan Palacios juan.palacios.puyana@gmail.com
Subject to the Lesser GNU Public License - see < http://www.gnu.org/licenses/lgpl.html>

"""
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

from __future__ import division

import copy
import histostats as _stats

__all__ = ('Histogram', 'Axis', 'Bin')

class Histogram(object) :

    def __init__(self, axis=None, nbins=100, min=0, max=0,label="") :
        if axis == None :
            self.axis = Axis(nbins, min, max, label)
        else:
            self.axis = copy.copy(axis)
        self._bins = [Bin(centre=self.axis.binCentre(b), 
                          width = self.axis.binWidth(b)) for b in xrange(self.axis.nbins+2)]

    def fill(self, x, weight=1) :
        if type(x) == list or type(x) == tuple :
            for entry in x : self._fill(entry, weight)
        else :
            self._fill(x,weight)

    def _fill(self, x, weight) :
        self._bins[self.axis.binIndex(x)].fill(weight)
    
    def binHeight(self, bin) :
        self.axis.checkIndex(bin)
        return self._bins[bin].height

    def binEntries(self, bin) :
        self.axis.checkIndex(bin)
        return self._bins[bin].entries

    def entries(self) :
        return sum([bin.entries for bin in self._bins])

    def integral(self) :
        return _stats.integral(self._bins[:len(self._bins)-2])

    def overflow(self) :
        return self._bins[self.axis.overflow_bin].entries

    def underflow(self) :
        return self._bins[self.axis.underflow_bin].entries

    def sigma(self) :
        return _stats.sigma(self.filledBins())

    def mean(self) :
        return _stats.mean(self.filledBins())

    def max(self) :
        return _stats._max(self.filledBins())

    def min(self) :
        return _stats._min(self.filledBins())

    def filledBins(self) :
        '''
        Return a list of bins which have been filled. Excludes underflow and overflow
        bins
        '''
        return filter(lambda bin : not bin.empty, self._bins[:len(self._bins)-2])
        
    def __add__(self, other) :
        return _arithmetic(self,other, lambda x, y : x+y)

    def __sub__(self, other) :
        return _arithmetic(self,other, lambda x, y : x-y)

    def __mul__(self, other) :
        return _arithmetic(self,other, lambda x, y : x*y)

    def __div__(self,other) :
        return _arithmetic(self,other, safe_divide)

    def __truediv__(self,other) :
        return _arithmetic(self,other, safe_divide)

class Bin(object) :

    def __init__(self, entries=0, height=0, centre=0, width=0) :
        self.entries=entries
        self.height=height
        self.centre=centre
        self.width = width
        self.empty = True

    def fill(self, weight) :
        self.entries += 1
        self.height += weight
        self.empty = False

    def __add__(self, other) :
        return self._binary_op(other, lambda x,y : x+y)

    def __sub__(self, other) :
        return self._binary_op(other, lambda x,y : x-y)

    def __mul__(self, other) :
        return self._binary_op(other, lambda x,y : x*y)

    def __div__(self,other) :
        return self._binary_op(other, safe_divide)

    def __truediv__(self,other) :
        return self._binary_op(other, safe_divide)

    def _binary_op(self, other, binary_fun) :
        self.empty = self.empty and other.empty
        return Bin(entries = binary_fun(self.entries, other.entries),
                   height = binary_fun(self.height, other.height),
                   centre = self.centre)

class Axis(object) :
    '''
    A simple, fixed bin histogram axis
    '''
    def __init__(self, nbins, min, max, label = "") :
        self.nbins = nbins
        self.min = float(min)
        self.max = float(max)
        self.range = (self.min, self.max)
        self._bin_width = (self.max - self.min)/self.nbins
        self.label = label
        self.underflow_bin = nbins
        self.overflow_bin = nbins+1

    def binIndex(self, x) :
        if x >= self.max :
            return self.overflow_bin
        elif x <= self.min :
            return self.underflow_bin
        else :
            return int((x-self.min)/self._bin_width)

    def binCentre(self, index) :
        self.checkIndex(index)
        return self.min + (index+0.5)*(self._bin_width)

    def binWidth(self, index=0) :
        self.checkIndex(index)
        return self._bin_width

    def checkIndex(self, index) :
        totalBins = self.nbins + 2
        if index < 0 or index >= totalBins :
            raise IndexError('Bin '+str(index)+' not in range [0,'+str(totalBins-1)+']')

    def inRange(self, x) :
        return x > self.min and x < self.max

    def compatible(self, rhs) :
        return  (self.nbins, 
                 self.min, 
                 self.max ) ==  (rhs.nbins, 
                                 rhs.min, 
                                 rhs.max)

    def __eq__(self, rhs) :
        if type(rhs) != Axis:
            return False
        return (self.nbins, 
                self.min, 
                self.max, 
                self.label) ==  (rhs.nbins, 
                                 rhs.min, 
                                 rhs.max, 
                                 rhs.label)

    def __ne__(self, rhs) :
        return not self.__eq__(rhs)

def _arithmetic(a,b, binary_op) :
    if (Axis.compatible(a.axis, b.axis)) :
        tmp = Histogram(a.axis)
        tmp._bins = _arithmetic_on_bins(a._bins, b._bins, binary_op)
        return tmp
    else :
        raise IndexError('Incompatible histogram axis paremeters')

def _arithmetic_on_bins(a,b, binary_op) :
    return [ binary_op(x,y) for x, y in zip(a, b)]



def safe_divide(a,b) :
    if b == 0 : 
        return 0
    else : 
        return a/b 
