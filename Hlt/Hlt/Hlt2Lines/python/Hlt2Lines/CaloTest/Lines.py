#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Hlt2CaloTest.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   19.12.2014
# =============================================================================
"""Test for Calo reconstruction for HLT2."""
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CaloTestLines(Hlt2LinesConfigurableUser) :
    __slots__ = { 'UseFullCalo' : True,
                  # Postscale
                  'Postscale'   : {'Hlt2CaloTest' : 0.0,
                                   'Hlt2PFTest' : 1.0}
                }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Stages import Photons, ParticleFlow
        from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

        # Instantiate filters to make the warnings about TisTos go away
        photons = Photons(self.getProp('UseFullCalo'))
        photon_filter = Hlt2ParticleFilter("Filter", "ALL", [photons])

        pf_filter = Hlt2ParticleFilter("Filter", "ALL", [ParticleFlow()])

        # Final stages of the lines
        stages = {'CaloTest' : [photon_filter],
                  'PFTest' : [pf_filter]}

        # Build the lines
        for name, algos in self.algorithms(stages):
            Hlt2Line(name,
                     prescale  = self.prescale,
                     algos     = algos,
                     postscale = self.postscale)
