"""
Stripping selection for inclusive b-jets

"""
__author__  = ['Marcin Kucharczyk']
__date__    = '30/01/2012'
__version__ = '$Revision: 1.0$'

from os import environ, path
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from CommonParticles.Utils import *
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StandardParticles import StdAllNoPIDsPions
from StandardParticles import StdLooseAllPhotons

__all__ = ('InclbJetsLinesConf',
           'confdict')

confdict = {
       'NrPVsCut'         : 1
    ,  'NrSeedsCut'       : 2
    ,  'TrkChi2Cut'       : 3.0
    ,  'PrtMomCut'        : 2.0
    ,  'PrtPtCut'         : 0.6
    ,  'PrtIPSCut'        : 2.5
    ,  'DZSVPVCut'        : 1.0
    ,  'SumMomSVCut'      : 3.0
    ,  'VtxChi2Cut'       : 20.0
    ,  'scale'            : 0.05
 }

class InclbJetsLinesConf(LineBuilder) :
    __configuration_keys__ = ('NrPVsCut',
                              'NrSeedsCut',
                              'TrkChi2Cut',
                              'PrtMomCut',
                              'PrtPtCut',
                              'PrtIPSCut',
                              'DZSVPVCut',
                              'SumMomSVCut',
                              'VtxChi2Cut',
                              'scale'
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__= config
        self.name = name
        self.inclbJetLine = self._makeInclbJetLine(name)
        self.registerLine(self.inclbJetLine)

    def _makeInclbJetSel(self, name) :
        from Configurables import bJetSeeds
        selectionAlg = bJetSeeds('Sel'+name+'Alg')
        selectionAlg.NrPVs = self.__confdict__['NrPVsCut']
        selectionAlg.NrSeeds = self.__confdict__['NrSeedsCut']
        selectionAlg.TrkChi2 = self.__confdict__['TrkChi2Cut']
        selectionAlg.PrtMom = self.__confdict__['PrtMomCut']
        selectionAlg.PrtPt = self.__confdict__['PrtPtCut']
        selectionAlg.PrtIPS = self.__confdict__['PrtIPSCut']
        selectionAlg.DZSVPV = self.__confdict__['DZSVPVCut']
        selectionAlg.SumMomSV = self.__confdict__['SumMomSVCut']
        selectionAlg.VtxChi2 = self.__confdict__['VtxChi2Cut']
        return Selection("Sel" + name, Algorithm = selectionAlg,
                         RequiredSelections = [StdAllNoPIDsPions, StdLooseAllPhotons])

    def _makeInclbJetLine(self, name):
        return StrippingLine(name ,
                             HLT = "HLT_PASS_RE('Hlt2*.Topo.*Decision')",
                             prescale = self.__confdict__['scale'],
                             selection = self._makeInclbJetSel(name))
