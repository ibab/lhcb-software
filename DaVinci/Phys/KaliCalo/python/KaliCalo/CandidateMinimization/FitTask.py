#!/usr/bin/env python
# =============================================================================
# @file FitTask.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @reshuffled from Vanya Belyaev's FitTask.py
# @date 2010-11-13
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

__all__ = [ 'FitTask' , 'fitHistos' ]

import KaliCalo.Kali.FitTaskBase              as _FitTaskBase
import KaliCalo.CandidateMinimization.FitFunctions as _FitFunctions

FitTask = _FitTaskBase.FitTask

fitHistos = _FitTaskBase.FitHistos( _FitFunctions.fitHistoSet    ,
                                    _FitFunctions.preFitHistoSet )

# EOF
