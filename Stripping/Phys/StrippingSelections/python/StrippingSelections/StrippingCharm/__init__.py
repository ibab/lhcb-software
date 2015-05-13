"""
Module importing stripping selection line builder modules
for Charm WG.
"""

_selections = (
    'StrippingD02HHForXSec',
    'StrippingD02K3PiForXSec',
    'StrippingD2HHHForXSec',
    'StrippingD2PhiPiForXSec',
    'StrippingLambdac2PHHForXSec',
    'StrippingXic2PKKPiForXSec',
    'StrippingXic2PKPiForXSec',
    "StrippingB2ppipiSigmacmm_Lcpi" ,
    'StrippingPromptCharm',
    'StrippingD2hh.py',
    'StrippingDstarD2hhpi0.py',
    'StrippingDstarD2KShh.py'
    )

for _sel in _selections :
    try :
        __import__( '%s.%s'  % ( __name__, _sel ) )
    except Exception, x:
        print '[WARNING] Submodule %s.%s raises the excetpion "%s" and will be skipped !' % ( __name__,_sel,x )

from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

