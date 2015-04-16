#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Lines.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Hlt2 lines for radiative decays."""

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV

from B2XGamma import B2XGammaLines
from B2XGammaUnbiased import B2XGammaUnbiasedLines
from B2GammaGamma import B2GammaGammaLines
from Lb2L0Gamma import Lb2L0GammaLines

cuts = {'Common': {'TrackTisTos' : 'Hlt1(Two)?TrackMVADecision%TOS',
                   'PhotonTisTos': 'L0(Photon|Electron).*Decision%TOS',
                   'NTRACK_MAX'  : 300},
        # Calo photons
        'CaloPhotons': {'PT_MIN': 200*MeV},
        # Converted photons
        'ConvLL': {'ee_Mass' : 50.0,
                   'ee_P'    : 5000.0,
                   'ee_PT'   : 200.0 },
        'ConvDD': {'ee_Mass' : 100.0,
                   'ee_P'    : 5000.0,
                   'ee_PT'   : 200.0},
       }

lines_to_run = [B2GammaGammaLines, B2XGammaLines, B2XGammaUnbiasedLines, Lb2L0GammaLines]

for line_conf in lines_to_run:
    # Get cuts
    cuts.update(line_conf.get_cuts())

class RadiativeLines(Hlt2LinesConfigurableUser):
    __slots__ = cuts
    def __apply_configuration__(self) :
        props = self.getProps()
        from HltLine.HltLine import Hlt2Line
        l0_dependencies = {}
        hlt1_dependencies = {}
        stages = {}
        for line_conf in lines_to_run:
            # Get L0 dependencies
            l0_dependencies.update(line_conf.get_l0())
            # Get HLT1 dependencies
            hlt1_dependencies.update(line_conf.get_hlt1())
            # Load stages
            stages.update(line_conf.get_stages(props))
        # Build lines
        for (linename, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(linename,
                     L0DU=l0_dependencies.get(linename, None),
                     HLT1=hlt1_dependencies.get(linename, None),
                     algos=algos,
                     prescale=self.prescale,
                     postscale=self.postscale)

# EOF
