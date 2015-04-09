#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   GammaGamma.py
# @author Sean Benson (sean.benson@cern.ch)
# @date   12.03.2015
# =============================================================================
"""Bs -> Gamma Gamma lines."""

from RadiativeLineBuilder import RadiativeLineBuilder


class B2GammaGammaLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages():
        from Stages import TrackGEC
        from Stages import CALOPhotonFilter, ConvPhotonFilter
        from Stages import PhotonFilter, ConvPhotonLL, ConvPhotonDD
        from Stages import B2GammaGammaCombiner
        from HltTracking.HltPVs import PV3D

        # Make hard non-converted photons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons     as Hlt2Photons
        HardCALOGamma = CALOPhotonFilter('HardCALO',[Hlt2Photons])

        # Build Bs -> gamma gamma (double conversion)
        bs2gammagammaDouble = B2GammaGammaCombiner('B2GammaGammaDouble',
                                                   'B_s0 -> gamma gamma',
                                                   [ConvPhotonLL(), ConvPhotonDD()])
        # Build Bs -> gamma gamma (single conversion)
        bs2gammagammaLL = B2GammaGammaCombiner('B2GammaGammaLL',
                                               'B_s0 -> gamma gamma',
                                               [HardCALOGamma,ConvPhotonLL()])
        bs2gammagammaDD = B2GammaGammaCombiner('B2GammaGammaDD',
                                               'B_s0 -> gamma gamma',
                                               [HardCALOGamma,ConvPhotonDD()])
        # Build Bs -> gamma gamma (all calo)
        bs2gammagamma = B2GammaGammaCombiner('B2GammaGamma',
                                             'B_s0 -> gamma gamma',
                                             [HardCALOGamma])

        return {'B2GammaGammaLL'     : [TrackGEC(), PV3D('Hlt2'), bs2gammagammaLL],
                'B2GammaGammaDD'     : [TrackGEC(), PV3D('Hlt2'), bs2gammagammaDD],
                'B2GammaGammaDouble' : [TrackGEC(), PV3D('Hlt2'), bs2gammagammaDouble],
                'B2GammaGamma'       : [TrackGEC(), PV3D('Hlt2'), bs2gammagamma]}

    @staticmethod
    def get_cuts():
        cuts = {}
        # Hard CALO photons
        cuts['HardCALO'] = {
                'CALO_P'    : 5000.0,
                'CALO_PT'   : 3000.0
                }
        # Bs
        cuts['B2GammaGamma'] = {
                'BsMin'     : 4300.0,
                'BsMax'     : 6800.0,
                'SUM_PT'    : 6000.0,
                'B_PT'      : 3000.0,
                'B_P'       : 5000.0,
                }
        cuts['B2GammaGammaLL'] = {
                'BsMin'     : 4000.0,
                'BsMax'     : 7000.0,
                'SUM_PT'    : 6000.0,
                'B_PT'      : 2500.0,
                'B_P'       : 4000.0,
                }
        cuts['B2GammaGammaDD'] = {
                'BsMin'     : 4200.0,
                'BsMax'     : 7000.0,
                'SUM_PT'    : 4000.0,
                'B_PT'      : 2500.0,
                'B_P'       : 4000.0,
                }
        cuts['B2GammaGammaDouble'] = {
                'BsMin'     : 4000.0,
                'BsMax'     : 7000.0,
                'SUM_PT'    : 2000.0,
                'B_PT'      : 1000.0,
                'B_P'       : 5000.0,
                }
        return cuts

    @staticmethod
    def get_hlt1():
        return {'B2GammaGamma': "HLT_PASS_RE('Hlt1B2GammaGammaDecision')"}

# EOF



