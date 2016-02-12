"""
Module with stripping selection line builder modules.
All line builders available via function lineBuilders().
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import StrippingHeavyIonDst2D0KPiForPID
import StrippingHeavyIonOpenCharm
import StrippingHeavyIonTrackEffDownMuon
import StrippingHeavyIonTrackEffVeloMuon
import StrippingHeavyIonDiMuon
import StrippingHeavyIonMiniBias
import StrippingHeavyIonPIDCalib
import StrippingHeavyIonTrackEffMuonTT



from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

from StrippingUtils.Utils import getLineBuildersFromModule as _getter
from StrippingUtils.Utils import getBuilderConfFromModule as _getcon

_lineBuilders = {}
_buildersConf = {}

for _sm in _strippingModules :
    _lineBuilders.update(_getter(_sm))
    _buildersConf.update(_getcon(_sm))

def lineBuilders() :
    """
    Return all the line builders in the module.
    """
    return dict(_lineBuilders)
def buildersConf() :
    """
    Return all the line builders in the module.
    """
    return dict(_buildersConf)





