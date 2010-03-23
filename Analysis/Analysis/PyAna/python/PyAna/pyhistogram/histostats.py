'''
Statistical functions operating on sequences of Bins.

Copyright (c) 2010 Juan Palacios juan.palacios.puyana@gmail.com
Subject to the Lesser GNU Public License - see < http://www.gnu.org/licenses/lgpl.html>

'''
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

from math import sqrt

__all__ = ('integral', 'mean', 'sigma', '_max', '_min')

def integral(bins) :
    return sum([bin.height for bin in bins])

def mean(bins) :
    if len(bins) == 0 :
        return 0
    weighted_total = 0
    for bin in bins :
        weighted_total += bin.height*bin.centre
    return weighted_total/integral(bins)

def sigma(bins) :
    mu = mean(bins)
    if len(bins) == 0 :
        return 0
    return sqrt(sum([((b.height*b.centre)-mu)**2 for b in bins])/integral(bins))

def _max(bins) :
    return max([bin.centre + bin.width/2. for bin in bins])

def _min(bins) :
    return min([bin.centre - bin.width/2. for bin in bins])


