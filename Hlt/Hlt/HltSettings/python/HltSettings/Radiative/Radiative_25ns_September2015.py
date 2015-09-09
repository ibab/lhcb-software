# =============================================================================
# @file   Radiative_25ns_September2015.py
# @author Albert Puig (albert.puig@epfl.ch)
# @date   29.07.2015
# =============================================================================
"""Threshold settings for Hlt2 Radiative lines for 2015.
"""


class Radiative_25ns_September2015(object):
    """Threshold settings for Hlt2 Radiative lines for 2015."""
    __all__ = ('ActiveHlt2Lines', 'Thresholds')

    def ActiveHlt2Lines(self):
        """Returns a list of active lines."""

        lines = [# Exclusive B0 - >K* gamma and Bs - >phi gamma
                 'Hlt2RadiativeBs2PhiGamma',
                 'Hlt2RadiativeBd2KstGamma',
                 'Hlt2RadiativeBs2PhiGammaUnbiased',
                 'Hlt2RadiativeBd2KstGammaULUnbiased',
                 # Inclusive
                 'Hlt2RadiativeIncHHGamma',
                 'Hlt2RadiativeIncHHHGamma',
                 'Hlt2RadiativeIncHHGammaEE',
                 'Hlt2RadiativeIncHHHGammaEE',
                 # Exclusive Bs -> gamma gamma
                 'Hlt2RadiativeB2GammaGamma',
                 'Hlt2RadiativeB2GammaGammaLL',
                 'Hlt2RadiativeB2GammaGammaDD',
                 'Hlt2RadiativeB2GammaGammaDouble',
                 # Exclusive Lb -> L0 gamma
                 'Hlt2RadiativeLb2L0GammaLL',
                 'Hlt2RadiativeLb2L0GammaEELL']

        return lines

    def Thresholds(self):
        """Set thresholds for the lines."""

        from GaudiKernel.SystemOfUnits import MeV, picosecond, mm

        thresholds = {}

        ##########################################################################
        # Common thresholds
        ##########################################################################
        common = {'Common': {'TrackTisTos' : 'Hlt1(Two)?TrackMVADecision%TOS',
                             'HTOS'        : 'Hlt1(Two)?TrackMVADecision%TOS',
                             'PhotonTisTos': 'L0(Photon|Electron).*Decision%TOS',
                             'NTRACK_MAX'  : 500},
                  # Calo photons
                  'CaloPhotons': {'PT_MIN': 2000*MeV,
                                  'P_MIN' : 5000*MeV},
                  # Converted photons
                  'ConvLL': {'ee_Mass' : 50.0,
                             'ee_P'    : 5000.0,
                             'ee_PT'   : 1000.0 },
                  'ConvDD': {'ee_Mass' : 100.0,
                             'ee_P'    : 5000.0,
                             'ee_PT'   : 1000.0}}
        thresholds.update(common)

        ##########################################################################
        # Thresholds for exclusive B0 - >K* gamma and Bs - >phi gamma
        ##########################################################################
        b2xgamma = {}
        # Tracks
        for track in ['ExclusivePions', 'ExclusiveKaons']:
            b2xgamma[track] = {'TRACK_TRCHI2DOF_MAX' : 4,
                               'TRACK_PT_MIN'        : 500*MeV,
                               'TRACK_P_MIN'         : 3000*MeV,
                               'TRACK_IPCHI2_MIN'    : 20}
        b2xgamma['ExclusiveUnbiasedPIDKaons'] = {'TRACK_TYPE' : 'K+',
                                                 'PID_VAR'    : 'PIDK',
                                                 'PID_CUT_MIN': 0}
        b2xgamma['ExclusiveUnbiasedKaons'] = {'TRACK_TRCHI2DOF_MAX' : 2,
                                              'TRACK_PT_MIN'        : 600*MeV,
                                              'TRACK_P_MIN'         : 4000*MeV,
                                              'TRACK_IPCHI2_MIN'    : '-1'}
        # Vector mesons
        b2xgamma['ExclusiveKstar'] = {'PARTICLE'      : 'K*(892)0',
                                      'VCHI2PDOF_MAX' : 16,
                                      'MASS_WIN'      : 100*MeV,
                                      'PT_MIN'        : 1500*MeV}
        b2xgamma['ExclusivePhi'] = {'PARTICLE'      : 'phi(1020)',
                                    'VCHI2PDOF_MAX' : 25,
                                    'MASS_WIN'      : 20*MeV,
                                    'PT_MIN'        : 1500*MeV}
        b2xgamma['ExclusiveUnbiasedPhi'] = {'PARTICLE'      : 'phi(1020)',
                                            'VCHI2PDOF_MAX' : 9,
                                            'MASS_WIN'      : 15*MeV,
                                            'PT_MIN'        : 1500*MeV}
        # The B's
        b2xgamma['Bd2KstGamma'] = {'PARTICLE'      : 'B0',
                                   'VCHI2PDOF_MAX' : 16,
                                   'PT_MIN'        : 2000*MeV,
                                   'MASS_WIN'      : 1000*MeV,
                                   'BPVDIRA_MIN'   : 0.045,
                                   'BPVIPCHI2_MAX' : 12}
        b2xgamma['Bs2PhiGamma'] = {'PARTICLE'      : 'B_s0',
                                   'VCHI2PDOF_MAX' : 16,
                                   'PT_MIN'        : 2000*MeV,
                                   'MASS_WIN'      : 1000*MeV,
                                   'BPVDIRA_MIN'   : 0.063,
                                   'BPVIPCHI2_MAX' : 12}
        b2xgamma['Bs2PhiGammaUnbiased'] = {'PARTICLE'     : 'B_s0',
                                           'VCHI2PDOF_MAX': 9,
                                           'PT_MIN'       : 2000*MeV,
                                           'SUM_PT_MIN'   : 5000*MeV,
                                           'TAU_MIN'      : 0.3*picosecond,
                                           'MASS_WIN'     : 1000*MeV}
        b2xgamma['Bd2KstGammaULUnbiased'] = {'PARTICLE'     : 'B0',
                                             'VCHI2PDOF_MAX': 9,
                                             'PT_MIN'       : 3000*MeV,
                                             'SUM_PT_MIN'   : 5000*MeV,
                                             'TAU_MIN'      : 2*picosecond,
                                             'MASS_WIN'     : 1000*MeV}
        thresholds.update(b2xgamma)

        ##########################################################################
        # Inclusive thresholds
        ##########################################################################
        comb_cuts = {'APT_MIN'           : 2000*MeV,
                     'VCHI2PDOF_MAX'     : 1000,
                     'VDCHI2_MIN'        : 0,
                     'ETA_MIN'           : 2,
                     'ETA_MAX'           : 5,
                     'CORRM_MIN'         : 1000*MeV,
                     'CORRM_MAX'         : 10000*MeV,
                     'DIRA_MIN'          : 0}
        inclusive = {'HHCombiner': 'Topo2BodyCombos',
                     'HHGammaCombo': comb_cuts,
                     'HHGammaBBDT': {'BDT_PARAMS': 'Hlt2_RadiativeInc_2plus1_BDTParams_v2r0.txt',
                                     'BDT_VARMAP': {'ipchi2'      : "log10(BPVIPCHI2())",
                                                    'ipchi2_sum'  : "SUMTREE(BPVIPCHI2(), ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')))",
                                                    'pt_min'      : "MINTREE(PT, ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')))/MeV",
                                                    'chi2dof_max' : "MAXTREE(TRCHI2DOF, ISBASIC & HASTRACK & (ABSID=='K+'))",
                                                    'gamma_pt'    : "CHILD(2, PT)",
                                                    'm_corrected' : "BPVCORRM",
                                                    'vm_corrected': "CHILD(1, BPVCORRM)",
                                                    'fdchi2'      : "log10(BPVVDCHI2)",
                                                    'vtx_chi2'    : "log10(VFASPF(VCHI2))"},
                                     'BDT_MIN': 1.010,
                                     'CMB_VRT_MCOR_MIN': 1000*MeV,
                                     'CMB_VRT_MCOR_MAX': 11000*MeV},
                     'HHGammaComboEE': comb_cuts,
                     'HHGammaEEBBDT': {'BDT_PARAMS': 'hlt2_topo_run2_v1.bbdt',
                                       'BDT_VARMAP': {'n'      : "NINTREE((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma'))",
                                                      'mcor'   : "BPVCORRM",
                                                      'chi2'   : "VFASPF(VCHI2)",
                                                      'sumpt'  : "SUMTREE(PT, ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')), 0.0)/MeV",
                                                      'eta'    : "BPVETA",
                                                      'fdchi2' : "BPVVDCHI2",
                                                      'minpt'  : "MINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')), PT)/MeV",
                                                      'nlt16'  : "NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')) & (BPVIPCHI2() < 16))",
                                                      'ipchi2' : "BPVIPCHI2()",
                                                      'n1trk'  : "NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')) & (PT > 1*GeV) & (BPVIPCHI2() > 16))"},
                                       'BDT_MIN': 0.999,
                                       'CMB_VRT_MCOR_MIN': 1000*MeV,
                                       'CMB_VRT_MCOR_MAX': 11000*MeV},
                     'HHHCombiner': 'Topo3BodyCombos',
                     'HHHGammaCombo': comb_cuts,
                     'HHHGammaBBDT': {'BDT_PARAMS': 'Hlt2_RadiativeInc_3plus1_BDTParams_v2r0.txt',
                                      'BDT_VARMAP': {'ipchi2'      : "log10(BPVIPCHI2())",
                                                     'track_pt_min': "MINTREE(PT, ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')))/MeV",
                                                     'pt_min'      : "MINTREE(PT, ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')))/MeV",
                                                     'ipchi2_sum'  : "SUMTREE(BPVIPCHI2(), ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')))",
                                                     'gamma_pt'    : "CHILD(2, PT)",
                                                     'gamma_p'     : "CHILD(2, P)",
                                                     'm_corrected' : "BPVCORRM",
                                                     'fdchi2'      : "log10(BPVVDCHI2)",
                                                     'vtx_chi2'    : "log10(VFASPF(VCHI2))"},
                                      'BDT_MIN': 1.045,
                                      'CMB_VRT_MCOR_MIN': 1000*MeV,
                                      'CMB_VRT_MCOR_MAX': 10000*MeV},
                     'HHHGammaComboEE': comb_cuts,
                     'HHHGammaEEBBDT': {'BDT_PARAMS': 'hlt2_topo_run2_v1.bbdt',
                                        'BDT_VARMAP': {'n'      : "NINTREE((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma'))",
                                                       'mcor'   : "BPVCORRM",
                                                       'chi2'   : "VFASPF(VCHI2)",
                                                       'sumpt'  : "SUMTREE(PT, ((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')), 0.0)/MeV",
                                                       'eta'    : "BPVETA",
                                                       'fdchi2' : "BPVVDCHI2",
                                                       'minpt'  : "MINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')), PT)/MeV",
                                                       'nlt16'  : "NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')) & (BPVIPCHI2() < 16))",
                                                       'ipchi2' : "BPVIPCHI2()",
                                                       'n1trk'  : "NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')|(ABSID=='gamma')) & (PT > 1*GeV) & (BPVIPCHI2() > 16))"},
                                        'BDT_MIN': 0.9978,
                                        'CMB_VRT_MCOR_MIN': 1000*MeV,
                                        'CMB_VRT_MCOR_MAX': 10000*MeV}}
        inclusive['HHGammaCombo']['CORRM_MAX'] = 11000*MeV  # Fix 2-body corrected mass
        inclusive['HHGammaComboEE']['CORRM_MAX'] = 11000*MeV  # Fix 2-body corrected mass
        thresholds.update(inclusive)

        ##########################################################################
        # Bs -> gamma gamma
        ##########################################################################
        bs2gammagamma = {'HardCalo': {'P_MIN' : 0000.0*MeV,
                                      'PT_MIN': 2000.0*MeV},
                         'B2GammaGamma': {'BsMin'     : 3500.0*MeV,
                                          'BsMax'     : 6800.0*MeV,
                                          'SUM_PT'    : 6000.0*MeV,
                                          'B_PT'      : 2000.0*MeV,
                                          'B_P'       : 0000.0*MeV,
                                          'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_None_v2.bbdt',
                                          'BDT_MIN'   : 1.15},
                         'B2GammaGammaLL': {'BsMin'     : 4000.0*MeV,
                                            'BsMax'     : 7000.0*MeV,
                                            'SUM_PT'    : 6000.0*MeV,
                                            'B_PT'      : 2500.0*MeV,
                                            'B_P'       : 4000.0*MeV,
                                            'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_LL_v2.bbdt',
                                            'BDT_MIN'   : 1.2},
                         'B2GammaGammaDD': {'BsMin'     : 4200.0*MeV,
                                            'BsMax'     : 7000.0*MeV,
                                            'SUM_PT'    : 4000.0*MeV,
                                            'B_PT'      : 2500.0*MeV,
                                            'B_P'       : 4000.0*MeV,
                                            'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_DD_v2.bbdt',
                                            'BDT_MIN'   : 1.26},
                         'B2GammaGammaDouble': {'BsMin'     : 4500.0*MeV,
                                                'BsMax'     : 6400.0*MeV,
                                                'SUM_PT'    : 5000.0*MeV,
                                                'B_PT'      : 2500.0*MeV,
                                                'B_P'       : 5000.0*MeV,
                                                'B_VTX'     : 9.0,
                                                'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_Double_v2.bbdt',
                                                'BDT_MIN'   : 1.18
                                                }}
        thresholds.update(bs2gammagamma)

        ##########################################################################
        # Lb -> L0 gamma
        ##########################################################################
        lb2l0gamma = {'Lambda0LL': {'DOCA_MAX'           : 0.2*mm,
                                    'TRACK_P_MIN'        : 2000*MeV,
                                    'TRACK_PT_MIN'       : 250*MeV,
                                    'TRACK_IPCHI2_MIN'   : 36,
                                    'TRACK_TRCHI2DOF_MAX': 3,
                                    'VCHI2PDOF_MAX'      : 15,
                                    'P_PIDP_MIN'         : 0,
                                    'VDCHI2_MIN'         : 0,
                                    'PT_MIN'             : 1500*MeV,
                                    'IP_MIN'             : 0.1*mm},
                      'Lb2L0GammaLL': {'SUM_PT_MIN'    : 5000*MeV,
                                       'PT_MIN'        : 1000*MeV,
                                       'MASS_WIN'      : 1000*MeV,
                                       'MTDOCACHI2_MAX': 9},
                      'Lb2L0GammaEELL': {'PARTICLE'     : 'Lambda_b0',
                                         'VCHI2PDOF_MAX': 12,
                                         'PT_MIN'       : 1000*MeV,
                                         'MASS_WIN'     : 1000*MeV,
                                         'BPVDIRA_MIN'  : 0.05,
                                         'BPVIPCHI2_MAX': 12}}
        thresholds.update(lb2l0gamma)

        ##########################################################################
        # Prescales
        ##########################################################################
        prescales = {'Prescale': {'Hlt2RadiativeB2GammaGamma'  : 1.0,
                                  'Hlt2RadiativeB2GammaGammaDD': 1.0}}
        thresholds.update(prescales)

        ##########################################################################
        # Return
        ##########################################################################
        from Hlt2Lines.Radiative.Lines import RadiativeLines

        return {RadiativeLines: thresholds}

# EOF

