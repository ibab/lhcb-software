#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Lines.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Hlt2 lines for radiative decays."""

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser


from B2XGamma import B2XGammaLines
from B2GammaGamma import B2GammaGammaLines
from Lb2L0Gamma import Lb2L0GammaLines

cuts = {'Common': {'TrackTisTos' : 'Hlt1(Two)?TrackMVADecision%TOS',
                   'PhotonTisTos': 'L0(Photon|Electron).*Decision%TOS',
                   'NTRACK_MAX'  : 300},
        # Converted photons
        'ConvLL': {'ee_Mass' : 50.0,
                   'ee_P'    : 5000.0,
                   'ee_PT'   : 200.0 },
        'ConvDD': {'ee_Mass' : 100.0,
                   'ee_P'    : 5000.0,
                   'ee_PT'   : 200.0},
       }

for line_conf in [B2GammaGammaLines, B2XGammaLines, Lb2L0GammaLines]:
    # Get cuts
    cuts.update(line_conf.get_cuts())

class RadiativeLines(Hlt2LinesConfigurableUser):
    __slots__ = cuts
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        hlt1_dependencies = {}
        stages = {}
        for line_conf in [B2GammaGammaLines, B2XGammaLines, Lb2L0GammaLines]:
            # Get HLT1 dependencies
            hlt1_dependencies.update(line_conf.get_hlt1())
            # Load stages
            stages.update(line_conf.get_stages())
        # Build lines
        for (linename, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(linename,
                     HLT1=hlt1_dependencies.get(linename, None),
                     algos=algos,
                     prescale=self.prescale,
                     postscale=self.postscale)

# EOF
