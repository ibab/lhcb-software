#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file FitBkgModels.py
#
#  Set of useful smooth 1D-models to describe background distribtions
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Set of useful smooth 1D-models to describe background distribtions
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    'Bkg_pdf'     , ## An exponential function, modulated by positiev polynomial
    'PSPol_pdf'   , ## A phase space  function, modulated by positive polynomial
    'PolyPos_pdf' , ## A positive polynomial
    ##
    'PS2_pdf'     , ## 2-body phase space (no parameters)
    'PSLeft_pdf'  , ## Low  edge of N-body phase space 
    'PSRight_pdf' , ## High edge of L-body phase space from N-body decays  
    'PSNL_pdf'    , ## L-body phase space from N-body decays  
    'PS23L_pdf'   , ## 2-body phase space from 3-body decays with orbital momenta
    )
# =============================================================================
import ROOT
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.FitBkgModels' module instead of 'PyPAW.FitBkgModels'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.FitBkgModels import *

        
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
