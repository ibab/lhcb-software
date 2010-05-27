from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHLTUnbiasedLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'BMassWinLow'         : 5000      # MeV
                   ,'BMassWinHigh'       : 5900      # MeV
                   ,'doca'               : 0.1 
                   ,'KaonPTmin'          : 1000       # MeV
                   ,'KaonPTmax'          : 1100      # MeV
                   ,'KaonPmin'           : 10000
                   ,'PIDK_min'           : 0
                   ,'PIDK_max'           : 5  
                   ,'TrackChi2'          : 10  
                   }
    


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichKaons            
        
        Hlt2B2HHLTUnbiased = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "B0 -> K+ K-"
                               , CombinationCut = "((AM> %(BMassWinLow)s *MeV) & (AM< %(BMassWinHigh)s *MeV) & (AMAXDOCA('LoKi::DistanceCalculator')< %(doca)s ))" % self.getProps()
                               , DaughtersCuts = { "K+" : "((P>%(KaonPmin)s)&(TRCHI2DOF<%(TrackChi2)s)&(PT> %(KaonPTmin)s *MeV)&(PIDK > %(PIDK_min)s))" % self.getProps() }
                               , MotherCut = "(INTREE ( (ABSID=='K+') & (PT> %(KaonPTmax)s *MeV) & (PIDK > %(PIDK_max)s) ))" % self.getProps()
                               , InputLocations = [BiKalmanFittedRichKaons])
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
