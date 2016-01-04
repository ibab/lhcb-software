"""
Module importing stripping selection line builder modules
for B2CC WG.
"""

# last update: 05th Dec 2015
_selections = ['StrippingBd2JpsieeKS',
               'StrippingB2JpsiPi0_Run1']#,
               #'StrippingB2JpsiPi0',
               #'StrippingB2JpsiXforBeta_s',
               #'StrippingBs2EtacPhiBDT',
               #'StrippingBs2JpsieePhi',
               #'StrippingB2JpsiHH']

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

