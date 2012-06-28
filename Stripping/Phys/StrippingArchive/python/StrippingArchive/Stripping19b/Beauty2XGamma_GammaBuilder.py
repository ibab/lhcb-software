#!/usr/bin/env python
# =============================================================================
# @file Beauty2XGamma_GammaBuilder.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-11-19
# =============================================================================
"""Photon builder for the Beauty2XGamma Stripping."""

__author__ = "Albert Puig (albert.puig@cern.ch)"

from copy import deepcopy
from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts

class GammaBuilder(object):
  "Produced the photons for the Radiative Stripping."
  
  def __init__(self, inputPhotons, config):
    self.config = config
    self.inputPhotons = inputPhotons
    self.photons = self._makePhotons()
    
  def _makePhotons(self):
    pass

# EOF
