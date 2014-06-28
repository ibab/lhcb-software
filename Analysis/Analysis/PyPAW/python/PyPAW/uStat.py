#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id$
# ============================================================================
# @file uStat.py
#
# Helper module to get ``U-statistics'' useful for ``Goodnes-Of-Fit'' tests
#
# This is a simple translation of
#   the original C++ lines written by Greig Cowan into python
#
# Usage is fairly trivial:
#
#  @code
# 
#   >>> pdf  = ...               ## pdf
#   >>> data = ...               ## dataset
#   >>> pdf.fitTo( data , ... )  ## fit it!
#
#   >>> import PyPAW.uStat as uStat
#
#   >>> r,histo = uStat.uPlot ( pdf , data ) 
#   >>> print r                  ## print fit results
#   >>> histo.Draw()             ## plot the results  
#
#  @endcode
#
# @author Vanya Belyaev Ivan.Belyaev@cern.ch
# @date 2011-09-21
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
#
# ============================================================================
"""

Helper module to get ``U-statistics'' useful for ``Goodness-Of-Fit'' tests

This is a simple translation of
  the original C++ lines written by Greig Cowan into Python

Usage is fairly trivial:

   >>> pdf  = ...               ## pdf
   >>> data = ...               ## dataset
   >>> pdf.fitTo( data , ... )  ## fit it!

   >>> import PyPAW.uStat as uStat

   >>> r,histo = uStat.uPlot ( pdf , data ) 
   >>> print r                  ## print fit results
   >>> histo.Draw()             ## plot the results  
   
"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2010-09-21"
__version__ = "$Revision$"
# ============================================================================
__all__     = (
    "uPlot" ,  ## make  plot of U-statistics 
    "uDist" ,  ## calculate  U-statistics 
    "uCalc" ,  ## calclulate the distance between two data points 
    )
# ============================================================================
import ROOT
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.uStat' module instead of 'PyPAW.uStat'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.uStat import *


# ===========================================================================

if '__main__' == __name__ :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' Symbols : %s'  %  list (  __all__ ) 
    print '*'*120
    
# ===========================================================================
# The END 
# ===========================================================================
