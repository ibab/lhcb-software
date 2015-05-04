"""
Module with stripping selection line builder modules.
All line builders available via function lineBuilders().
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'


## from BandQ
import StrippingDiMuonForXsection
import StrippingB2JpsiXforEM_s

## from Calib
import StrippingTrackEffVeloMuon
import StrippingTrackEffMuonTT
import StrippingTrackEffDownMuon

## from Charm
import StrippingD2PhiPiForXSec
import StrippingD2HHHForXSec
import StrippingD02K3PiForXSec
import StrippingD02HHForXSec
import StrippingLambdac2PHHForXSec
import StrippingXic2PKPiForXSec
import StrippingXic2PKKPiForXSec

## from QEE
import StrippingZ02MuMu
import StrippingWMu
import StrippingSingleTrackTIS
import StrippingMuMuSS
import StrippingLowMultINC
import StrippingInclbJets

## from Semileptonic 
import StrippingB2DMuNuX
import StrippingB2DMuNuXUtils


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
