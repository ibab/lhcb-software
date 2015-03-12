#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   GammaGamma.py
# @author Sean Benson (sean.benson@cern.ch)
# @date   12.03.2015
# =============================================================================
"""Bs -> Gamma Gamma lines."""


class B2GammaGammaLines(object):
    @staticmethod
    def get_stages():
        from Stages import TrackGEC
        from Stages import ConvPhotonFilter
        from Stages import PhotonFilter, ConvPhotonLL, ConvPhotonDD
        from Stages import B2GammaGammaCombiner
        from HltTracking.HltPVs import PV3D

        # Build Bs -> gamma gamma (double conversion)
        bs2gammagammaDouble = B2GammaGammaCombiner('B2GammaGammaDouble',
                                                   'B_s0 -> gamma gamma',
                                                   [ConvPhotonLL(), ConvPhotonDD()])
        # Build Bs -> gamma gamma (single conversion)
        bs2gammagammaLL = B2GammaGammaCombiner('B2GammaGammaLL',
                                               'B_s0 -> gamma gamma',
                                               [ConvPhotonLL()])
        bs2gammagammaDD = B2GammaGammaCombiner('B2GammaGammaDD',
                                               'B_s0 -> gamma gamma',
                                               [ConvPhotonDD()])
        # Build Bs -> gamma gamma (all calo)
        bs2gammagamma = B2GammaGammaCombiner('B2GammaGamma',
                                             'B_s0 -> gamma gamma',
                                             [PhotonFilter()])

        return {'B2GammaGammaLL'     : [TrackGEC(), PV3D('Hlt2'), bs2gammagammaLL],
                'B2GammaGammaDD'     : [TrackGEC(), PV3D('Hlt2'), bs2gammagammaDD],
                'B2GammaGammaDouble' : [TrackGEC(), PV3D('Hlt2'), bs2gammagammaDouble],
                'B2GammaGamma'       : [TrackGEC(), PV3D('Hlt2'), bs2gammagamma]}

    @staticmethod
    def get_cuts():
        cuts = {}
        # Converted photons
        for converted in ['ConvLL', 'ConvDD']:
            cuts[converted] = {'ee_Mass' : 40,
                               'ee_P'    : 2,
                               'ee_PT'   : 200}
        # Bs
        for line in ['B2GammaGamma', 'B2GammaGammaLL', 'B2GammaGammaDD', 'B2GammaGammaDouble']:
            cuts[line] = {'BsMassWin' : 2200,
                          'B_PT'      : 1000,
                          'B_P'       : 3000}
        # Special case
        cuts['B2GammaGammaDouble']['BsMassWin'] = 1500
        return cuts

# EOF



