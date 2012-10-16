"""
Module for high-pt jets selection
"""

__author__  = ['Marcin Kucharczyk']
__date__    = '17/09/2011'
__version__ = '$Revision: 1.0$'
__all__ = ('Hlt2HighPtJetsLinesConf')

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
import GaudiKernel.SystemOfUnits as units

class Hlt2HighPtJetsLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {            
           'MinWeightSeedCut' : 6
        ,  'ConeExtFactorCut' : 4.0
        ,  'MaxDeltaPhiCut'   : 15.0
        ,  'MaxDeltaEtaCut'   : 0.35
        ,  'Min1stJetPtCut'   : 10.0
        ,  'Min2ndJetPtCut'   : 0.0
        ,  'JetMultCut'       : 1
        ,  'NrPVsCut'         : 1
        ,  'MinSumEtCut'      : 0.0
        ,  'PostScale'        : 1.0
    }
                
    def __apply_configuration__(self) :

        from HltLine.HltLine import Hlt2Line
        from Configurables import GaudiSequencer, HltANNSvc
        from HltTracking.HltPVs import PV3D
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking

        #######################################################################
        DVSeq = []
        hlt2Tracking = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        DVSeq.extend([PV3D(),hlt2Tracking])
        
        ####################################################################### 
        from Configurables import TrackClusterFinder
        Hlt2TrkFinder = TrackClusterFinder("Hlt2TrkFinder")
        DVSeq.append(Hlt2TrkFinder)
        Hlt2TrkFinder.InputTracks = [hlt2Tracking.outputSelection()]
        Hlt2TrkFinder.InputPVs = "Hlt/Vertex/PV3D"
        Hlt2TrkFinder.MinWeightSeed = self.getProp('MinWeightSeedCut')
        Hlt2TrkFinder.ConeExtFactor = self.getProp('ConeExtFactorCut')
        Hlt2TrkFinder.MaxDeltaPhi = self.getProp('MaxDeltaPhiCut')
        Hlt2TrkFinder.MaxDeltaEta = self.getProp('MaxDeltaEtaCut')
        Hlt2TrkFinder.Min1stJetPt = self.getProp('Min1stJetPtCut')
        Hlt2TrkFinder.Min2ndJetPt = self.getProp('Min2ndJetPtCut')
        Hlt2TrkFinder.JetMult = self.getProp('JetMultCut')
        Hlt2TrkFinder.NrPVs = self.getProp('NrPVsCut')
        Hlt2TrkFinder.MinSumEt = self.getProp('MinSumEtCut')

        Hlt1HighPtJets = "HLT_PASS_RE('Hlt1HighPtJetsSinglePV.*Decision')"

        line = Hlt2Line('HighPtJets'
                        , prescale = self.prescale
                        , HLT = Hlt1HighPtJets
                        , algos = DVSeq + [Hlt2TrkFinder]
                        , postscale = self.getProp('PostScale')
                        )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2HighPtJetsDecision" : 50295 } )
        
