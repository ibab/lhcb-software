"""
Module with all former stripping selection line builder modules.
All line builders available via function lineBuilders(stripping) in StrippingArchive.Utils.
"""

__author__ = 'Rob Lambert'
__all__ = ['strippingArchive', 'strippingDescription']

#import known strippings
import Stripping13
import Stripping14
import Stripping15
import Stripping16
import Stripping17

#give a dictionary of strippings which use the same line builders
_duplicate_strippings={ "Stripping13b"        : "Stripping13",
                        "Stripping1Point4Tev" : "Stripping13" }


#give a dictionary to describe what each stripping was for
_stripping_help={"Stripping13"  : "2011 data taking, processing during the first half of 2011",
                 "Stripping13b" : "2011 data reprocessing, of the data in the first half of 2011",
                 "Stripping1Point4Tev" : "2011 data reprocessing, for data in 2011 taken at 1.4 TeV",
                 "Stripping14" : "2010 data reprocessing, reprocessed in early 2011 with the 2010 dataset",
                 "Stripping15" : "2011 data processing for data in 2011 taken after the June technical stop",
                 "Stripping16" : "2011 data processing for data in 2011 taken after the July technical stop, many CPU improvements",
                 "Stripping16" : "2011 data reprocessing for all data in 2011, reprocessed at the end of 2011, many bandwidth improvements"
                 }

#compile dictionary of all known strippings
from sys import modules as _modules
_this = _modules[__name__]

_strippings={}

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

for _k in _strippingKeys:
    _strippings[_k] = getattr(_this, _k)

#add the duplicated strippings
for _k in _duplicate_strippings:
    if _k in _strippingKeys:
        raise KeyError, _k+' already defined as a StippingArchive. check _duplicate_strippings'
    if _duplicate_strippings[_k] not in _strippingKeys:
        raise KeyError, _duplicate_strippings[_k]+' is not defined as a StippingArchive. check _duplicate_strippings'
    _strippings[_k]=_strippings[_duplicate_strippings[_k]]

#check the descriptions
for _k in _strippings:
    if _k not in _stripping_help:
        raise KeyError, _k+' has not been provided a description. check _stripping_help'

#the only two functions to be exported to the end user
def strippingArchive(stripping=None):
    '''Return the archived stripping line builders.
    strippingArchive(): return all line builder modules in a dictionary {stripping:module}.
    strippingArchive(stripping): return the line builder module for that stripping'''
    if stripping is None:
        return dict(_strippings)
    if stripping not in _strippings:
        if type(stripping) is not str:
            raise TypeError, "Strippings must be strings, like Stripping15 for example"
        #case insensitive for s
        if stripping[0]=='s':
            stripping='S'+stripping[1:]
            if stripping in _strippings:
                return _strippings[stripping]
        elif stripping[0]=='S':
            stripping='s'+stripping[1:]
            if stripping in _strippings:
                return _strippings[stripping]
        raise KeyError, stripping + ' is not known, call strippingArchive() with no arguement to get the full dictionary'
    return _strippings[stripping]

def strippingDescription(stripping=None):
    '''Return the description of the stripping pass
    strippingDescription(): return all descriptions in a dictionary {stripping:module}.
    strippingDescription(stripping): return the description for that stripping'''
    if stripping is None:
        return dict(_stripping_help)
    if stripping not in _strippings:
        if type(stripping) is not str:
            raise TypeError, "Strippings must be strings, like Stripping15 for example"
        #case insensitive for s
        if stripping[0]=='s':
            stripping='S'+stripping[1:]
            if stripping in _strippings:
                return _stripping_help[stripping]
        elif stripping[0]=='S':
            stripping='s'+stripping[1:]
            if stripping in _strippings:
                return _stripping_help[stripping]
        raise KeyError, stripping + ' is not known, call strippingDescription() with no arguement to get the full dictionary'
    return _stripping_help[stripping]
