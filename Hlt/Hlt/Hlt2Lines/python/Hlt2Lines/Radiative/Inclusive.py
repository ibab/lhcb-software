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
        from Stages import TrackGEC, TopoCombiner
        from HltTracking.HltPVs import PV3D
        from Hlt2Lines.Topo.Lines import TopoLines
        from Hlt2Lines.Topo.Stages import FilterMVA
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage

        # Load external stages
        topo_lines = TopoLines()
        two_body = Hlt2ExternalStage(topo_lines, topo_lines.stages(props['HHCombiner'])[0])
        three_body = Hlt2ExternalStage(topo_lines, topo_lines.stages(props['HHHCombiner'])[0])

        # Add the photon
        two_body_plus_photon = TopoCombiner('HHGammaCombo', 2, two_body)
        two_body_plus_ee = TopoCombiner('HHGammaComboEE', 2, two_body, True)
        three_body_plus_photon = TopoCombiner('HHHGammaCombo', 3, three_body)
        three_body_plus_ee = TopoCombiner('HHHGammaComboEE', 3, three_body, True)

        # Filter BDTs
        filtered_two_plus_one = FilterMVA(21,
                                          [two_body_plus_photon],
                                          props['HHGammaBBDT'],
                                          nickname='HHGammaBBDT',
                                          preambulo=['from LoKiCore.math import log10'])
        filtered_two_plus_ee = FilterMVA(22,
                                         [two_body_plus_ee],
                                         props['HHGammaEEBBDT'],
                                         nickname='HHGammaEEBBDT')
        filtered_three_plus_one = FilterMVA(31,
                                            [three_body_plus_photon],
                                            props['HHHGammaBBDT'],
                                            nickname='HHHGammaBBDT',
                                            preambulo=['from LoKiCore.math import log10'])
        filtered_three_plus_ee = FilterMVA(32,
                                           [three_body_plus_ee],
                                           props['HHHGammaEEBBDT'],
                                           nickname='HHHGammaEEBBDT')
        # Build stages
        return {'RadiativeIncHHGamma'   : [TrackGEC(), PV3D('Hlt2'), filtered_two_plus_one],
                'RadiativeIncHHHGamma'  : [TrackGEC(), PV3D('Hlt2'), filtered_three_plus_one],
                'RadiativeIncHHGammaEE' : [TrackGEC(), PV3D('Hlt2'), filtered_two_plus_ee],
                'RadiativeIncHHHGammaEE': [TrackGEC(), PV3D('Hlt2'), filtered_three_plus_ee]}

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
        return {'HHCombiner': 'Topo2BodyCombos',
                'HHGammaCombo': comb_cuts,
                'HHGammaBBDT': {'BDT_PARAMS': 'Hlt2_RadiativeInc_2plus1_BDTParams_v1r0.txt',
                                'BDT_VARMAP': {'sv_ipchi2'         : 'log10(BPVIPCHI2())',
                                               'sv_children_pt_sum': '(CHILD(1, PT) + CHILD(2, PT))/MeV',
                                               'gamma_pt'          : 'CHILD(2, PT)/MeV',
                                               'nbody_fdchi2'      : 'log10(BPVVDCHI2)',
                                               'sv_vtx_chi2'       : 'log10(VFASPF(VCHI2))'},
                                'BDT_MIN': 10.0,
                                'CMB_VRT_MCOR_MIN': 1000*MeV,
                                'CMB_VRT_MCOR_MAX': 10000*MeV},
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
                                  'BDT_MIN': 0.0,
                                  'CMB_VRT_MCOR_MIN': 1000*MeV,
                                  'CMB_VRT_MCOR_MAX': 10000*MeV},
                'HHHCombiner': 'Topo3BodyCombos',
                'HHHGammaCombo': comb_cuts,
                'HHHGammaBBDT': {'BDT_PARAMS': 'Hlt2_RadiativeInc_3plus1_BDTParams_v1r0.txt',
                                 'BDT_VARMAP': {'nbody_doca_max'   : 'log10(BPVIPCHI2())',
                                               'sv_children_pt_sum': '(CHILD(1, PT) + CHILD(2, PT))*MeV',
                                               'gamma_pt'          : 'CHILD(2, PT)*MeV',
                                               'nbody_fdchi2'      : 'log10(BPVVDCHI2)',
                                               'sv_vtx_chi2'       : 'log10(VFASPF(VCHI2))'},
                                 'BDT_MIN': 0.0,
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
                                   'BDT_MIN': 0.0,
                                   'CMB_VRT_MCOR_MIN': 1000*MeV,
                                   'CMB_VRT_MCOR_MAX': 10000*MeV}}

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
