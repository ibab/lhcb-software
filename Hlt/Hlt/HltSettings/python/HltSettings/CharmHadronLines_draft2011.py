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
                  'Hlt2CharmHadD02KK'
                  , 'Hlt2CharmHadD02KPi'
                  , 'Hlt2CharmHadD02PiPi'
                  ## Inclusive lines
                  #, 'Hlt2CharmHad2BodyInc'
                  ## Wide mass lines
                  , 'Hlt2CharmHadD02KKWideMass'
                  , 'Hlt2CharmHadD02KPiWideMass'
                  , 'Hlt2CharmHadD02PiPiWideMass'

                  ### from Hlt2CharmHadD2HHHLines
                  #, 'Hlt2CharmHad2BodyForD2HHH'
                  #, 'Hlt2CharmHadD2HHH'
                  #, 'Hlt2CharmHadD2HHHWideMass'
                  #, 'Hlt2CharmHadD02HHHH'
                  #, 'Hlt2CharmHadD02HHHHWideMass'
                  , 'Hlt2CharmHadD02HHKsLL'
                  , 'Hlt2CharmHadD2KS0Pi'
                  , 'Hlt2CharmHadD2KS0K'
             ]
        
        return lines
    # }

    def Thresholds(self) : # {
        """
        Returns a dictionary of cuts
        """
        d = { }

        from Hlt2Lines.Hlt2CharmHadD02HHLines   import Hlt2CharmHadD02HHLinesConf
        from Hlt2Lines.Hlt2CharmHadD2HHHLines   import Hlt2CharmHadD2HHHLinesConf
        from Hlt2Lines.Hlt2D2KS0HLines          import Hlt2CharmHadD2KS0HLinesConf
        from Hlt2Lines.Hlt2CharmHadD02HHKsLines import Hlt2CharmHadD02HHKsLinesConf
        d.update( { Hlt2CharmHadD02HHLinesConf       : {
                        ## Cut values for basic D0 -> hh signal lines
                          'Trk_PT_MIN'               : 800.0      # MeV
                        , 'Trk_P_MIN'                : 5000.0     # MeV
                        , 'Trk_MIPCHI2DV_MIN'        : 2.0        # neuter
                        , 'Trk_TRCHI2DOF_MAX'        : 3.0        # neuter
                        , 'Pair_AMINDOCA_MAX'        : 0.10       # mm
                        , 'Trk_Max_APT_MIN'          : 1500.0     # MeV
                        , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                        , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                        , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                        , 'D0_PT_MIN'                : 2000.0     # MeV
                        , 'Sig_M_MIN'                : 1815.0     # MeV
                        , 'Sig_M_MAX'                : 1915.0     # MeV
                        , 'WideMass_M_MIN'           : 1715.0     # MeV
                        , 'WideMass_M_MAX'           : 2015.0     # MeV
                        ## 2-body inclusive!
                        , 'Inc_Trk_PT_MIN'           : 500.0      # MeV
                        , 'Inc_Trk_P_MIN'            : 5000.0     # MeV
                        , 'Inc_Trk_MIPCHI2DV_MIN'    : 16.0       # neuter
                        , 'Inc_Trk_TRCHI2DOF_MAX'    : 3.0        # neuter
                        , 'Inc_Pair_AMINDOCA_MAX'    : 0.15       # in mm
                        , 'Inc_D0_BPVVDCHI2_MIN'     : 100.0      # neuter
                        , 'Inc_Trk_Max_APT_MIN'      : 1000.0     # MeV
                        , 'Inc_D0_PT_MIN'            : 1500.0     # MeV
                        , 'Inc_D0_SUMPT_MIN'         : 1800.0     # MeV
                        , 'Inc_D0_BPVIPCHI2_MIN'     : 2.0        # neuter
                        , 'Inc_D0_MCOR_MIN'          : 300.       # MeV
                        , 'Inc_D0_MCOR_MAX'          : 3200.      # MeV
                        ## GEC
                        , 'GEC_Filter_NTRACK'        : True       # 
                        , 'GEC_NTRACK_MAX'           : 120        # 
                        , 'Postscale' : { 
                                          'Hlt2CharmHadD02KKWideMass'    : 0.1
                                         , 'Hlt2CharmHadD02KPiWideMass'  : 0.1
                                         , 'Hlt2CharmHadD02PiPiWideMass' : 0.1
                                        }
                    }

                    , Hlt2CharmHadD2HHHLinesConf : {
                        ## 3Body
                          'TrkPt_3Body'                 : 250.0    # MeV
                        , 'TrkP_3Body'                  : 2000.0   # MeV
                        , 'TrkPVIPChi2_3Body'           : 5.0      # unitless
                        , 'TrkChi2_3Body'               : 3.0      # unitless
                        , 'PairMinDoca_3Body'           : 0.08     # mm
                        , 'VtxPVDispChi2_3Body'         : 150.0    # unitless
                        , 'VtxChi2_3Body'               : 20.0     # unitless
                        , 'DIPChi2_3Body'               : 15.0     # unitless
                        , 'DSumPt_3Body'                : 2500.0   # MeV
                        , 'MCOR_MAX_3Body'              : 3500.    # MeV
                        ## 2-body Input for 3Body
                        , 'TrkPt_2BodyFor3Body'         : 500.0    # MeV
                        , 'TrkP_2BodyFor3Body'          : 5000.0   # MeV
                        , 'TrkPVIPChi2_2BodyFor3Body'   : 10.0      # unitless
                        , 'TrkChi2_2BodyFor3Body'       : 3.0      # unitless
                        , 'Doca_2BodyFor3Body'          : 0.1    # mm
                        , 'VtxPVDispChi2_2BodyFor3Body' : 40.      # unitless
                        , 'VtxPVDisp_2BodyFor3Body'     : 3.0      # mm
                        , 'DSumPt_2BodyFor3Body'        : 2000.0   # MeV
                        , 'MCOR_MAX_2BodyFor3Body'      : 3500.    # MeV
                        , 'GEC_Filter_NTRACK'           : True     # do or do not
                        , 'GEC_NTRACK_MAX'              : 110      # max number of tracks
                        # prescales
                        , 'Prescale'                    : {
                                'Hlt2CharmHadD2HHHWideMass'    : 0.1
                           }
                    }

                    , Hlt2CharmHadD02HHKsLinesConf : {
                        ## Slots for K_S h h' lines
                          'KshhTFHHTrkPLL'          : 1500.0     # in MeV
                        , 'KshhTFHHTrkChi2UL'       : 5.0        # unitless
                        , 'KshhTFHHTrkMaxPVIPLL'    : 0.1        # in mm
                        , 'KshhTFHHVtxChi2UL'       : 10.0       # unitless
                        , 'KshhTFHHVtxPVDispLL'     : -1.0       # in mm
                        , 'KshhTFHHPtLL'            : 500.0      # in MeV
                        , 'KshhTFKsLLTrkPLL'        : 2000.0     # in MeV
                        , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                        , 'KshhTFKsLLTrkChi2UL'     : 20.0       # unitless
                        , 'KshhTFKsLLCombSymMassWin': 50.0       # in MeV
                        , 'KshhTFKsLLMothSymMassWin': 11.4       # in MeV
                        , 'KshhTFKsLLVtxChi2UL'     : 30.0       # unitless
                        , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0    # in mm
                        , 'KshhTFKsLLVtxPVDispZUL'  : 650.0      # in mm
                        , 'KshhTFKsLLVtxPVDispChi2LL' : 100.0    # unitless
                        , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                        , 'KshhTFKsDDTrkPLL'        : 2000.0     # in MeV
                        , 'KshhTFKsDDTrkPVIPChi2LL' : 4.0        # unitless
                        , 'KshhTFKsDDTrkChi2UL'     : 10.0       # unitless
                        , 'KshhTFKsDDCombSymMassWin': 80.0       # in MeV
                        , 'KshhTFKsDDMothSymMassWin': 24.9       # in MeV
                        , 'KshhTFKsDDVtxChi2UL'     : 30.0       # unitless
                        , 'KshhTFKsDDVtxPVDispZLL'  : 0.0        # in mm
                        , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0     # in mm
                        , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                        , 'KshhTFKsDDDiraLL'        : 0.9999     # unitless
                        , 'KshhTFDVtxChi2UL'        : 20.0       # unitless
                        , 'KshhTFDVtxPVDispLL'      : -1.0       # in mm
                        , 'KshhTFDPtLL'             : 2000.0     # in MeV
                        , 'KshhTFDDiraLL'           : 0.0        # unitless
                        , 'KshhTFDwKsLLSymMassWin'  : 100.0      # in MeV
                        , 'KshhTFDwKsDDSymMassWin'  : 120.0      # in MeV
                        , 'ComRobUseGEC'            : True       # do or do not 
                        , 'ComRobGEC'               : 120        # max number of tracks
                        , 'HLT1FILTER'               : ''
                        , 'Prescale'                : { }
                        , 'Postscale'               : { }
                    }
                    
                  } )

        return d
    # }
            
   
