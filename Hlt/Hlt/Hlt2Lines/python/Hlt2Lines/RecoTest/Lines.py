#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Hlt2RecoTest.py
# @author Sascha Stahl (sascha.stahl@cern.ch)
# @date   17.04.2015
# =============================================================================
"""Test for reconstruction for HLT2."""
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
from GaudiKernel.SystemOfUnits import GeV, MeV, mm

from Configurables import HltANNSvc

class RecoTestLines(Hlt2LinesConfigurableUser) :
    __slots__ = { 'Common'      : {'Pt'             : 0.0,
                                   'TrChi2Tight'    : 4,
                                   'MuPt'           : 500 * MeV,
                                   'VertexChi2'     : 25,
                                   'MassWindow'     : 70 * MeV},
                  'Postscale'   : {'Hlt2CreateReco' : 0.0}
                }


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Stages import CreatePions, JpsiReFitPVsFilter
        stages = {
                'CreateReco'   : [ CreatePions("AllOfThem") ]
                ,'JPsiReFitPVsTurbo'   : [ JpsiReFitPVsFilter("JpsiReFitPVsFilter") ]
                }
        for nickname, algos in self.algorithms(stages):
            if nickname is 'JPsiReFitPVsTurbo':
                HltANNSvc().Hlt2SelectionID.update( { "Hlt2JPsiReFitPVsTurboDecision":  57221 } )
                Hlt2Line(nickname, prescale = self.prescale,
                         algos = algos, postscale = self.postscale, Turbo=True)
            else:
                Hlt2Line(nickname, priority = 1, prescale = self.prescale,
                         algos = algos, postscale = self.postscale)
