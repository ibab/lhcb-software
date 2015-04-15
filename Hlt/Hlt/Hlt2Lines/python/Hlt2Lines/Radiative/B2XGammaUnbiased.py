#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   B2XGammaUnbiased.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Creation of exclusive HLT2 radiative lines."""

from RadiativeLineBuilder import RadiativeLineBuilder


class B2XGammaUnbiasedLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages(_):
        from Stages import TrackGEC, PIDFilter, ParticleFilter, HHCombiner, B2XGammaUnbiasedCombiner
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2LoosePions, Hlt2LooseKaons

        # Create vector mesons
        phi = HHCombiner('UnbiasedPhi', 'phi(1020) -> K+ K-',
                         [ParticleFilter('UnbiasedKaons',
                                         [PIDFilter('UnbiasedPIDKaons',
                                                    [Hlt2LooseKaons])])])

        # Build Bs -> phi gamma
        bs2phigamma = B2XGammaUnbiasedCombiner('Bs2PhiGammaUnbiased',
                                               'B_s0 -> phi(1020) gamma',
                                               phi)
        bs2phigammaee = B2XGammaUnbiasedCombiner('Bs2PhiGammaEEUnbiased',
                                                 'B_s0 -> phi(1020) gamma',
                                                 phi,
                                                 converted=True)

        # Build stages
        return {'Bs2PhiGammaUnbiased'  : [TrackGEC(), PV3D('Hlt2'), bs2phigamma],
                'Bs2PhiGammaEEUnbiased': [TrackGEC(), PV3D('Hlt2'), bs2phigammaee]}

    @staticmethod
    def get_cuts():
        from GaudiKernel.SystemOfUnits import MeV, picosecond
        cuts = {}
        # Track cuts
        cuts['UnbiasedPIDKaons'] = {'TRACK_TYPE' : 'K+',
                                    'PID_VAR'    : 'PIDK',
                                    'PID_CUT_MIN': 0}

        cuts['UnbiasedKaons'] = {'TRACK_TRCHI2DOF_MAX' : 2,
                                 'TRACK_PT_MIN'        : 600*MeV,
                                 'TRACK_P_MIN'         : 4000*MeV,
                                 'TRACK_IPCHI2_MIN'    : '-1'}
        # Vector mesons
        cuts['UnbiasedPhi'] = {'PARTICLE'      : 'phi(1020)',
                               'VCHI2PDOF_MAX' : 9,
                               'MASS_WIN'      : 15*MeV}
        # The B mesons
        for line in ['Bs2PhiGammaUnbiased', 'Bs2PhiGammaEEUnbiased']:
            cuts[line] = {'PARTICLE'     : 'B_s0',
                          'VCHI2PDOF_MAX': 9,
                          'PT_MIN'       : 2000*MeV,
                          'TAU_MIN'      : 0.3*picosecond,
                          'MASS_WIN'     : 1000*MeV}
        cuts['Bs2PhiGammaUnbiased']['SUM_PT_MIN'] = 5000*MeV
        cuts['Bs2PhiGammaEEUnbiased']['SUM_PT_MIN'] = 3000*MeV
        return cuts

    @staticmethod
    def get_hlt1():
        return {'Bs2PhiGammaUnbiased': "HLT_PASS_RE('Hlt1B2PhiGamma_LTUNBDecision')"}

    @staticmethod
    def get_l0():
        return {'Bs2PhiGammaUnbiased': "L0_CHANNEL('Electron') | L0_CHANNEL('Photon')"}

# EOF

