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
    def get_stages():
        from Stages import TrackGEC, LambdaL0Filter, Lb2L0GammaCombiner
        from HltTracking.HltPVs import PV3D
        from Inputs import LambdaLL  #, LambdaDD

        lambda0_ll = LambdaL0Filter("Lambda0LL", [LambdaLL])
        # lambda0_dd = LambdaFilter("Lambda0LL", [LambdaDD])

        # Build stages
        return {'Lb2L0GammaLL' : [TrackGEC(),
                                  PV3D('Hlt2'),
                                  Lb2L0GammaCombiner('LL',
                                                     '[Lambda_b0 -> Lambda0 gamma]cc',
                                                     lambda0_ll)]}

    @staticmethod
    def get_cuts():
        cuts = {'TRACK_DOCA_MAX'     : '0.2*mm',
                'TRACK_P_MIN'        : '2.0*GeV',
                'TRACK_IPCHI2_MIN'   : '9',
                'L0_PT_MIN'          : '1000*MeV',
                'L0_IP_MIN'          : '0.05*mm',
                'LB0_SUM_PT'         : '5000*MeV',
                'LB0_PT'             : '1000*MeV',
                'LB0_MASS_WINDOW'    : '1500*MeV',
                'LB0_MTDOCACHI2_MAX' : '10'}
        return {'Lb2L0GammaLL': cuts}

# EOF