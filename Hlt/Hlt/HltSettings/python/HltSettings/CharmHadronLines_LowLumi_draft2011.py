class CharmHadronLines :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, V. Gligorov, P. Spradlin
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) : # {
        """
        Returns a list of active lines
        """

        lines = [ ### from Hlt2CharmHadD02HHLines
                  'Hlt2CharmHadD02HH_D02KK'
                  , 'Hlt2CharmHadD02HH_D02KPi'
                  , 'Hlt2CharmHadD02HH_D02PiPi'
                  ## Inclusive lines
                  #, 'Hlt2CharmHadD02HH2BodyInc'
                  ## Wide mass lines
                  , 'Hlt2CharmHadD02HH_D02KKWideMass'
                  , 'Hlt2CharmHadD02HH_D02KPiWideMass'
                  , 'Hlt2CharmHadD02HH_D02PiPiWideMass'

                  ### from Hlt2CharmHadD2HHHLines
                  , 'Hlt2CharmHadD2HHH'
                  , 'Hlt2CharmHadD2HHHWideMass'

                  ### from Hlt2CharmHadD02HHHHLines
                  , 'Hlt2CharmHadD02HHHH'
                  , 'Hlt2CharmHadD02HHHHWideMass'

                  ### from Hlt2CharmHadD02HHKsLines
                  , 'Hlt2CharmHadD02HHKsLL'
                  #, 'Hlt2CharmHadD02HHKsDD'

                  ### from Hlt2CharmHadD2KS0HLines
                  , 'Hlt2CharmHadD2KS0H_D2KS0Pi'
                  , 'Hlt2CharmHadD2KS0H_D2KS0K'
             ]
        
        return lines
    # }

    def Thresholds(self) : # {
        """
        Returns a dictionary of cuts
        """
        from GaudiKernel.SystemOfUnits import MeV, GeV, mm


        d = { }

        from Hlt2Lines.Hlt2CharmHadD02HHLines   import Hlt2CharmHadD02HHLinesConf
        from Hlt2Lines.Hlt2CharmHadD2HHHLines   import Hlt2CharmHadD2HHHLinesConf
        from Hlt2Lines.Hlt2CharmHadD2KS0HLines  import Hlt2CharmHadD2KS0HLinesConf
        from Hlt2Lines.Hlt2CharmHadD02HHKsLines import Hlt2CharmHadD02HHKsLinesConf
        from Hlt2Lines.Hlt2CharmHadTwoBodyForMultiBody     import Hlt2CharmHadTwoBodyForMultiBodyConf
        from Hlt2Lines.Hlt2CharmHadD02HHHHLines   import Hlt2CharmHadD02HHHHLinesConf
        d.update( { Hlt2CharmHadD02HHLinesConf       : {
                        ## Cut values for basic D0 -> hh signal lines
                          'Trk_PT_MIN'               : 800.0 * MeV
                        , 'Trk_P_MIN'                : 5.0  * GeV
                        , 'Trk_MIPCHI2DV_MIN'        : 2.0        # neuter
                        , 'Trk_TRCHI2DOF_MAX'        : 3.0        # neuter
                        , 'Pair_AMINDOCA_MAX'        : 0.10 * mm
                        , 'Trk_Max_APT_MIN'          : 1500.0 * MeV
                        , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                        , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                        , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                        , 'D0_PT_MIN'                : 2000.0 * MeV
                        , 'Sig_M_MIN'                : 1815.0 * MeV
                        , 'Sig_M_MAX'                : 1915.0 * MeV
                        , 'WideMass_M_MIN'           : 1715.0 * MeV
                        , 'WideMass_M_MAX'           : 2015.0 * MeV
                        ## 2-body inclusive!
                        , 'Inc_Trk_PT_MIN'           : 500.0 * MeV
                        , 'Inc_Trk_P_MIN'            : 5.0 * GeV
                        , 'Inc_Trk_MIPCHI2DV_MIN'    : 16.0       # neuter
                        , 'Inc_Trk_TRCHI2DOF_MAX'    : 3.0        # neuter
                        , 'Inc_Pair_AMINDOCA_MAX'    : 0.15 * mm
                        , 'Inc_D0_BPVVDCHI2_MIN'     : 100.0      # neuter
                        , 'Inc_Trk_Max_APT_MIN'      : 1000.0 * MeV
                        , 'Inc_D0_APT_MIN'           : 1500.0 * MeV
                        , 'Inc_D0_SumAPT_MIN'        : 1800.0 * MeV
                        , 'Inc_D0_BPVIPCHI2_MIN'     : 2.0        # neuter
                        , 'Inc_D0_BPVCORRM_MIN'      : 300.0 * MeV
                        , 'Inc_D0_BPVCORRM_MAX'      : 3200.0 * MeV
                        ## GEC
                        , 'GEC_Filter_NTRACK'        : True       # 
                        , 'GEC_NTRACK_MAX'           : 110        # 
                        , 'Postscale' : { 
                                          'Hlt2CharmHadD02KKWideMass'    : 0.1
                                         , 'Hlt2CharmHadD02KPiWideMass'  : 0.1
                                         , 'Hlt2CharmHadD02PiPiWideMass' : 0.1
                                        }
                    }

                    , Hlt2CharmHadTwoBodyForMultiBodyConf : {
                          'Trk_PT_MIN'                :  500.0 * MeV
                        , 'Trk_P_MIN'                 : 5000.0 * MeV
                        , 'Trk_MIPCHI2DV_MIN'         :   10.0      # unitless
                        , 'Trk_TRCHI2DOF_MAX'         :    5.0      # unitless
                        , 'Pair_AMINDOCA_MAX'         :    0.1 * mm
                        , 'Pair_BPVVDCHI2_MIN'        :   40.0      # unitless
                        , 'Pair_BPVVD_MIN'            :    3.0 * mm
                        , 'Pair_SumAPT_MIN'           : 1500.0 * MeV
                        , 'Pair_BPVCORRM_MAX'         : 3500.0 * MeV
                    }

                    , Hlt2CharmHadD2HHHLinesConf : {
                        ## 3Body
                          'TrkPt_3Body'                 : 250.0    # MeV
                        , 'TrkP_3Body'                  : 2000.0   # MeV
                        , 'TrkPVIPChi2_3Body'           : 2.0      # unitless
                        , 'TrkChi2_3Body'               : 5.0      # unitless
                        , 'PairMinDoca_3Body'           : 0.08     # mm
                        , 'VtxPVDispChi2_3Body'         : 150.0    # unitless
                        , 'VtxChi2_3Body'               : 20.0     # unitless
                        , 'DIPChi2_3Body'               : 15.0     # unitless
                        , 'DSumPt_3Body'                : 2000.0   # MeV
                        , 'MCOR_MAX_3Body'              : 3500.    # MeV
                        ## 2-body Input for 3Body
                        , 'TrkChi2_2BodyFor3Body'       : 5.0      # unitless
                        , 'GEC_Filter_NTRACK'           : True     # do or do not
                        , 'GEC_NTRACK_MAX'              : 110      # max number of tracks
                        # prescales
                        , 'Prescale'                    : {
                                'Hlt2CharmHadD2HHHWideMass'    : 0.1
                           }
                    }

                    , Hlt2CharmHadD02HHKsLinesConf : {
                          'KshhTFHHTrkPLL'          : 1500.0 * MeV
                        , 'KshhTFHHTrkChi2UL'       : 5.0
                        , 'KshhTFHHVtxChi2UL'       : 10.0
                        , 'KshhTFHHPtLL'            : 1000.0 * MeV
                        , 'KshhTFHHFDLL'            : 2.0 * mm
                        , 'KshhTFHHTightTrkChi2UL'  : 2.0
                        , 'KshhTFHHTightVtxChi2UL'  : 5.0
                        , 'KshhTFHHTightFDLL'       : 4.0 * mm
                        , 'KshhTFHHTightPtLL'       : 1000.0 * MeV
                        , 'KshhTFHHTightTrkPVIPChi2LL' : 12.5
                        , 'KshhTFKsLLTrkPLL'        : 2000.0 * MeV
                        , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0
                        , 'KshhTFKsLLTrkChi2UL'     : 20.0
                        , 'KshhTFKsLLCombSymMassWin': 50.0 * MeV
                        , 'KshhTFKsLLMothSymMassWin': 11.4 * MeV
                        , 'KshhTFKsLLVtxChi2UL'     : 30.0
                        , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0 * mm
                        , 'KshhTFKsLLVtxPVDispZUL'  : 650.0 * mm
                        , 'KshhTFKsLLVtxPVDispChi2LL' : 100.0
                        , 'KshhTFKsLLDiraLL'        : 0.9997
                        , 'KshhTFKsDDTrkPLL'        : 2000.0 * MeV
                        , 'KshhTFKsDDTrkPVIPChi2LL' : 4.0
                        , 'KshhTFKsDDTrkChi2UL'     : 10.0
                        , 'KshhTFKsDDCombSymMassWin': 80.0 * MeV
                        , 'KshhTFKsDDMothSymMassWin': 24.9 * MeV
                        , 'KshhTFKsDDVtxChi2UL'     : 20.0
                        , 'KshhTFKsDDVtxPVDispZLL'  : 0.0 * mm
                        , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0 * mm
                        , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0
                        , 'KshhTFKsDDDiraLL'        : 0.9999
                        , 'KshhTFDVtxChi2UL'        : 20.0
                        , 'KshhTFDPtLL'             : 2000.0 * MeV
                        , 'KshhTFDDiraLL'           : 0.0
                        , 'KshhTFDwKsLLSymMassWin'  : 100.0 * MeV
                        , 'KshhTFDwKsDDSymMassWin'  : 120.0 * MeV
                        , 'ComRobUseGEC'            : True
                        , 'ComRobGEC'               : 110
                        , 'HLT1FILTER'               : ''
                        , 'Prescale'                : { }
                        , 'Postscale'               : { }
                    }

                    , Hlt2CharmHadD02HHHHLinesConf : {
                        ## 4Body
                          'TrkPt_4Body'                 : 250.0 * MeV
                        , 'TrkP_4Body'                  : 2000.0 * MeV
                        , 'TrkPVIPChi2_4Body'           : 1.7      # unitless
                        , 'TrkChi2_4Body'               : 5.0      # unitless
                        , 'PairMinDoca_4Body'           : 0.10 * mm
                        , 'VtxPVDispChi2_4Body'         : 100.0    # unitless
                        , 'VtxChi2_4Body'               : 20.0     # unitless
                        , 'DIPChi2_4Body'               : 25.0     # unitless
                        , 'DSumPt_4Body'                : 1500.0 * MeV
                        , 'MCOR_MAX_4Body'              : 3500.0 * MeV
                        , 'Sig_M_MIN'                   : 1800.0 * MeV
                        , 'Sig_M_MAX'                   : 1930.0 * MeV
                        , 'WideMass_M_MIN'              : 1700.0 * MeV
                        , 'WideMass_M_MAX'              : 2100.0 * MeV
                        , 'TrkChi2_2BodyFor4Body'       : 5.0      # unitless
                        , 'GEC_Filter_NTRACK'           : True
                        , 'GEC_NTRACK_MAX'              : 110
                        # prescales
                        , 'Prescale'                  : {
                                'Hlt2CharmHadD02HHHHWideMass'    : 0.1
                        }
                    }

                    , Hlt2CharmHadD2KS0HLinesConf : {
                          'KS0DaugTrackChi2'       : 3.0
                        , 'KS0DaugMIPChi2'         : 90.0
                        , 'KS0VertexChi2'          : 15.0
                        , 'KS0PT'                  : 800.0 * MeV
                        , 'BachPionP'              : 4500.0 * MeV
                        , 'BachPionPT'             : 450.0 * MeV
                        , 'BachPionTrackChi2'      : 5.0
                        , 'BachPionMIPChi2'        : 30.0
                        , 'BachKaonP'              : 4500.0 * MeV
                        , 'BachKaonPT'             : 450. * MeV
                        , 'BachKaonTrackChi2'      : 5.0
                        , 'BachKaonMIPChi2'        : 30.0
                        , 'DMesonComboLowMass'     : 1760.0 * MeV
                        , 'DMesonComboHighMass'    : 2080.0 * MeV
                        , 'DMesonMotherLowMass'    : 1770.0 * MeV
                        , 'DMesonMotherHighMass'   : 2070.0 * MeV
                        , 'DMesonMotherVertexChi2' : 15.0
                        , 'DMesonMotherMIPChi2'    : 25.0
                        , 'DMesonMotherPT'         : 1800.0 * MeV
                        , 'Prescale'               : {
                              'Hlt2CharmHadD2KS0Pi'    : 1.0
                             , 'Hlt2CharmHadD2KS0K'    : 1.0 
                              }
                      }
                    
                  } )

        return d
    # }
            
   
