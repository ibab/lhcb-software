"""
Module importing stripping selection line builder modules
for BnoC WG.
"""

_selections = [ 'StrippingHb2Charged2Body', 
                'StrippingB2CharmlessQuasi2Body', 
                'StrippingB2HHBDT',
                'StrippingD2HHBDT', 
                'StrippingBc2hhh_BnoC',
                'StrippingBu2hhh',
                'StrippingB2pphh',
                'StrippingBs2Kst_0Kst_0',
                'StrippingB2HHPi0',
                'StrippingB2KShh',
                'StrippingLb2V0hh',
                'StrippingBs2PhiPhi',
                'StrippingB2Kpi0',
				'StrippingXb23ph']

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

