"""
Module importing stripping selection line builder modules
for RD WG.
"""

_selections = [
    'StrippingB2Lambda0MuLines',
    'StrippingB2XMuMu',
    # 'StrippingB2XMuMuInclusive',
    'StrippingB2KstTauTau',
    'StrippingBu2LLK',
    'StrippingRareStrange',
    'StrippingRareNStrange',
    'StrippingBeauty2XGamma',
    'StrippingBeauty2XGammaExclusive',
    'StrippingBeauty2XGammaNoBias',
    'StrippingLb2L0Gamma',
    'StrippingBs2gammagamma',
    'StrippingBd2eeKstarBDT',
    'StrippingDarkBoson',
    'StrippingB2XTau',
    'StrippingZVTOP',
    'StrippingBs2MuMuLines',
    'StrippingBu2MuNu',
    'StrippingD23MuLines',
    'StrippingB23MuLines',
    'StrippingB24pLines',
    'StrippingBLVLines',
    'StrippingKshort2MuMuMuMu',
    'StrippingKshort2PiPiMuMu',
    'StrippingKshort2eePiPi',
    'StrippingLc23MuLines',
    'StrippingLFVLines',
    'StrippingRareStrange',
    'StrippingRareNStrange',
    'StrippingTau2LambdaMuLines',
    'StrippingTau23MuLines',
    'StrippingBs2st2KKMuX',
    'StrippingPhiToKSKS',
    'StrippingB2XLL',
    ]

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
