from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class CharmHad_EM2015 :
    """
    Threshold settings for Hlt2 CharmHad lines: EM 2015

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author C. Linn
    @date 2015-05-19
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2CharmHadDpm2KPiPi_XSecTurbo',
            'Hlt2CharmHadDpm2KKPi_XSecTurbo',
            'Hlt2CharmHadDs2KKPi_XSecTurbo',
            'Hlt2CharmHadDs2PiPiPi_XSecTurbo',
            'Hlt2CharmHadLc2KPPi_XSecTurbo',
            'Hlt2CharmHadLc2KPK_XSecTurbo',
            'Hlt2CharmHadLc2PiPPi_XSecTurbo',
            'Hlt2CharmHadD02KPi_XSecTurbo',
            'Hlt2CharmHadDst_2D0Pi_D02KPi_XSecTurbo',
            'Hlt2CharmHadDst_2D0Pi0_D02KPi_XSecTurbo',
            'Hlt2CharmHadDst_2DsGamma_Ds2KKPi_XSecTurbo',
            'Hlt2CharmHadDst_2D0Pi_D02K3Pi_XSecTurbo',
            'Hlt2CharmHadDst_2D0Gamma_D02KPi_XSecTurbo',
            'Hlt2CharmHadSigmac_2LcPi_XSecTurbo',
            'Hlt2CharmHadXic02PKKPi_Turbo'
       ]


        return lines

    def Thresholds(self) :

        d = {}
        from Hlt2Lines.CharmHad.Lines import CharmHadLines
        d.update ({CharmHadLines : { 
                 ## Common here for full explication.  Otherwise it would
                 ##   default to the common defined in CharmHad/Lines.py
                 'Common'       : {
                                 'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,
                                 'Trk_ALL_P_MIN'          :  1000 * MeV,
                                 'VCHI2PDOF_MAX'          :  10.0
                                  },
                 ## Input particle definitions here for full explication.
                 ## SharedDetachedDpmChild used by the following lines:
                 ##     Dpm2KPiPi_XSecTurbo
                 ##     Ds2KKPi_XSecTurbo
                 ##     Ds2PiPiPi_XSecTurbo
                 ##     D02KPi_XSecTurbo
                 ##     Dst_2D0Pi_D02K3Pi_XSecTurbo
                 ##     Dst_2DsGamma_Ds2KKPi_XSecTurbo
                 ##     Dst_2D0Pi_D02KPi_XSecTurbo
                 'CharmHadSharedDetachedDpmChild_K' : {
                                 'PID_LIM'                  :  5.0,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'CharmHadSharedDetachedDpmChild_pi' : {
                                 'PID_LIM'                  :  5.0,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 ## SharedDetachedLcChild used by
                 ##     Lc2KPPi_XSecTurbo
                 ##     Lc2PiPPi_XSecTurbo
                 ##     Lc2KPK_XSecTurbo
                 ##     Sigmac_2LcPi_XSecTurbo
                 'CharmHadSharedDetachedLcChild_K' : {
                                 'PID_LIM'                  :  5.0,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'CharmHadSharedDetachedLcChild_pi' : {
                                 'PID_LIM'                  :  5.0,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 'CharmHadSharedDetachedLcChild_p' : {
                                 'PID_LIM'                  :  5.0,
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                              },
                 ## SharedPromptChild used by line Xic02PKKPi_Turbo
                 'CharmHadSharedPromptChild_K' : {
                                 'PID_LIM'                  :  5.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                              },
                 'CharmHadSharedPromptChild_pi' : {
                                 'PID_LIM'                  :  3.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                              },
                 'CharmHadSharedPromptChild_p' : {
                                 'PID_LIM'                  :  10.0,
                                 'Trk_ALL_PT_MIN'           :  250.0 * MeV,
                                              },
                 # Soft particles for tagging decays
                 'CharmHadSharedSoftTagChild_pi' : {
                                 'Trk_ALL_PT_MIN'           :  100.0 * MeV,
                                           },

                 'D02HH_XSec'   : {
                                  'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                  'Pair_AMINDOCA_MAX'        : 0.10 * mm,
                                  'Trk_Max_APT_MIN'          : 500.0 * MeV, 
                                  'D0_BPVVDCHI2_MIN'         : 49.0,       # neuter
                                  'D0_BPVDIRA_MIN'           : 0.99985,    # neuter
                                  'D0_VCHI2PDOF_MAX'         : 10.0,       # neuter
                                  'D0_PT_MIN'                : 0.0 * MeV, 
                                  'Comb_AM_MIN'              : 1775.0 * MeV, 
                                  'Comb_AM_MAX'              : 1955.0 * MeV, 
                                  ## These should be removed to input particle filtering
                                  'Trk_ALL_PT_MIN'           : 250.0 * MeV, 
                                  'Trk_ALL_P_MIN'            : 2.0  * GeV, 
                                  'Trk_ALL_MIPCHI2DV_MIN'    : 16.0, 
                                  ## Add mass filter after combination cut
                                  'Mass_M_MIN'               :  1784.0 * MeV,
                                  'Mass_M_MAX'               :  1944.0 * MeV,
                                },
                 'D02HHHH_XSec' : {
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4,
                                 'AM_34'                    :  (139.5 + 139.5) * MeV,
                                 'AM_4'                     :  (139.5) * MeV,
                                 'AM_MIN'                   :  1774 * MeV,
                                 'AM_MAX'                   :  1954 * MeV,
                                 'ASUMPT_MIN'               :  0.0 * MeV,
                                 'ADOCA_MAX'                :  0.1 * mm,
                                 'ACHI2DOCA_MAX'            :  99999999.0,
                                 'VCHI2PDOF_MAX'            :  10.0,
                                 'BPVDIRA_MIN'              :  0.99985,
                                 'BPVLTIME_MIN'             :  0.1*picosecond,
                                 'Mass_M_MIN'               :  1784.0 * MeV,
                                 'Mass_M_MAX'               :  1944.0 * MeV,
                                },
                 'Dpm2HHH_XSec' : {
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'BPVVDCHI2_MIN'            :  16.0,
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH_XSec' : { 
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'BPVVDCHI2_MIN'            :  16.0,
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 'Lc2HHH_XSec' : {
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0, 
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'BPVVDCHI2_MIN'            :  4.0,
                                 'BPVLTIME_MIN'             :  0.075 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2553 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },
                 'D0_TAG_XSec' : {
                                 'DeltaM_AM_MIN'            :  130.0 * MeV,
                                 'DeltaM_MIN'               :  130.0 * MeV,
                                 'DeltaM_AM_MAX'            :  165.0 * MeV,
                                 'DeltaM_MAX'               :  160.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  25.0
                                },
                 'D_TAG_NEUTRAL_XSec' : {
                                 'DeltaM_AM_MIN'            :  130.0 * MeV,
                                 'DeltaM_MIN'               :  130.0 * MeV,
                                 'DeltaM_AM_MAX'            :  165.0 * MeV,
                                 'DeltaM_MAX'               :  160.0 * MeV,
                                },
                 'Sigmac_TAG_XSec' : { 
                                 'DeltaM_AM_MIN'            :  150.0 * MeV,
                                 'DeltaM_MIN'               :  155.0 * MeV,
                                 'DeltaM_AM_MAX'            :  205.0 * MeV,
                                 'DeltaM_MAX'               :  200.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  25.0
                                },
                 'Xic02PKKPi_Turbo' : {
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                 'Trk_ALL_PT_MIN'           : 250.*MeV,
                                 'Trk_2OF4_PT_MIN'          : 400.*MeV,
                                 'Trk_1OF4_PT_MIN'          : 800.*MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    : 4.0,
                                 'Trk_2OF4_MIPCHI2DV_MIN'   : 4.0,
                                 'Trk_1OF4_MIPCHI2DV_MIN'   : 4.0,
                                 'ASUMPT_MIN'               : 0.0 * MeV,
                                 'AM_MIN'                   :  2386.0 * MeV,
                                 'AM_MAX'                   :  2556.0 * MeV,
                                 'Mass_M_MIN'               :  2396.0 * MeV,
                                 'Mass_M_MAX'               :  2546.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  10.0,
                                 'BPVDIRA_MIN'              :  0.99939,
                                 'BPVVDCHI2_MIN'            : 4.0,
                                 'BPVLTIME_MIN'             : 0.075 * picosecond
                                },
                }
        })

        return d
