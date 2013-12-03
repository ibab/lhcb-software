#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
##  This module contains set of simple and useful utilities for booking and  
#   manipulations with Gaudi-AIDA histograms, inspired by Thomas' request
#
#   @author Vanya BELYAEV ibelyaev@physics.syr.edu
#   @date 2007-08-03
#
#                     $Revision$
#   Last modification $Date$
#   by                $Author$
# =============================================================================
import warnings
warnings.warn (
    """
    AnalysisPython:
    Use 'GaudiPython.HistoUtils' module instead of 'HistoUtils'
    """                ,
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
from GaudiPython.HistoUtils import *
# =============================================================================
# The END 
# =============================================================================
