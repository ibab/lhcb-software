from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
from HltLine.HltLine import Hlt2Member


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
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.GoodParticles import GoodPions            
        
        

        Hlt2B2HH = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "B0 -> pi+ pi-"
                               , CombinationCut = "((AM> %(BMassWinLow)s *MeV) & (AM< %(BMassWinHigh)s *MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(doca)s ))" % self.getProps()
                               , DaughtersCuts = { "pi+" : "(PT> %(PionPTmin)s *MeV) & (MIPDV(PRIMARY)> %(PionIPmin)s )" % self.getProps() }
                               # , DaughtersCuts = { "pi+" : "(PT> 700 *MeV) & (MIPDV(PRIMARY)>0.08 )" }
                               , MotherCut = "(BPVIP()< %(BIP)s ) & (BPVVDSIGN> %(BSignDistFlight)s ) & (INTREE ( (ABSID=='pi+') & (PT> %(PionPTmax)s *MeV))) & ( INTREE(( ABSID=='pi+') & (MIPDV(PRIMARY)> %(PionIPmax)s )))" % self.getProps()
                               , InputLocations = [GoodPions])
###########################################################################
        # Define the Hlt2 Line
        #
        line = Hlt2Line('B2HH'
                        , prescale = 1
                        ,  algos = [ GoodPions, Hlt2B2HH ]
                        ,postscale = 1
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHDecision" : 50080 } )
