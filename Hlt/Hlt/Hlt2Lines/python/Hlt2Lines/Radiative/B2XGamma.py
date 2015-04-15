#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Exclusive.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Creation of exclusive HLT2 radiative lines."""

from RadiativeLineBuilder import RadiativeLineBuilder


class B2XGammaLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages(_):
        from Stages import TrackGEC, ParticleFilter, MassWindowFilter, HHCombiner, B2XGammaCombiner
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2LoosePions, Hlt2LooseKaons

        # Create vector mesons
        kstar = HHCombiner('Kstar', '[K*(892)0 -> K+ pi-]cc',
                           [ParticleFilter('Pions', [Hlt2LoosePions]),
                            ParticleFilter('Kaons', [Hlt2LooseKaons])],
                           'TrackTisTos')
        kstar_tight = MassWindowFilter('KstarTight', [kstar])
        phi = HHCombiner('Phi', 'phi(1020) -> K+ K-',
                         [ParticleFilter('Kaons', [Hlt2LooseKaons])],
                         'TrackTisTos')

        # Build B0 -> K* gamma
        bd2kstgamma_wideKst = B2XGammaCombiner('Bd2KstGammaWideKMass',
                                               '[B0 -> K*(892)0 gamma]cc',
                                               kstar)
        bd2kstgamma_wideB = B2XGammaCombiner('Bd2KstGammaWideBMass',
                                             '[B0 -> K*(892)0 gamma]cc',
                                             kstar_tight)
        bd2kstgamma = MassWindowFilter('Bd2KstGamma', [bd2kstgamma_wideB])
        bd2kstgammaee = B2XGammaCombiner('Bd2KstGammaEE',
                                         '[B0 -> K*(892)0 gamma]cc',
                                         kstar_tight,
                                         converted=True)
        # Build Bs -> phi gamma

        bs2phigamma_wideB = B2XGammaCombiner('Bs2PhiGammaWideBMass',
                                             'B_s0 -> phi(1020) gamma',
                                             phi)
        bs2phigamma = MassWindowFilter('Bs2PhiGamma', [bs2phigamma_wideB])
        bs2phigammaee = B2XGammaCombiner('Bs2PhiGammaEE',
                                         'B_s0 -> phi(1020) gamma',
                                         phi,
                                         converted=True)

        # Build stages
        return {'Bd2KstGammaWideKMass' : [TrackGEC(), PV3D('Hlt2'), bd2kstgamma_wideKst],
                'Bd2KstGammaWideBMass' : [TrackGEC(), PV3D('Hlt2'), bd2kstgamma_wideB],
                'Bd2KstGamma'          : [TrackGEC(), PV3D('Hlt2'), bd2kstgamma],
                'Bd2KstGammaEE'        : [TrackGEC(), PV3D('Hlt2'), bd2kstgammaee],
                'Bs2PhiGammaWideBMass' : [TrackGEC(), PV3D('Hlt2'), bs2phigamma_wideB],
                'Bs2PhiGamma'          : [TrackGEC(), PV3D('Hlt2'), bs2phigamma],
                'Bs2PhiGammaEE'        : [TrackGEC(), PV3D('Hlt2'), bs2phigammaee]}

    @staticmethod
    def get_cuts():
        from GaudiKernel.SystemOfUnits import MeV
        cuts = {}
        # Track cuts
        for track in ['Pions', 'Kaons']:
            cuts[track] = {'TRACK_TRCHI2DOF_MAX' : 4,
                           'TRACK_PT_MIN'        : 500*MeV,
                           'TRACK_P_MIN'         : 3000*MeV,
                           'TRACK_IPCHI2_MIN'    : 20}
        # Vector mesons
        cuts['Kstar'] = {'PARTICLE'      : 'K*(892)0',
                         'VCHI2PDOF_MAX' : 16,
                         'MASS_WIN'      : 125*MeV}
        cuts['Phi'] = {'PARTICLE'      : 'phi(1020)',
                       'VCHI2PDOF_MAX' : 25,
                       'MASS_WIN'      : 20*MeV}
        cuts['KstarTight'] = {'PARTICLE' : 'K*(892)0',
                              'MASS_WIN' : 100*MeV}
        # The B mesons
        for line in ['Bd2KstGammaWideKMass', 'Bd2KstGammaWideBMass', 'Bs2PhiGammaWideBMass',
                     'Bd2KstGammaEE', 'Bs2PhiGammaEE']:
            if line.startswith('Bd'):
                particle = 'B0'
                dira = 0.045
            else:
                particle = 'B_s0'
                dira = 0.063
            cuts[line] = {'PARTICLE'      : particle,
                          'VCHI2PDOF_MAX' : 16,
                          'PT_MIN'        : 2000*MeV,
                          'MASS_WIN'      : 1500*MeV,
                          'BPVDIRA_MIN'   : dira,
                          'BPVIPCHI2_MAX' : 12}
        cuts['Bd2KstGammaWideKMass']['MASS_WIN'] = 1000*MeV
        cuts['Bd2KstGamma'] = {'PARTICLE': 'B0',
                               'MASS_WIN': 1000*MeV}
        cuts['Bd2KstGammaEE']['MASS_WIN'] = 1000*MeV
        cuts['Bs2PhiGamma'] = {'PARTICLE': 'B_s0',
                               'MASS_WIN': 1000*MeV}
        cuts['Bs2PhiGammaEE']['MASS_WIN'] = 1000*MeV
        return cuts

    @staticmethod
    def get_hlt1():
        hlt1 = {}
        for line in ['Bd2KstGammaWideKMass', 'Bd2KstGammaWideBMass', 'Bs2PhiGammaWideBMass',
                     'Bd2KstGamma', 'Bs2PhiGamma', 'Bd2KstGammaEE', 'Bs2PhiGammaEE']:
            hlt1[line] = "HLT_PASS_RE('Hlt1(Two)?TrackMVADecision')"
        return hlt1

    @staticmethod
    def get_l0():
        l0 = {}
        for line in ['Bd2KstGammaWideKMass', 'Bd2KstGammaWideBMass', 'Bs2PhiGammaWideBMass',
                     'Bd2KstGamma', 'Bs2PhiGamma']:
            l0[line] = "L0_CHANNEL('Electron') | L0_CHANNEL('Photon')"
        return l0

# EOF
