from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class RareCharm_25ns_Draft2016(object) :
    """
    Threshold settings for Hlt2 topological lines for 2015 25 ns data-taking.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author Mike Sokoloff
    @date 2015-07-24
    """

    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = ['Hlt2RareCharmD02MuMu',
                 'Hlt2RareCharmD02PiPi',
                 'Hlt2RareCharmD02KPi',
                 'Hlt2RareCharmD02KMu',
                 'Hlt2RareCharmD02EMu',
                 'Hlt2RareCharmD2PiMuMuOS',
                 'Hlt2RareCharmD2PiMuMuSS',
                 'Hlt2RareCharmD2PiMuMuWS',
                 'Hlt2RareCharmD2KMuMuOS',
                 'Hlt2RareCharmD2KMuMuSS',
                 'Hlt2RareCharmD2KMuMuWS',
                 'Hlt2RareCharmD2PiEEOS',
                 'Hlt2RareCharmD2PiEESS',
                 'Hlt2RareCharmD2PiEEWS',
                 'Hlt2RareCharmD2KEEOS',
                 'Hlt2RareCharmD2KEESS',
                 'Hlt2RareCharmD2KEEWS',
                 'Hlt2RareCharmD2PiMuEOS',
                 'Hlt2RareCharmD2PiMuESS',
                 'Hlt2RareCharmD2PiMuEWS',
                 'Hlt2RareCharmD2PiEMuOS',
                 'Hlt2RareCharmD2KMuEOS',
                 'Hlt2RareCharmD2KMuESS',
                 'Hlt2RareCharmD2KMuEWS',
                 'Hlt2RareCharmD2KEMuOS',
                 'Hlt2RareCharmLc2PMuMu',
                 'Hlt2RareCharmLc2PMuMuSS',
                 'Hlt2RareCharmLc2Pee',
                 'Hlt2RareCharmLc2PMue',
                 'Hlt2RareCharmD02PiPiMuMu',
                 'Hlt2RareCharmD02KKMuMu',
                 'Hlt2RareCharmD02KPiMuMu',
                 'Hlt2RareCharmD02PiPiee',
                 'Hlt2RareCharmD02KKee',
                 'Hlt2RareCharmD02KPiee',
                 'Hlt2RareCharmD02PiPiMue',
                 'Hlt2RareCharmD02KKMue',
                 'Hlt2RareCharmD02KPiMue',
                 'Hlt2RareCharmD02KPiMuMuSS']
        return lines

    def Thresholds(self) :
        """Return the trigger thresholds."""

        d = {}

        from Hlt2Lines.RareCharm.Lines import RareCharmLines
        d.update( { RareCharmLines : {'Prescale' : {"Hlt2RareCharmD02KPi"    : 0.01,
                                                    "Hlt2RareCharmD02PiPi"    : 0.02},
                                      'Common' : {#two body lines
                                                 'XTrackChi2'         : 5               # adimensional
                                                 ,'XminP'              : 4000 * MeV      # MeV
                                                 ,'XminPT'             : 750 * MeV       # MeV
                                                 ,'XmaxPT'             : 1100 *MeV       # MeV
                                                 ,'XminIPChi2'         : 3               # adimensional
                                                 ,'XmaxIPChi2'         : 8               # adimensional

                                                 ,'DMassWin'           : 70. * MeV       # MeV
                                                 ,'DMassWinSig'        : 300. * MeV      # MeV
                                                 ,'D0MinPT'            : 1800. * MeV     # MeV
                                                 ,'doca'               : 1.0 * mm        # mm

                                                 ,'DVChi2'             : 10         # adimensional
                                                 ,'DDira'              : 0.9997     # adimensional
                                                 ,'DMinFlightChi2'     : 20         # adimensional
                                                 ,'DMaxIPChi2'         : 15         # adimensional
                                                 #three body lines
                                                 ,'TrkPt_Hmumu'                 : 300.0 * MeV
                                                 ,'TrkP_Hmumu'                  : 3000.0 * MeV ## 1
                                                 ,'TrkPVIPChi2_Hmumu'           : 2.0      # unitless
                                                 ,'TrkPVIPChi2MAX_Hmumu'       : 17.0      #0 unitless
                                                 ,'TrkPtMAX_Hmumu'             : 0.0 * MeV
                                                 ,'TrkChi2_Hmumu'               : 4.0      # unitless
                                                 ,'PairMinDoca_Hmumu'           : 0.1 * mm
                                                 ,'PairMaxDoca_Hmumu'           : 0.25* mm
                                                 ,'VtxPVDispChi2_Hmumu'         : 20.0    #20 unitless
                                                 ,'VtxChi2_Hmumu'               : 20.0     # unitless
                                                 ,'DIPChi2_Hmumu'               : 36.0     #4  unitless
                                                 ,'DSumPt_Hmumu'                : 500.0 * MeV ## 1
                                                 ,'DDira_Hmumu'              : 0.9999     # adimensional
                                                 ,'MCOR_MAX_Hmumu'              : 3500.0 * MeV
                                                 ,'Sig_M_MIN_Hmumu'                   : 1800.0 * MeV
                                                 ,'Sig_M_MAX_Hmumu'                   : 2050.0 * MeV
                                                 ,'WideMass_M_MIN_Hmumu'              : 1700.0 * MeV
                                                 ,'WideMass_M_MAX_Hmumu'              : 2100.0 * MeV
                                                 ,'Lambda_c_Sig_M_MIN'                   : 2200.0 * MeV
                                                 ,'Lambda_c_Sig_M_MAX'                   : 2370.0 * MeV
                                                 ,'Lambda_c_WideMass_M_MIN'              : 1700.0 * MeV
                                                 ,'Lambda_c_WideMass_M_MAX'              : 2300.0 * MeV
                                                 #three body lines for LFV/LNV
                                                 ,'TrkChi2_HLL'               : 3.0
                                                 ,'TrkPt_HLL'                 : 300.0 * MeV
                                                 ,'TrkP_HLL'                  : 2000.0 * MeV
                                                 ,'TrkPVIPChi2_HLL'           : 5.0
                                                 ,'DMassWin_HLL'              : 200.0 * MeV
                                                 ,'DMAXDOCA_HLL'              : 0.15 * mm
                                                 ,'DMassLow_HLL'              : 1763.0 * MeV
                                                 ,'DimuonMass_HLL'            : 250.0 * MeV
                                                 ,'DVCHI2DOF_HLL'             : 5
                                                 ,'DIPCHI2_HLL'               : 25
                                                 ,'BPVVD_HLL'             : 20.0
                                                 ,'DDIRA_HLL'                 : 0.9999
                                         #four body lines
                                                 ,'TrkPt_HHmumu'                 : 300.0 * MeV
                                                 , 'TrkP_HHmumu'                  : 3000.0 * MeV
                                                 , 'TrkPVIPChi2_HHmumu'           : 2.0      # unitless
                                                 , 'TrkPVIPChi2MAX_HHmumu'           : 12.0      # !!unitless
                                                 , 'TrkPtMAX_HHmumu'                 : 0.0 * MeV
                                                 , 'TrkChi2_HHmumu'               : 4.0      # unitless
                                                 , 'PairMinDoca_HHmumu'           : 0.1 * mm
                                                 , 'PairMaxDoca_HHmumu'           : 0.2 * mm
                                                 , 'VtxPVDispChi2_HHmumu'         : 36.0    #  unitless
                                                 , 'VtxChi2_HHmumu'               : 15.0     # unitless
                                                 , 'DIPChi2_HHmumu'               : 25.0     #  unitless
                                                 , 'DSumPt_HHmumu'                : 3000.0 * MeV
                                                 , 'DDira_HHmumu'              : 0.9999     # adimensional
                                                 , 'MCOR_MAX_HHmumu'              : 3500.0 * MeV
                                                 , 'Sig_M_MIN_HHmumu'                   : 1800.0 * MeV
                                                 , 'Sig_M_MAX_HHmumu'                   : 1950.0 * MeV
                                                 , 'WideMass_M_MIN_HHmumu'              : 1700.0 * MeV
                                                 , 'WideMass_M_MAX_HHmumu'              : 2100.0 * MeV
                                                 #for Lines with Electrons
                                                 , 'VtxPVDispChi2_XeeORmue'         : 49.0     #  unitless
                                                 , 'TrkPVIPChi2_XeeORmue'           : 3.0      # unitless
                                                 },
                                        'TwoMuonForD2XXH' : {
                                                   'Trk_PT_MIN_mumuX'                :  300.0 * MeV# 500.0 /300
                                                 , 'Trk_P_MIN_mumuX'                 : 3000.0 * MeV # 5000.0 /3000
                                                 , 'Trk_MIPCHI2DV_MIN_mumuX'         :    2.0      # 2.0 /0  unitless
                                                 , 'Trk_TRCHI2DOF_MAX_mumuX'         :    4.0      # unitless
                                                 , 'Pair_AMINDOCA_MAX_mumuX'         :    0.1 * mm
                                                 , 'Pair_BPVVDCHI2_MIN_mumuX'        :    9.0      # 0.0 unitless
                                                 , 'Pair_BPVVD_MIN_mumuX'            :    0.0 * mm
                                                 , 'Pair_SumAPT_MIN_mumuX'           : 0.0 * MeV
                                                 , 'Pair_BPVCORRM_MAX_mumuX'         : 3500.0 * MeV
                                                   },
                                        'TwoElectronForD2XXH' : {
                                                   'Trk_PT_MIN_eeX'                :  300.0 * MeV# 500.0 /300
                                                 , 'Trk_P_MIN_eeX'                 : 3000.0 * MeV # 5000.0 /3000
                                                 , 'Trk_MIPCHI2DV_MIN_eeX'         :    3.0      # 2.0 /0  unitless
                                                 , 'Trk_TRCHI2DOF_MAX_eeX'         :    4.0      # unitless
                                                 , 'Pair_AMINDOCA_MAX_eeX'         :    0.1 * mm
                                                 , 'Pair_BPVVDCHI2_MIN_eeX'        :    20.0      # 0.0 unitless
                                                 , 'Pair_BPVVD_MIN_eeX'            :    0.0 * mm
                                                 , 'Pair_SumAPT_MIN_eeX'           : 0.0 * MeV
                                                 , 'Pair_BPVCORRM_MAX_eeX'         : 3500.0 * MeV
                                                   },
                                       'TwoMuElForD2XXH' : {
                                                   'Trk_PT_MIN_mueX'                :  300.0 * MeV# 500.0 /300
                                                 , 'Trk_P_MIN_mueX'                 : 3000.0 * MeV # 5000.0 /3000
                                                 , 'Trk_MIPCHI2DV_MIN_mueX'         :    3.0      # 2.0 /0  unitless
                                                 , 'Trk_TRCHI2DOF_MAX_mueX'         :    4.0      # unitless
                                                 , 'Pair_AMINDOCA_MAX_mueX'         :    0.1 * mm
                                                 , 'Pair_BPVVDCHI2_MIN_mueX'        :    20.0      # 0.0 unitless
                                                 , 'Pair_BPVVD_MIN_mueX'            :    0.0 * mm
                                                 , 'Pair_SumAPT_MIN_mueX'           : 0.0 * MeV
                                                 , 'Pair_BPVCORRM_MAX_mueX'         : 3500.0 * MeV},
                                      }
                  }  )
        return d
