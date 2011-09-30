"""
Stripping selection for high-pt jets

"""
__author__  = ['Marcin Kucharczyk']
__date__    = '07/03/2011'
__version__ = '$Revision: 2.0$'

from os import environ, path
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from CommonParticles.Utils import *
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection

__all__ = ('HighPtJetsLinesConf',
           'confdict')

confdict = {
       'MinWeightSeedCut' : 6
    ,  'ConeExtFactorCut' : 4.0
    ,  'MaxDeltaPhiCut'   : 5.4
    ,  'MaxDeltaEtaCut'   : 0.15
    ,  'Min1stJetPtCut'   : 10.0
    ,  'Min2ndJetPtCut'   : 7.0
    ,  'JetMultCut'       : 2
    ,  'MaxRPVCut'        : 0.8
    ,  'scale'            : 0.2
 }

class HighPtJetsLinesConf(LineBuilder) :
    __configuration_keys__ = ('MinWeightSeedCut',
                              'ConeExtFactorCut',
                              'MaxDeltaPhiCut',
                              'MaxDeltaEtaCut',
                              'Min1stJetPtCut',
                              'Min2ndJetPtCut',
                              'JetMultCut',
                              'MaxRPVCut',
                              'scale'
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__= config
        self.name = name
        self.highPtJetLine = self._makeHighPtJetLine(name)
        self.registerLine(self.highPtJetLine)

    def _makeHighPtJetSel(self, name) :
        from Configurables import TrackClusterFinder
        selectionAlg = TrackClusterFinder('Sel'+name+'Alg')
        selectionAlg.MinWeightSeed = self.__confdict__['MinWeightSeedCut']
        selectionAlg.ConeExtFactor = self.__confdict__['ConeExtFactorCut']
        selectionAlg.MaxDeltaPhi = self.__confdict__['MaxDeltaPhiCut']
        selectionAlg.MaxDeltaEta = self.__confdict__['MaxDeltaEtaCut']
        selectionAlg.Min1stJetPt = self.__confdict__['Min1stJetPtCut']
        selectionAlg.Min2ndJetPt = self.__confdict__['Min2ndJetPtCut']
        selectionAlg.JetMult = self.__confdict__['JetMultCut']
        selectionAlg.MaxRPV = self.__confdict__['MaxRPVCut']
        return Selection("Sel" + name, Algorithm = selectionAlg,
                         RequiredSelections = [])

    def _makeHighPtJetLine(self, name):
        return StrippingLine(name ,
                             prescale = self.__confdict__['scale'],
                             selection = self._makeHighPtJetSel(name))
