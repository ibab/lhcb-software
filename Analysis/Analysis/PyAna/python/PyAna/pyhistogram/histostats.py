'''Statistical functions operating on sequences of Bins.
'''
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


