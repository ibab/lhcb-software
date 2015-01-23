#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Hlt2CaloTest.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   19.12.2014
# =============================================================================
"""Test for Calo reconstruction for HLT2."""

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CaloTestLinesConf(HltLinesConfigurableUser) :
    __slots__ = { 'UseFullCalo': True,
                  # Pre- and postscale
                  'Postscale'     : {'Hlt2CaloTestFull' : 0.0,
                                     'Hlt2CaloTestL0'   : 0.0 },
                  # IDs
                  'HltANNSvcID'  : {'Hlt2CaloTestFull' : 50520,
                                    'Hlt2CaloTestL0'  : 50521 }
                   }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        # from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotonsFromL0 as PhotonsFromL0
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons as PhotonsFull
        from HltLine.HltDecodeRaw import DecodeL0CALO

        ############################################################################
        #   L0Photons
        ############################################################################
        prescale = 0 if self.getProp('UseFullCalo') else 1
        line = Hlt2Line('CaloTestL0',
                        prescale=prescale,
                        algos=[PV3D('Hlt2'), DecodeL0CALO, PhotonsFromL0],
                        postscale=self.postscale)

        ############################################################################
        #   L0Photons
        ############################################################################
        prescale = 1 if self.getProp('UseFullCalo') else 0
        line = Hlt2Line('CaloTestFull',
                        prescale=prescale,
                        algos=[PV3D('Hlt2'), PhotonsFull],
                        postscale=self.postscale)

        for line, lid in self.getProp('HltANNSvcID').iteritems():
            HltANNSvc().Hlt2SelectionID.update({line + 'Decision' : lid})
