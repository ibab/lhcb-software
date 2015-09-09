#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   B2XGamma.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Creation of exclusive HLT2 radiative lines."""

from RadiativeLineBuilder import RadiativeLineBuilder


class B2XGammaLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages(_):
        from Stages import TrackGEC
        from Stages import PIDFilter, ParticleFilter, HHCombiner
        from Stages import B2XGammaCombiner, B2XGammaUnbiasedCombiner
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2LoosePions, Hlt2LooseKaons

        # Create vector mesons
        kstar = HHCombiner('ExclusiveKstar', '[K*(892)0 -> K+ pi-]cc',
                           [ParticleFilter('ExclusivePions', [Hlt2LoosePions]),
                            ParticleFilter('ExclusiveKaons', [Hlt2LooseKaons])],
                           'TrackTisTos')
        phi = HHCombiner('ExclusivePhi', 'phi(1020) -> K+ K-',
                         [ParticleFilter('ExclusiveKaons', [Hlt2LooseKaons])],
                         'TrackTisTos')

        unbiased_phi = HHCombiner('ExclusiveUnbiasedPhi', 'phi(1020) -> K+ K-',
                                  [ParticleFilter('ExclusiveUnbiasedKaons',
                                                  [PIDFilter('ExclusiveUnbiasedPIDKaons',
                                                             [Hlt2LooseKaons])])])
        # Build B0 -> K* gamma
        bd2kstgamma = B2XGammaCombiner('Bd2KstGamma',
                                       '[B0 -> K*(892)0 gamma]cc',
                                       kstar)
        # Build Bs -> phi gamma

        bs2phigamma = B2XGammaCombiner('Bs2PhiGamma',
                                       'B_s0 -> phi(1020) gamma',
                                       phi)

        # Build unbiased lines
        bs2phigamma_unbiased = B2XGammaUnbiasedCombiner('Bs2PhiGammaUnbiased',
                                                        'B_s0 -> phi(1020) gamma',
                                                        unbiased_phi)
        bd2kstgamma_unbiased = B2XGammaUnbiasedCombiner('Bd2KstGammaULUnbiased',
                                                        '[B0 -> K*(892)0 gamma]cc',
                                                        kstar)
        # Build stages
        return {'RadiativeBd2KstGamma'           : [TrackGEC(), PV3D('Hlt2'), bd2kstgamma],
                'RadiativeBs2PhiGamma'           : [TrackGEC(), PV3D('Hlt2'), bs2phigamma],
                'RadiativeBs2PhiGammaUnbiased'   : [TrackGEC(), PV3D('Hlt2'), bs2phigamma_unbiased],
                'RadiativeBd2KstGammaULUnbiased' : [TrackGEC(), PV3D('Hlt2'), bd2kstgamma_unbiased]}

    @staticmethod
    def get_cuts():
        from GaudiKernel.SystemOfUnits import MeV, picosecond
        cuts = {}
        # Track cuts
        for track in ['ExclusivePions', 'ExclusiveKaons']:
            cuts[track] = {'TRACK_TRCHI2DOF_MAX' : 4,
                           'TRACK_PT_MIN'        : 500*MeV,
                           'TRACK_P_MIN'         : 3000*MeV,
                           'TRACK_IPCHI2_MIN'    : 20}
        cuts['ExclusiveUnbiasedPIDKaons'] = {'TRACK_TYPE' : 'K+',
                                             'PID_VAR'    : 'PIDK',
                                             'PID_CUT_MIN': 0}

        cuts['ExclusiveUnbiasedKaons'] = {'TRACK_TRCHI2DOF_MAX' : 2,
                                          'TRACK_PT_MIN'        : 600*MeV,
                                          'TRACK_P_MIN'         : 4000*MeV,
                                          'TRACK_IPCHI2_MIN'    : '-1'}
        # Vector mesons
        cuts['ExclusiveKstar'] = {'PARTICLE'      : 'K*(892)0',
                                  'VCHI2PDOF_MAX' : 16,
                                  'MASS_WIN'      : 100*MeV,
                                  'PT_MIN'        : 1500*MeV}
        cuts['ExclusivePhi'] = {'PARTICLE'      : 'phi(1020)',
                                'VCHI2PDOF_MAX' : 25,
                                'MASS_WIN'      : 20*MeV,
                                'PT_MIN'        : 1500*MeV}
        # Vector mesons
        cuts['ExclusiveUnbiasedPhi'] = {'PARTICLE'      : 'phi(1020)',
                                        'VCHI2PDOF_MAX' : 9,
                                        'MASS_WIN'      : 15*MeV,
                                        'PT_MIN'        : 1500*MeV}
        # The B mesons
        for line in ['Bd2KstGamma', 'Bs2PhiGamma']:
            if line.startswith('Bd'):
                particle = 'B0'
                dira = 0.045
            else:
                particle = 'B_s0'
                dira = 0.063
            cuts[line] = {'PARTICLE'      : particle,
                          'VCHI2PDOF_MAX' : 16,
                          'PT_MIN'        : 2000*MeV,
                          'MASS_WIN'      : 1000*MeV,
                          'BPVDIRA_MIN'   : dira,
                          'BPVIPCHI2_MAX' : 12}
        cuts['Bs2PhiGammaUnbiased'] = {'PARTICLE'     : 'B_s0',
                                       'VCHI2PDOF_MAX': 9,
                                       'PT_MIN'       : 2000*MeV,
                                       'SUM_PT_MIN'   : 5000*MeV,
                                       'TAU_MIN'      : 0.3*picosecond,
                                       'MASS_WIN'     : 1000*MeV}
        cuts['Bd2KstGammaULUnbiased'] = {'PARTICLE'     : 'B0',
                                         'VCHI2PDOF_MAX': 9,
                                         'PT_MIN'       : 2000*MeV,
                                         'SUM_PT_MIN'   : 5000*MeV,
                                         'TAU_MIN'      : 2*picosecond,
                                         'MASS_WIN'     : 1000*MeV}
        return cuts

    @staticmethod
    def get_hlt1():
        hlt1 = {}
        for line in ['RadiativeBd2KstGamma', 'RadiativeBs2PhiGamma', 'RadiativeBd2KstGammaULUnbiased']:
            hlt1[line] = "HLT_PASS_RE('Hlt1(Two)?TrackMVADecision')"
        hlt1['RadiativeBs2PhiGammaUnbiased'] = "HLT_PASS_RE('Hlt1B2PhiGamma_LTUNBDecision')"
        return hlt1

    @staticmethod
    def get_l0():
        l0 = {}
        for line in ['RadiativeBd2KstGamma', 'RadiativeBs2PhiGamma', 'RadiativeBs2PhiGammaUnbiased', 'Bd2KstGammaULUnbiased']:
            l0[line] = "L0_CHANNEL('Electron') | L0_CHANNEL('Photon')"
        return l0

# EOF
