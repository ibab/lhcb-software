from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2CharmHadMinBiasLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'LCMassWinLow'         : 2150   # MeV
                   ,'LCMassWinHigh'       : 2430   # MeV
                   ,'LCChildTrChi2'       : 4      # 
                   ,'LCChildPT'           : 300    # MeV
                   ,'LCChildIPChi2'       : 4      #
                   ,'LCDIRA'              : 0.9999 # 
                   ,'LCPT'                : 2500   # MeV
                   ,'LCFDCHI2'            : 16     # 
                   ,'LCVCHI2'             : 20    
                   #
                   , 'D0MassWinLow'       : 1715
                   , 'D0MassWinHigh'      : 2015
                   , 'D0ChildTrChi2'      : 3
                   , 'D0ChildPT'          : 800
                   , 'D0PT'               : 2000
                   , 'D0DIRA'             : 0.9998
                   , 'D0TAU'              : 0.15 # ps 
                   , 'D0VCHI2'            : 15             
                   #
                   , 'DplusMassWinLow'    : 1765
                   , 'DplusMassWinHigh'   : 2065
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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedKaons,BiKalmanFittedProtons

        from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted

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
                               , Inputs = [ BiKalmanFittedPions.outputSelection(),
                                            BiKalmanFittedKaons.outputSelection(),
                                            BiKalmanFittedProtons.outputSelection()])

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

        _combination_cut_dplus = "(AM>%(DplusMassWinLow)s*MeV) & (AM<%(DplusMassWinHigh)s*MeV)" % self.getProps()

        # Now the D+->hhh
        Hlt2CharmHadDplus2hhh = Hlt2Member( CombineParticles
                               , "Combine"         
                               , DecayDescriptors = [   "[D+ -> K- K+ pi+]cc",
                                                        "[D+ -> K- pi+ pi+]cc",
                                                        "[D+ -> K+ pi+ pi-]cc",
                                                        "[D+ -> pi- pi+ pi+]cc"]
                               , DaughtersCuts = { 'pi+' : _daughters_cut_lc, 'K+' : _daughters_cut_lc }
                               , CombinationCut = _combination_cut_dplus
                               , MotherCut = _mother_cut_lc
                               , Inputs = [ BiKalmanFittedPions.outputSelection(),
                                            BiKalmanFittedKaons.outputSelection()])

        # Now Lc -> Lambda Pi
        Hlt2CharmHadLambdaC2LambdaPi = Hlt2Member( CombineParticles
                               , "Combine"
                               , DecayDescriptor = "[Lambda_c+ -> Lambda0 pi+]cc"
                               , DaughtersCuts = { 'Lambda0' : "(ALL)", 'pi+' : _daughters_cut_lc }
                               , CombinationCut = _combination_cut_lc
                               , MotherCut = _mother_cut_lc
                               , Inputs = [ BiKalmanFittedPions.outputSelection(),
                                            LambdaLLTrackFitted.outputSelection()])

        ###########################################################################
        # Define the Hlt2 Lines
        #
        from HltTracking.HltPVs import PV3D
        line = Hlt2Line('CharmHadMinBiasLambdaC2KPPi'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')" 
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions,  
                                            BiKalmanFittedKaons,  
                                            BiKalmanFittedProtons,  
                                            Hlt2CharmHadLambdaC2KPPi]
                        , postscale = self.postscale
                        )

        line2 = Hlt2Line('CharmHadMinBiasD02KPi'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')" 
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, 
                                            BiKalmanFittedKaons, 
                                            Hlt2CharmHadD02KPi]
                        , postscale = self.postscale
                        )

        line3 = Hlt2Line('CharmHadMinBiasD02KK'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')"
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedKaons,           
                                            Hlt2CharmHadD02KK]
                        , postscale = self.postscale
                        )

        line4 = Hlt2Line('CharmHadMinBiasDplus2hhh'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')"
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, 
                                            BiKalmanFittedKaons,
                                            Hlt2CharmHadDplus2hhh]
                        , postscale = self.postscale
                        ) 

        line5 = Hlt2Line('CharmHadMinBiasLambdaC2LambdaPi'
                        , HLT = "HLT_PASS_RE('Hlt1CharmCalibrationNoBiasDecision')"
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions,
                                            LambdaLLTrackFitted, 
                                            Hlt2CharmHadLambdaC2LambdaPi]
                        , postscale = self.postscale
                        )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasLambdaC2LambdaPiDecision" : 55337 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasDplus2hhhDecision" : 55336 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasD02KKDecision" :55335  } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasD02KPiDecision" :55334  } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2CharmHadMinBiasLambdaC2KPPiDecision" :55333  } )
