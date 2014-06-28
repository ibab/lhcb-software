#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file Corr2D.py 
#
#  Simple 2D-decorrelation transformation 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-08
#
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Simple 2D-decorrelation transformation 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-08"
__all__     = ( 'Corr2D', )
# =============================================================================
import ROOT
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.Corr2D' module instead of 'PyPAW.Corr2D'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.Corr2D import *

# =============================================================================
if '__main__' == __name__ :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' Symbols : %s'  %  list (  __all__ ) 
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
