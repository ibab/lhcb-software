#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Lb2L0Gamma.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   08.04.2015
# =============================================================================
"""Creation of the Lb -> L0 gamma lines."""

from RadiativeLineBuilder import RadiativeLineBuilder


class Lb2L0GammaLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages(_):
        from Stages import TrackGEC, Lambda0Filter, Lb2L0GammaCombiner, B2XGammaCombiner
        from HltTracking.HltPVs import PV3D
        from Inputs import LambdaLL  #, LambdaDD

        lambda0_ll = Lambda0Filter("Lambda0LL", [LambdaLL])
        # lambda0_dd = LambdaFilter("Lambda0LL", [LambdaDD])

        # Build stages
        stages = {'RadiativeLb2L0GammaLL': [TrackGEC(),
                                            PV3D('Hlt2'),
                                            Lb2L0GammaCombiner('Lb2L0GammaLL',
                                                               '[Lambda_b0 -> Lambda0 gamma]cc',
                                                               lambda0_ll)],
                  'RadiativeLb2L0GammaEELL': [TrackGEC(),
                                              PV3D('Hlt2'),
                                              B2XGammaCombiner('Lb2L0GammaEELL',
                                                               '[Lambda_b0 -> Lambda0 gamma]cc',
                                                               lambda0_ll,
                                                               True)]}
        return stages

    @staticmethod
    def get_cuts():
        from GaudiKernel.SystemOfUnits import MeV, mm
        cuts = {'Lambda0LL': {'DOCA_MAX'           : 0.2*mm,
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
                                   'VCHI2PDOF_MAX': 25,
                                   'PT_MIN'       : 1000*MeV,
                                   'MASS_WIN'     : 1000*MeV,
                                   'BPVDIRA_MIN'  : 0.1,
                                   'BPVIPCHI2_MAX': 16 }}
        return cuts

    @staticmethod
    def get_l0():
        return {'RadiativeLb2L0GammaLL': "(L0_CHANNEL('Electron') | L0_CHANNEL('Photon'))"}

# EOF
