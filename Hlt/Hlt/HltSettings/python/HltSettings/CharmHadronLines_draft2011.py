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

        lines = [ ## from Hlt2CharmHadD02HHLines
                  #'Hlt2CharmD02KK'
                  #, 'Hlt2CharmD02KPi'
                  #, 'Hlt2CharmD02PiPi'
                  ## Inclusive lines
                  #, 'Hlt2Charm2BodyInc'
                  ## Wide mass lines
                  #, 'Hlt2CharmD02KKWideMass'
                  #, 'Hlt2CharmD02KPiWideMass'
                  #, 'Hlt2CharmD02PiPiWideMass'
             ]
        
        return lines
    # }

    def Thresholds(self) : # {
        """
        Returns a dictionary of cuts
        """
        d = { }

        from Hlt2Lines.Hlt2CharmHadD02HHLines import Hlt2CharmHadD02HHLinesConf
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
                        , 'Sig_M_MIN'                : 1839.0     # MeV
                        , 'Sig_M_MAX'                : 1889.0     # MeV
                        , 'WideMass_M_MIN'           : 1700.0     # MeV
                        , 'WideMass_M_MAX'           : 2100.0     # MeV
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
                                          'Hlt2CharmD02KKWideMass'     : 0.05
                                         , 'Hlt2CharmD02KPiWideMass'  : 0.05
                                         , 'Hlt2CharmD02PiPiWideMass' : 0.05
                                        }
                    }
                  } )

        return d
    # }
            
   
