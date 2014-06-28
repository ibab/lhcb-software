#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
## @file FitBasic.py
#
#  Set of useful basic utilities to build various fit models 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Set of useful basic utilities to build various fit models 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    ##
    'makeVar'   , ## helper function to create the proper RooRealVar
    'Mass_pdf'  , ## useful base class to create "signal" PDFs for mass-fits
    'H1D_dset'  , ## convertor of 1D-histo to RooDataHist 
    'H2D_dset'  , ## convertor of 2D-histo to RooDataHist 
    'H1D_pdf'   , ## convertor of 1D-histo to RooHistPdf 
    'H2D_pdf'   , ## convertor of 1D-histo to RooDataPdf
    ##
    'Fit1DBase' , ## useful base class fro 1D-models
    'Fit1D'     , ## the model for 1D-fit: signal + background + optional components  
    'Fit2D'     , ## the model for 2D-fit: signal + background + optional components
    ##
    'Adjust1D'  , ## addjust PDF to avoid zeroes (sometimes useful)
    )
# =============================================================================
import ROOT
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.FitBasic' module instead of 'PyPAW.FitBasic'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.FitBasic import *


        

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
