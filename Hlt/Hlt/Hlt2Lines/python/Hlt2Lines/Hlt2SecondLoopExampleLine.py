from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2SecondLoopExampleLinesConf(HltLinesConfigurableUser) :
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
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedSecondLoopPions        

        #The first stage just uses the regulard kalman fitted pions
        Hlt2SecondLoopExample_Stage1 = Hlt2Member( CombineParticles 
                                        , "Combine_Stage1"     
                                        , DecayDescriptor = "D0 -> pi+ pi-"
                                        , CombinationCut = "((AM< %(BMassWinHigh)s *MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(doca)s ))" % self.getProps()
                                        , DaughtersCuts = { "pi+" : "(PT> %(PionPTmin)s *MeV) & (MIPDV(PRIMARY)> %(PionIPmin)s )" % self.getProps() }
                                        , MotherCut = "(BPVVDSIGN> %(BSignDistFlight)s ) & (INTREE ( (ABSID=='pi+') & (PT> %(PionPTmax)s *MeV))) & ( INTREE(( ABSID=='pi+') & (MIPDV(PRIMARY)> %(PionIPmax)s )))" % self.getProps()
                                        , InputLocations = [ BiKalmanFittedPions ])

        #In the second stage, use the particle made in stage 1 with the lower momentum pions
        #reconstructed during the second loop
        Hlt2SecondLoopExample_Stage2 = Hlt2Member( CombineParticles 
                                        , "Combine_Stage2"    
                                        , DecayDescriptor = "B+ -> D0 pi+"
                                        , CombinationCut = "(AM> %(BMassWinLow)s *MeV)" % self.getProps()
                                        , DaughtersCuts = { "pi+" : "(PT> %(PionPTmin)s *MeV) & (MIPDV(PRIMARY)> %(PionIPmin)s )" % self.getProps() }
                                        , MotherCut = "(BPVIP()< %(BIP)s )" % self.getProps()
                                        , InputLocations = [ BiKalmanFittedSecondLoopPions,Hlt2SecondLoopExample_Stage1 ]) 

        ###########################################################################
        # Define the Hlt2 Line
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        from HltTracking.HltPVs import PV3D
        line = Hlt2Line('SecondLoopExample'
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, Hlt2SecondLoopExample_Stage1, BiKalmanFittedSecondLoopPions, Hlt2SecondLoopExample_Stage2 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SecondLoopExampleDecision" : 50082 } )
