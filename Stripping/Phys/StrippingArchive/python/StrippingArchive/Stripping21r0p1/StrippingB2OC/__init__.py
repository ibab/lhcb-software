"""
Module importing stripping selection line builder modules
for B2OC WG.
"""

_selections = [ "StrippingBeauty2Charm", "StrippingDstarD02Kpipi0" , "StrippingB2DDphi", "StrippingB2nbody", 'StrippingB2DXD2HMuNu' ]

for _sel in _selections :
    try :
        __import__( '%s.%s'  % ( __name__, _sel ) )
    except Exception, x:
        print '[WARNING] Submodule %s.%s raises the exception "%s" and will be skipped !' % ( __name__,_sel,x )

from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

