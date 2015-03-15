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
                  'Postscale'   : {'Hlt2CaloTest' : 0.0}
                }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Stages import Photons
        stages = {'Photons' : [Photons(self.getProp('UseFullCalo'))]}
        line = Hlt2Line('CaloTest',
                        prescale  = self.prescale,
                        algos     = self.algorithms(stages)['Photons']
                        postscale = self.postscale)
