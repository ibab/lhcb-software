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
    def get_stages(props):
        from Stages import TrackGEC
        from Stages import PhotonFilter
        from Stages import ConvPhotonLL, ConvPhotonDD, ConvPhotonAll
        from Stages import B2GammaGammaCombiner, FilterBDTGammaGamma
        from HltTracking.HltPVs import PV3D

        # Make hard non-converted photons
        from Inputs import Hlt2Photons
        HardCALOGamma = PhotonFilter('HardCalo')

        # Build Bs -> gamma gamma (double conversion)
        bs2gammagammaDouble = B2GammaGammaCombiner('B2GammaGammaDouble',
                                                   'B_s0 -> gamma gamma',
                                                   [ConvPhotonAll()])
        BDTFilter_Double = FilterBDTGammaGamma('Double', [bs2gammagammaDouble], props['B2GammaGammaDouble'])

        # Build Bs -> gamma gamma (single conversion)
        bs2gammagammaLL = B2GammaGammaCombiner('B2GammaGammaLL',
                                               'B_s0 -> gamma gamma',
                                               [HardCALOGamma, ConvPhotonLL()])
        BDTFilter_LL = FilterBDTGammaGamma('LL', [bs2gammagammaLL], props['B2GammaGammaLL'])

        bs2gammagammaDD = B2GammaGammaCombiner('B2GammaGammaDD',
                                               'B_s0 -> gamma gamma',
                                               [HardCALOGamma, ConvPhotonDD()])
        BDTFilter_DD = FilterBDTGammaGamma('DD', [bs2gammagammaDD], props['B2GammaGammaDD'])

        # Build Bs -> gamma gamma (all calo)
        bs2gammagamma = B2GammaGammaCombiner('B2GammaGamma',
                                             'B_s0 -> gamma gamma',
                                             [HardCALOGamma])
        BDTFilter_None = FilterBDTGammaGamma('None', [bs2gammagamma], props['B2GammaGamma'])

        return {'RadiativeB2GammaGammaLL'     : [TrackGEC(), PV3D('Hlt2'), BDTFilter_LL],
                'RadiativeB2GammaGammaDD'     : [TrackGEC(), PV3D('Hlt2'), BDTFilter_DD],
                'RadiativeB2GammaGammaDouble' : [TrackGEC(), PV3D('Hlt2'), BDTFilter_Double],
                'RadiativeB2GammaGamma'       : [TrackGEC(), PV3D('Hlt2'), BDTFilter_None]}

    @staticmethod
    def get_cuts():
        from GaudiKernel.SystemOfUnits import MeV
        cuts = {}
        # Hard CALO photons
        cuts['HardCalo'] = {
                'P_MIN'    : 5000.0*MeV,
                'PT_MIN'   : 3000.0*MeV
                }
        # Bs
        cuts['B2GammaGamma'] = {
                'BsMin'     : 4300.0*MeV,
                'BsMax'     : 6800.0*MeV,
                'SUM_PT'    : 6000.0*MeV,
                'B_PT'      : 3000.0*MeV,
                'B_P'       : 5000.0*MeV,
                'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_None_v2.bbdt',
                'BDT_MIN'   : 1.0
                }
        cuts['B2GammaGammaLL'] = {
                'BsMin'     : 4000.0*MeV,
                'BsMax'     : 7000.0*MeV,
                'SUM_PT'    : 6000.0*MeV,
                'B_PT'      : 2500.0*MeV,
                'B_P'       : 4000.0*MeV,
                'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_LL_v2.bbdt',
                'BDT_MIN'   : 1.0
                }
        cuts['B2GammaGammaDD'] = {
                'BsMin'     : 4200.0*MeV,
                'BsMax'     : 7000.0*MeV,
                'SUM_PT'    : 4000.0*MeV,
                'B_PT'      : 2500.0*MeV,
                'B_P'       : 4000.0*MeV,
                'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_DD_v2.bbdt',
                'BDT_MIN'   : 1.0
                }
        cuts['B2GammaGammaDouble'] = {
                'BsMin'     : 4000.0*MeV,
                'BsMax'     : 7000.0*MeV,
                'SUM_PT'    : 2000.0*MeV,
                'B_PT'      : 1000.0*MeV,
                'B_P'       : 5000.0*MeV,
                'B_VTX'     : 25.0,
                'BDT_PARAMS': '$PARAMFILESROOT/data/Hlt2B2GammaGamma_Double_v2.bbdt',
                'BDT_MIN'   : 1.0,
                }
        return cuts

    @staticmethod
    def get_hlt1():
        return {'RadiativeB2GammaGamma': "HLT_PASS_RE('Hlt1B2GammaGammaDecision')"}

# EOF

