from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2HHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'BMassWinLow'         : 4800      # MeV
                   ,'BMassWinHigh'       : 5800      # MeV
                   ,'doca'               : 0.1       # MeV
                   ,'PionPTmin'          : 700       # MeV
                   ,'PionPTmax'          : 2400      # MeV
                   ,'PionIPmin'          : 0.08      # mm
                   ,'PionIPmax'          : 0.24      # mm
                   ,'BIP'                : 0.1       # mm
                   ,'BSignDistFlight'    : 2.2       # mm
                   }
    


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions        
        

        Hlt2B2HH = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "B0 -> pi+ pi-"
                               , CombinationCut = "(in_range(%(BMassWinLow)s *MeV, AM, %(BMassWinHigh)s *MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(doca)s ))" % self.getProps()
                               , DaughtersCuts = { "pi+" : "(PT> %(PionPTmin)s *MeV) & (MIPDV(PRIMARY)> %(PionIPmin)s )" % self.getProps() }
                               , MotherCut = "(BPVIP()< %(BIP)s ) & (BPVVDSIGN> %(BSignDistFlight)s ) & (INTREE ( (ABSID=='pi+') & (PT> %(PionPTmax)s *MeV))) & ( INTREE(( ABSID=='pi+') & (MIPDV(PRIMARY)> %(PionIPmax)s )))" % self.getProps()
                               , InputLocations = [ BiKalmanFittedPions ])
        ###########################################################################
        # Define the Hlt2 Line
        #
        from HltTracking.HltPVs import PV3D
        line = Hlt2Line('B2HH'
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, Hlt2B2HH ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHDecision" : 50080 } )
