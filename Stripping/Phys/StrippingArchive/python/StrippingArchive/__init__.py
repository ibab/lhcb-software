"""
Module with all former stripping selection line builder modules.
All line builders available via function lineBuilders(stripping) in StrippingArchive.Utils.
"""

__author__ = 'Rob Lambert'
__all__ = ['strippingArchive', 'strippingDescription']

# List of known strippings
_known_strippings = [
  "Stripping13", 
  "Stripping14", 
  "Stripping15", 
  "Stripping16", 
  "Stripping17", 
  "Stripping17b", 
  "PreStripping18",
  "Stripping18", 
  "Stripping19",
  "Stripping19b",
  "Stripping19c", 
  "Stripping20", 
  "Stripping20r1", 
  "Stripping20r0p1"
]

#give a dictionary of strippings which use the same line builders
_duplicate_strippings={ "Stripping13b"        : "Stripping13",
                        "Stripping13Calib"    : "Stripping13",
                        "Stripping1Point4Tev" : "Stripping13",
                        "Stripping18b"        : "Stripping18",
                        "Stripping19a"        : "Stripping19", 
                        "Stripping20r1p1"     : "Stripping20r0p1" }


#give a dictionary to describe what each stripping was for
_stripping_help={"Stripping13"  : "2011 data taking, processing during the first half of 2011",
                 "Stripping13b" : "2011 data reprocessing, of the data in the first half of 2011",
                 "Stripping13Calib" : "2011 special calibration-only stripping",
                 "Stripping1Point4Tev" : "2011 data reprocessing, for data in 2011 taken at 1.4 TeV",
                 "Stripping14" : "2010 data reprocessing, reprocessed in early 2011 with the 2010 dataset",
                 "Stripping15" : "2011 data processing for data in 2011 taken after the June technical stop",
                 "Stripping16" : "2011 data processing for data in 2011 taken after the July technical stop, many CPU improvements",
                 "Stripping17" : "2011 data reprocessing for all data in 2011, reprocessed at the end of 2011, many bandwidth improvements",
                 "Stripping17b": "2011 data restripping for all data in 2011, DiMuon and Semileptonic streams replace Stripping17, the rest is incremental", 
                 "PreStripping18": "Test of stripping selections for 2012 data taking, using a part of 2011 sample",
                 "Stripping18" : "2012 data processing for data taken from start of 2012, including stream dedicated for ICHEP 2012", 
                 "Stripping18b": "2012 data processing for data taken from start of 2012, without stream dedicated for ICHEP 2012", 
                 "Stripping19" : "2012 data processing for data taken from start of 2012, selections tightened to improve timing",
                 "Stripping19a": "2012 data processing for data taken from May 2012, with bug fix", 
                 "Stripping19b": "2012 data processing for data taken from June 2012 TS", 
                 "Stripping19c": "2012 data processing for data taken since August 2012", 
                 "Stripping20" : "2012 data reprocessing, promt processing of 2012 data after September TS", 
                 "Stripping20r1" : "2011 data reprocessing with Reco14", 
                 "Stripping20r0p1" : "First incremental restripping of 2012 data", 
                 "Stripping20r1p1" : "First incremental restripping 20 2011 data"
                 }

_strippingKeys = []

#import known strippings
for x in _known_strippings : 
    module_name = __name__ + "." + x
    print "Trying to import module", module_name
    try : 
        __import__(module_name)
    except Exception, x: 
	print "  -> Cannot be loaded with this version of DaVinci (%s)" % str(x)
    else : 
	_strippingKeys.append(x)


#compile dictionary of all known strippings
from sys import modules as _modules
_this = _modules[__name__]

_strippings={}

for _k in _strippingKeys:
    _strippings[_k] = getattr(_this, _k)

#add the duplicated strippings
for _k in _duplicate_strippings:
    if _k in _strippingKeys:
        raise KeyError, _k+' already defined as a StippingArchive. check _duplicate_strippings'
    if _duplicate_strippings[_k] not in _known_strippings:
        raise KeyError, _duplicate_strippings[_k]+' is not defined as a StippingArchive. check _duplicate_strippings'
    if _duplicate_strippings[_k] in _strippingKeys : 
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
        
    if type(stripping) is not str:
        raise TypeError, "Strippings must be strings, like Stripping15 for example"

    for key in _strippings.keys() : 
	if key.lower() == stripping.lower() : 
	    return _strippings[key]

    raise KeyError, stripping + ' is not known, call strippingArchive() with no arguement to get the full dictionary'
    

def strippingDescription(stripping=None):
    '''Return the description of the stripping pass
    strippingDescription(): return all descriptions in a dictionary {stripping:module}.
    strippingDescription(stripping): return the description for that stripping'''
    if stripping is None:
        return dict(_stripping_help)

    if type(stripping) is not str:
        raise TypeError, "Strippings must be strings, like Stripping15 for example"

    for key in _strippings.keys() : 
	if key.lower() == stripping.lower() : 
	    return _stripping_help[key]

    raise KeyError, stripping + ' is not known, call strippingDescription() with no arguement to get the full dictionary'
