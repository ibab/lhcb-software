class CharmHadronLines :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) : # {
        """
        Returns a list of active lines
        """

        lines = [ ### from Hlt2CharmHadD02HHLines
                  #'Hlt2CharmHadD02KK'
                  #, 'Hlt2CharmHadD02KPi'
                  #, 'Hlt2CharmHadD02PiPi'
                  ## Inclusive lines
                  #, 'Hlt2CharmHad2BodyInc'
                  ## Wide mass lines
                  #, 'Hlt2CharmHadD02KKWideMass'
                  #, 'Hlt2CharmHadD02KPiWideMass'
                  #, 'Hlt2CharmHadD02PiPiWideMass'

                  ### from Hlt2CharmHadD2HHHLines
                  #, 'Hlt2CharmHad2BodyForD2HHH'
                    'Hlt2CharmHadD2HHH'
                  , 'Hlt2CharmHadD2HHHWideMass'
             ]
        
        return lines
    # }

    def Thresholds(self) : # {
        """
        Returns a dictionary of cuts
        """
        d = { }

        from Hlt2Lines.Hlt2CharmHadD02HHLines import Hlt2CharmHadD02HHLinesConf
        from Hlt2Lines.Hlt2CharmHadD2HHHLines import Hlt2CharmHadD2HHHLinesConf
        d.update( { Hlt2CharmHadD02HHLinesConf       : {
                        ## Cut values for basic D0 -> hh signal lines
                          'Trk_PT_MIN'               : 800.0      # MeV
                        , 'Trk_P_MIN'                : 5000.0     # MeV
                        , 'Trk_MIPCHI2DV_MIN'        : 2.0        # neuter
                        , 'Trk_TRCHI2DOF_MAX'        : 5.0        # neuter
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
                                          'Hlt2CharmHadD02KKWideMass'    : 0.05
                                         , 'Hlt2CharmHadD02KPiWideMass'  : 0.05
                                         , 'Hlt2CharmHadD02PiPiWideMass' : 0.05
                                        }
                    }

                    , Hlt2CharmHadD2HHHLinesConf : {
                        ## 3Body
                          'TrkPt_3Body'                 : 250.0    # MeV
                        , 'TrkP_3Body'                  : 2000.0   # MeV
                        , 'TrkPVIPChi2_3Body'           : 2.0      # unitless
                        , 'TrkChi2_3Body'               : 5.0      # unitless
                        , 'PairMinDoca_3Body'           : 0.10     # mm
                        , 'VtxPVDispChi2_3Body'         : 100.0    # unitless
                        , 'VtxChi2_3Body'               : 20.0     # unitless
                        , 'DIPChi2_3Body'               : 25.0     # unitless
                        , 'DSumPt_3Body'                : 2000.0   # MeV
                        , 'MCOR_MAX_3Body'              : 3500.    # MeV
                        ## 2-body Input for 3Body
                        , 'TrkPt_2BodyFor3Body'         : 500.0    # MeV
                        , 'TrkP_2BodyFor3Body'          : 5000.0   # MeV
                        , 'TrkPVIPChi2_2BodyFor3Body'   : 7.0      # unitless
                        , 'TrkChi2_2BodyFor3Body'       : 5.0      # unitless
                        , 'Doca_2BodyFor3Body'          : 0.085    # mm
                        , 'VtxPVDispChi2_2BodyFor3Body' : 40.      # unitless
                        , 'VtxPVDisp_2BodyFor3Body'     : 3.0      # mm
                        , 'DSumPt_2BodyFor3Body'        : 1400.0   # MeV
                        , 'MCOR_MAX_2BodyFor3Body'      : 3500.    # MeV
                        # prescales
                        , 'Prescale'                    : {
                                'Hlt2CharmHadD2HHHWideMass'    : 0.1
                           }
                    }

                  } )

        return d
    # }
            
   
