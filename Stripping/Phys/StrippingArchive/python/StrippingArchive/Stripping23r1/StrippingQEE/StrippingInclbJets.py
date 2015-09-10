
"""
Stripping selection for inclusive b-jets

"""
__author__  = ['Marcin Kucharczyk']
__date__    = '30/01/2012'
__version__ = '$Revision: 1.0$'

from PhysSelPython.Wrappers import Selection
from StandardParticles import StdAllNoPIDsPions
from StandardParticles import StdLooseAllPhotons
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


__all__ = (
  'InclbJetsLinesConf',
  'default_config',
)


default_config = {
    'NAME'        : 'InclbJets',
    'BUILDERTYPE' : 'InclbJetsConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : {
        'NrPVsCut'      : 1,
        'NrSeedsCut'    : 2,
        'TrkChi2Cut'    : 3.0,
        'PrtMomCut'     : 2.0,
        'PrtPtCut'      : 0.6,
        'PrtIPSCut'     : 2.5,
        'DZSVPVCut'     : 1.0,
        'SumMomSVCut'   : 1.0,
        'VtxChi2Cut'    : 20.0,
        'scale'         : 0.05,
        #
        'HLT2'          : "HLT_PASS_RE('Hlt2*.Topo.*Decision')",
        'VertexFitter'  : 'LoKi::VertexFitter:PUBLIC',
    },
}

class InclbJetsConf(LineBuilder) :

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__ = config
        self.inclbJetLine = self._makeInclbJetLine(name)
        self.registerLine(self.inclbJetLine)

    def _makeInclbJetSel(self, name) :
        from Configurables import bJetSeeds
        selectionAlg = bJetSeeds('Sel'+name+'Alg')
        selectionAlg.NrPVs    = self.__confdict__['NrPVsCut']
        selectionAlg.NrSeeds  = self.__confdict__['NrSeedsCut']
        selectionAlg.TrkChi2  = self.__confdict__['TrkChi2Cut']
        selectionAlg.PrtMom   = self.__confdict__['PrtMomCut']
        selectionAlg.PrtPt    = self.__confdict__['PrtPtCut']
        selectionAlg.PrtIPS   = self.__confdict__['PrtIPSCut']
        selectionAlg.DZSVPV   = self.__confdict__['DZSVPVCut']
        selectionAlg.SumMomSV = self.__confdict__['SumMomSVCut']
        selectionAlg.VtxChi2  = self.__confdict__['VtxChi2Cut']

        selectionAlg.VertexFitter = self.__confdict__['VertexFitter']

        return Selection("Sel" + name, Algorithm = selectionAlg,
                         RequiredSelections = [StdAllNoPIDsPions, StdLooseAllPhotons])

    def _makeInclbJetLine(self, name):
        return StrippingLine(name ,
                             HLT2 = self.__confdict__['HLT2'],
                             prescale = self.__confdict__['scale'],
                             RequiredRawEvents = ["Calo"],
                             selection = self._makeInclbJetSel(name))
