from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2CharmHadMinBiasLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'LCMassWinLow'         : 2150   # MeV
                   ,'LCMassWinHigh'       : 2430   # MeV
                   ,'LCChildTrChi2'              : 4      # 
                   ,'LCChildPT'             : 300    # MeV
                   ,'LCChildIPChi2'         : 4      #
                   ,'LCDIRA'              : 0.9998 # 
                   ,'LCPT'                : 1000   # MeV
                   ,'LCFDCHI2'            : 16     # 
                   ,'LCVCHI2'             : 20    
                   #
                   , 'D0MassWinLow'       : 1715
                   , 'D0MassWinHigh'      : 2015
                   , 'D0ChildTrChi2'      : 3
                   , 'D0ChildPT'          : 800
                   , 'D0PT'               : 2000
                   , 'D0DIRA'             : 0.9995
                   , 'D0TAU'              : 0.15 # ps 
                   , 'D0VCHI2'            : 15             
                   # 
                   , 'Prescale'         : { } 
                   , 'Postscale'        : { }
                   }
    


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedSecondLoopPions,BiKalmanFittedSecondLoopKaons,BiKalmanFittedSecondLoopProtons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedKaons

        # First the lambda C
        _daughters_cut_lc = "( TRCHI2DOF < %(LCChildTrChi2)s ) & ( PT > %(LCChildPT)s *MeV ) & ( MIPCHI2DV(PRIMARY) > %(LCChildIPChi2)s )" % self.getProps()

        _combination_cut_lc = "(AM>%(LCMassWinLow)s*MeV) & (AM<%(LCMassWinHigh)s*MeV)" % self.getProps()

        _mother_cut_lc = "( PT > %(LCPT)s*MeV ) & (BPVDIRA > %(LCDIRA)s ) & ( BPVVDCHI2 > %(LCFDCHI2)s ) & (VFASPF(VCHI2PDOF) < %(LCVCHI2)s)" % self.getProps()


        Hlt2CharmHadLambdaC2KPPi = Hlt2Member( CombineParticles 
                               , "Combine"     
                               , DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc"
                               , DaughtersCuts = { 'pi+' : _daughters_cut_lc, 'K+' : _daughters_cut_lc, 'p+' : _daughters_cut_lc }
                               , CombinationCut = _combination_cut_lc
                               , MotherCut = _mother_cut_lc
                               , Inputs = [ BiKalmanFittedSecondLoopPions.outputSelection(), 
                                            BiKalmanFittedSecondLoopKaons.outputSelection(),
                                            BiKalmanFittedSecondLoopProtons.outputSelection() ])

        # First the lambda C
        _daughters_cut_d0 = "( TRCHI2DOF < %(D0ChildTrChi2)s ) & ( PT > %(D0ChildPT)s *MeV )" % self.getProps()

        _combination_cut_d0 = "(AM>%(D0MassWinLow)s*MeV) & (AM<%(D0MassWinHigh)s*MeV)" % self.getProps()

        _mother_cut_d0 = "( PT > %(D0PT)s ) & (BPVDIRA > %(D0DIRA)s ) & (VFASPF(VCHI2PDOF) < %(D0VCHI2)s) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(D0TAU)s*ps)" % self.getProps()


        # Now the D0->hh
        Hlt2CharmHadD02KPi = Hlt2Member( CombineParticles
                               , "Combine"     
                               , DecayDescriptor = "[D0 -> K- pi+]cc"
                               , DaughtersCuts = { 'pi+' : _daughters_cut_d0, 'K+' : _daughters_cut_d0 }
                               , CombinationCut = _combination_cut_d0
                               , MotherCut = _mother_cut_d0
                               , Inputs = [ BiKalmanFittedPions.outputSelection(),
                                            BiKalmanFittedKaons.outputSelection()])

        Hlt2CharmHadD02KK = Hlt2Member( CombineParticles
                               , "Combine"         
                               , DecayDescriptor = "[D0 -> K- K+]cc"
                               , DaughtersCuts = { 'K+' : _daughters_cut_d0 }
                               , CombinationCut = _combination_cut_d0
                               , MotherCut = _mother_cut_d0
                               , Inputs = [ BiKalmanFittedKaons.outputSelection()])
        ###########################################################################
        # Define the Hlt2 Lines
        #
        from HltTracking.HltPVs import PV3D
        line = Hlt2Line('CharmHadMinBiasLambdaC2KPPi'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')" 
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedSecondLoopPions, 
                                            BiKalmanFittedSecondLoopKaons, 
                                            BiKalmanFittedSecondLoopProtons, 
                                            Hlt2CharmHadLambdaC2KPPi]
                        , postscale = self.postscale
                        )

        line = Hlt2Line('CharmHadMinBiasD02KPi'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')" 
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, 
                                            BiKalmanFittedKaons, 
                                            Hlt2CharmHadD02KPi]
                        , postscale = self.postscale
                        )

        line = Hlt2Line('CharmHadMinBiasD02KK'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')"
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedKaons,           
                                            Hlt2CharmHadD02KK]
                        , postscale = self.postscale
                        )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasD02KKDecision" :55335  } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasD02KPiDecision" :55334  } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasLambdaC2KPPiDecision" :55333  } )
