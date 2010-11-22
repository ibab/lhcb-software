#!/usr/bin/env python
# =============================================================================
# @file FillTask.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @reshuffled from Vanya Belyaev's TreeTask.py
# @date 2010-11-11
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

__all__ = [ 'FillTask' , 'fillDatabase' ]

import KaliCalo.Kali.FillTaskBase             as _FillTaskBase
import KaliCalo.MassDistribution.FillSelector as _FillSelector

FillTask = _FillTaskBase.FillTaskBase

fillDatabase = _FillTaskBase.FillDatabase( _FillSelector.fillDataBase )

# EOF
