#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Hlt2RecoTest.py
# @author Sascha Stahl (sascha.stahl@cern.ch)
# @date   17.04.2015
# =============================================================================
"""Test for reconstruction for HLT2."""
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class RecoTestLines(Hlt2LinesConfigurableUser) :
    __slots__ = { 'Common' :        {'TrChi2'     :   10,    # Adimensional
                                      'Pt':            0 },                  # Postscale
                  'Postscale'   : {'Hlt2CreateReco' : 0.0}
                }

    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Stages import CreatePions
        stages = {'CreateReco'   : [ CreatePions("AllOfThem") ] }
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, priority = 10, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)

