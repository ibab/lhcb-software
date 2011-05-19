"""
Module with all former stripping selection line builder modules.
All line builders available via function lineBuilders(stripping).
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

#import known strippings
import Stripping13


#compile dictionary of all known strippings
from sys import modules as _modules
_this = _modules[__name__]

_strippings={}

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

for _k in _strippingKeys:
    _strippings[_k] = getattr(_this, _k)

def strippingArchive():
    return dict(_strippings)
