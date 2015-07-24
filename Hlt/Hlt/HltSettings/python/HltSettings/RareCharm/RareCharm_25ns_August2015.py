from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class RareCharm_25ns_August2015 :
    """
    Threshold settings for Hlt2 topological lines for 2015 25 ns data-taking.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author Mike Sokoloff
    @date 2015-07-24
    """

    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = [
                    'Hlt2RareCharmD02KKMuMuFilter',
                    'Hlt2RareCharmD02KPi',
                    'Hlt2RareCharmD02MuMu',
                    'Hlt2RareCharmD02PiPieeFilter',
                    'Hlt2RareCharmD02KKMueFilter',
                    'Hlt2RareCharmLc2PMueFilter',
                    'Hlt2RareCharmD02PiPi',
                    'Hlt2RareCharmD2KMuMuSSFilter',
                    'Hlt2RareCharmD2PiMueFilter',
                    'Hlt2RareCharmD2KMueFilter',
                    'Hlt2RareCharmD02KPiMuMuFilter',
                    'Hlt2RareCharmD2KMuMuFilter',
                    'Hlt2RareCharmD2KeeFilter',
                    'Hlt2RareCharmD02KPiMueFilter',
                    'Hlt2RareCharmD02KPieeFilter',
                    'Hlt2RareCharmD02KKeeFilter',
                    'Hlt2RareCharmD2PiMuMuFilter',
                    'Hlt2RareCharmLc2PeeFilter',
                    'Hlt2RareCharmD2PieeFilter',
                    'Hlt2RareCharmD02KPiMuMuSSFilter',
                    'Hlt2RareCharmD2PiMuMuSSFilter',
                    'Hlt2RareCharmLc2PMuMuSSFilter',
                    'Hlt2RareCharmD02PiPiMuMuFilter',
                    'Hlt2RareCharmD02EMu',
                    'Hlt2RareCharmD02KMu',
                    'Hlt2RareCharmLc2PMuMuFilter',
                    'Hlt2RareCharmD02PiPiMueFilter',
                 ]

        return lines

    def Thresholds(self) :
        """Return the trigger thresholds."""

        d = {}

        from Hlt2Lines.RareCharm.Lines import RareCharmLines
        d.update( { RareCharmLines : 
                  {'Common': 
                   {'D0MinPT': 1800.0 * MeV,
                    'DDira': 0.9997,
                    'DDira_HHmumu': 0.9999,
                    'DDira_Hmumu': 0.9999,
                    'DIPChi2_HHmumu': 25.0,
                    'DIPChi2_Hmumu': 36.0,
                    'DMassWin': 70.0 * MeV,
                    'DMassWinSig': 300.0 * MeV,
                    'DMaxIPChi2': 15,
                    'DMinFlightChi2': 20,
                    'DSumPt_HHmumu': 3000.0 * MeV,
                    'DSumPt_Hmumu': 500.0 * MeV,
                    'DVChi2': 10,
                    'Lambda_c_Sig_M_MAX': 2370.0 * MeV,
                    'Lambda_c_Sig_M_MIN': 2200.0 * MeV,
                    'Lambda_c_WideMass_M_MAX': 2300.0 * MeV,
                    'Lambda_c_WideMass_M_MIN': 1700.0 * MeV,
                    'MCOR_MAX_HHmumu': 3500.0 * MeV,
                    'MCOR_MAX_Hmumu': 3500.0 * MeV,
                    'PairMaxDoca_HHmumu': 0.2 * mm,
                    'PairMaxDoca_Hmumu': 0.25 * mm,
                    'PairMinDoca_HHmumu': 0.1 * mm,
                    'PairMinDoca_Hmumu': 0.1 * mm,
                    'Sig_M_MAX_HHmumu': 1950.0 * MeV,
                    'Sig_M_MAX_Hmumu': 2050.0 * MeV,
                    'Sig_M_MIN_HHmumu': 1800.0 * MeV,
                    'Sig_M_MIN_Hmumu': 1800.0 * MeV,
                    'TrkChi2_HHmumu': 4.0,
                    'TrkChi2_Hmumu': 4.0,
                    'TrkPVIPChi2MAX_HHmumu': 12.0,
                    'TrkPVIPChi2MAX_Hmumu': 17.0,
                    'TrkPVIPChi2_HHmumu': 2.0,
                    'TrkPVIPChi2_Hmumu': 2.0,
                    'TrkPVIPChi2_XeeORmue': 3.0,
                    'TrkP_HHmumu': 3000.0 * MeV,
                    'TrkP_Hmumu': 3000.0 * MeV,
                    'TrkPtMAX_HHmumu': 0.0 * MeV,
                    'TrkPtMAX_Hmumu': 0.0 * MeV,
                    'TrkPt_HHmumu': 300.0 * MeV,
                    'TrkPt_Hmumu': 300.0 * MeV,
                    'VtxChi2_HHmumu': 15.0,
                    'VtxChi2_Hmumu': 20.0,
                    'VtxPVDispChi2_HHmumu': 36.0,
                    'VtxPVDispChi2_Hmumu': 20.0,
                    'VtxPVDispChi2_XeeORmue': 49.0,
                    'WideMass_M_MAX_HHmumu': 2100.0 * MeV,
                    'WideMass_M_MAX_Hmumu': 2100.0 * MeV,
                    'WideMass_M_MIN_HHmumu': 1700.0 * MeV,
                    'WideMass_M_MIN_Hmumu': 1700.0 * MeV,
                    'XTrackChi2': 5,
                    'XmaxIPChi2': 8,
                    'XmaxPT': 1100 * MeV,
                    'XminIPChi2': 3,
                    'XminP': 4000 * MeV,
                    'XminPT': 750 * MeV,
                    'doca': 1.0 * mm},
 'Prescale': {'Hlt2RareCharmD02KPi': 0.01, 'Hlt2RareCharmD02PiPi': 0.02},
 'TwoElectronForD2XXH': {'Pair_AMINDOCA_MAX_eeX': 0.1 * mm,
                         'Pair_BPVCORRM_MAX_eeX': 3500.0 * MeV,
                         'Pair_BPVVDCHI2_MIN_eeX': 20.0,
                         'Pair_BPVVD_MIN_eeX': 0.0 * mm,
                         'Pair_SumAPT_MIN_eeX': 0.0 * MeV,
                         'Trk_MIPCHI2DV_MIN_eeX': 3.0,
                         'Trk_PT_MIN_eeX': 300.0 * MeV,
                         'Trk_P_MIN_eeX': 3000.0 * MeV,
                         'Trk_TRCHI2DOF_MAX_eeX': 4.0},
 'TwoMuElForD2XXH': {'Pair_AMINDOCA_MAX_mueX': 0.1 * mm,
                     'Pair_BPVCORRM_MAX_mueX': 3500.0 * MeV,
                     'Pair_BPVVDCHI2_MIN_mueX': 20.0,
                     'Pair_BPVVD_MIN_mueX': 0.0 * mm,
                     'Pair_SumAPT_MIN_mueX': 0.0 * MeV,
                     'Trk_MIPCHI2DV_MIN_mueX': 3.0,
                     'Trk_PT_MIN_mueX': 300.0 * MeV,
                     'Trk_P_MIN_mueX': 3000.0 * MeV,
                     'Trk_TRCHI2DOF_MAX_mueX': 4.0},
 'TwoMuonForD2XXH': {'Pair_AMINDOCA_MAX_mumuX': 0.1 * mm,
                     'Pair_BPVCORRM_MAX_mumuX': 3500.0 * MeV,
                     'Pair_BPVVDCHI2_MIN_mumuX': 9.0,
                     'Pair_BPVVD_MIN_mumuX': 0.0 * mm,
                     'Pair_SumAPT_MIN_mumuX': 0.0 * MeV,
                     'Trk_MIPCHI2DV_MIN_mumuX': 2.0,
                     'Trk_PT_MIN_mumuX': 300.0 * MeV,
                     'Trk_P_MIN_mumuX': 3000.0 * MeV,
                     'Trk_TRCHI2DOF_MAX_mumuX': 4.0},
        }   }  )

        return d
