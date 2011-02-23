"""
Module with stripping selection line builder modules.
All line builders available via function lineBuilders().
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import StrippingB2DX
import StrippingB2XGamma
import StrippingBs2JpsiPhiPrescaledAndDetatched
import StrippingB2hhLTUnbiased
import StrippingNeuroBayesMuMu
import StrippingB2nbody
import StrippingHb2Charged2Body
import StrippingBu2LLK
import StrippingBd2DstarMuNu
import StrippingB0q2DplusMuX
import StrippingBd2DstarTauNu
import StrippingB2XuMuNu
import StrippingB2DMuNuX
import StrippingBu2D0h_D02KShh_NoPID
import StrippingBu2D0h_D02KShh_NoPID_WS
import StrippingBs2PhiPhi
import StrippingInclPhi

from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

from StrippingUtils.Utils import getLineBuildersFromModule as _getter

_lineBuilders = {}

for _sm in _strippingModules :
    _lineBuilders.update(_getter(_sm))

def lineBuilders() :
    """
    Return all the line builders in the module.
    """
    return dict(_lineBuilders)
