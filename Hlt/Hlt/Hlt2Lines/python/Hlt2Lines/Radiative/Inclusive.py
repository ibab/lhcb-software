#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Inclusive.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   14.04.2015
# =============================================================================
"""Inclusive B->hhgamma and B->hhhgamma lines."""


from RadiativeLineBuilder import RadiativeLineBuilder


class RadiativeInclusiveLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages(props):
        from Stages import TrackGEC, TopoCombiner, BonsaiBDTFilter
        from HltTracking.HltPVs import PV3D
        from Hlt2Lines.Topo.Lines import TopoLines
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage

        # Load external stages
        topo_lines = TopoLines()
        two_body = Hlt2ExternalStage(topo_lines, topo_lines.stages('Topo2BodyCombos')[0])
        three_body = Hlt2ExternalStage(topo_lines, topo_lines.stages('Topo3BodyCombos')[0])

        # Add the photon
        two_body_plus_photon = TopoCombiner('HHGammaCombo', 2, two_body)
        three_body_plus_photon = TopoCombiner('HHHGammaCombo', 3, three_body)
        two_body_plus_photon_ee = TopoCombiner('HHGammaEECombo', 2, two_body, True)
        three_body_plus_photon_ee = TopoCombiner('HHHGammaEECombo', 3, three_body, True)
        # Filter BDT
        filtered_two_plus_one = BonsaiBDTFilter('HHGammaBBDT',
                                                [two_body_plus_photon],
                                                props['HHGammaBBDT'])
        filtered_three_plus_one = BonsaiBDTFilter('HHHGammaBBDT',
                                                  [three_body_plus_photon],
                                                  props['HHHGammaBBDT'])
        filtered_two_plus_one_ee = BonsaiBDTFilter('HHGammaEEBBDT',
                                                   [two_body_plus_photon_ee],
                                                   props['HHGammaEEBBDT'])
        filtered_three_plus_one_ee = BonsaiBDTFilter('HHHGammaEEBBDT',
                                                     [three_body_plus_photon_ee],
                                                     props['HHHGammaEEBBDT'])
        # Build stages
        return {'RadiativeIncHHGamma' : [TrackGEC(), PV3D('Hlt2'), filtered_two_plus_one],
                'RadiativeIncHHHGamma': [TrackGEC(), PV3D('Hlt2'), filtered_three_plus_one],
                'RadiativeIncHHGammaEE' : [TrackGEC(), PV3D('Hlt2'), filtered_two_plus_one_ee],
                'RadiativeIncHHHGammaEE': [TrackGEC(), PV3D('Hlt2'), filtered_three_plus_one_ee]}


    @staticmethod
    def get_cuts():
        from GaudiKernel.SystemOfUnits import MeV
        comb_cuts = {'APT_MIN'           : 2000*MeV,
                     'VCHI2PDOF_MAX'     : 1000,
                     'VDCHI2_MIN'        : 0,
                     'ETA_MIN'           : 2,
                     'ETA_MAX'           : 5,
                     'CORRM_MIN'         : 1000*MeV,
                     'CORRM_MAX'         : 10000*MeV,
                     'DIRA_MIN'          : 0}
        return {'HHGammaCombo': comb_cuts,
                'HHGammaBBDT': {'BBDT_PARAMS': 'file_name',
                                'BBDT_VARMAP': {},
                                'BBDT_CUT': 0.0},
                'HHHGammaCombo': comb_cuts,
                'HHHGammaBBDT': {'BBDT_PARAMS': 'file_name',
                                 'BBDT_VARMAP': {},
                                 'BBDT_CUT': 0.0},
                'HHGammaEECombo': comb_cuts,
                'HHGammaEEBBDT': {'BBDT_PARAMS': 'file_name',
                                  'BBDT_VARMAP': {},
                                  'BBDT_CUT': 0.0},
                'HHHGammaEECombo': comb_cuts,
                'HHHGammaEEBBDT': {'BBDT_PARAMS': 'file_name',
                                   'BBDT_VARMAP': {},
                                   'BBDT_CUT': 0.0}}


    @staticmethod
    def get_hlt1():
        hlt1 = {}
        for line in ['RadiativeIncHHGamma', 'RadiativeIncHHHGamma', 'RadiativeIncHHGammaEE', 'RadiativeIncHHHGammaEE']:
            hlt1[line] = "HLT_PASS_RE('Hlt1(Two)?TrackMVADecision')"
        return hlt1

    @staticmethod
    def get_l0():
        l0 = {}
        for line in ['RadiativeIncHHGamma', 'RadiativeIncHHHGamma']:
            l0[line] = "(L0_CHANNEL('Electron') | L0_CHANNEL('Photon'))"
        return l0

# EOF
