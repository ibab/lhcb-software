from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHLTUnbiasedLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'BMassWinLow'         :  5000      # MeV
                   ,'BMassWinHigh'       :  5900      # MeV
                   ,'doca'               :     0.07
                   ,'KaonPTmin'          :  1800       # MeV
                   ,'KaonPTmax'          :  2500       # MeV
                   ,'KaonPmin'           : 10000       # MeV
                   ,'BPmin'              : 10000
                   ,'PIDK_min'           :     0.1
                   ,'PIDK_max'           :     0.1
                   ,'PIDMu_min'          :     2.0
                   ,'TrackChi2'          :    5.
                   ,'VertexChi2'         :    10.0
                   }
    


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichKaons            
        
        Hlt2B2HHLTUnbiased = Hlt2Member( CombineParticles 
                                         , "Combine"     
                                         , DecayDescriptor = "B_s0 -> K+ K-"
                                         , DaughtersCuts = { "K+" : "(ISLONG & (TRCHI2DOF<%(TrackChi2)s)& (P> %(KaonPmin)s *MeV) &(PT> %(KaonPTmin)s *MeV)&(PIDK > %(PIDK_min)s) & (PIDmu < %(PIDMu_min)s))" % self.getProps() }
                                         , CombinationCut = "((AM> %(BMassWinLow)s *MeV) & (AM< %(BMassWinHigh)s *MeV) & (AMAXDOCA('LoKi::DistanceCalculator')< %(doca)s ))" % self.getProps()
                                         , MotherCut = "(P>%(BPmin)s * MeV) & (MAXTREE(('K+'==ABSID) ,PT) > %(KaonPTmax)s*MeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(PIDK_max)s ) & (VFASPF(VCHI2PDOF) < %(VertexChi2)s)"  % self.getProps()
                                         , InputLocations = [BiKalmanFittedRichKaons]
                                         )
        ###########################################################################
        # Define the Hlt2 Line
        #
        line = Hlt2Line('B2HHLTUnbiased'
                        , prescale = self.prescale
                        , HLT = "HLT_PASS_RE('Hlt1DiHadronLTUnbiasedDecision')" 
                        , algos = [ BiKalmanFittedRichKaons, Hlt2B2HHLTUnbiased ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHLTUnbiasedDecision" : 50081 } )
