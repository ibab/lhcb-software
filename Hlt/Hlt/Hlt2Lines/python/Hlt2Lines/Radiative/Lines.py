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

cuts = {'Common': {'TrackTisTos': 'Hlt1(Two)?Track(MVA)?.*Decision%TOS',
                   'PhotonTisTos': 'L0(Photon|Electron).*Decision%TOS',
                   'NTRACK_MAX': 300} }

cuts.update(B2GammaGammaLines.get_cuts())

cuts.update(B2XGammaLines.get_cuts())


class RadiativeLines(Hlt2LinesConfigurableUser):
    __slots__ = cuts
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        # Load stages
        stages = B2XGammaLines.get_stages()
        stages.update(B2GammaGammaLines.get_stages())
        # Build lines
        for (linename, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(linename,
                     algos=algos,
                     prescale=self.prescale,
                     postscale=self.postscale)


# EOF
