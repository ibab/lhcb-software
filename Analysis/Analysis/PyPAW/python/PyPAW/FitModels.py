#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file FitModels.py
#
#  Set of useful PDFs for various 1D and 2D fits
#  It includes
#  - soeme empricial PDFs to describe narrow peaks: Gauss, CrystalBall, ....
#  - some PDF to describe "wide" peaks: BreitWigner,LASS, Bugg, Flatter, ...
#  - some useful PDFs to describe smooth background: phase space ;
#    expo times polynomial; phase space times polynomial, ...
#  - set of smooth non-facrorizeable model for 2D fits 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Set of useful PDFs for varous 1D and 2D fits

It includes

Empricial PDFs to describe narrow peaks

  - Gauss 
  - Crystal Ball
  - right-side Crystal Ball
  - double-side Crystal Ball
  - Needham function for J/psi, psi' and Y peaks
  - Apolonios
  - bifurcated Gauissian
  - generalized normal v1 
  - generalized normal v2
  - skew Gaussian
  - Bukin,
  - Student-T
  - bifurcated Student-T

PDF to describe ``wide'' peaks

 - BreitWigner
 - LASS
 - Bugg
 - Flatter
 - ...
- some useful PDFs to describe smooth background in 1D
 - phase space 
 - expo times polynomial
 - phase space times polynomial
 - ... 
- set of smooth non-facrorizeable model for 2D fits 
- ...  
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
# =============================================================================
__all__ = (
    #
    ## empirical 1D signal models
    # 
    'Gauss_pdf'              , ## simple     Gauss
    'CrystalBall_pdf'        , ## Crystal-ball function
    'CrystalBallRS_pdf'      , ## right-side Crystal-ball function
    'CB2_pdf'                , ## double-sided Crystal Ball function    
    'Needham_pdf'            , ## Needham function for J/psi or Y (CB function with alpha=alpha(sigma))
    'Apolonios_pdf'          , ## Apolonios function         
    'BifurcatedGauss_pdf'    , ## bifurcated Gauss
    'GenGaussV1_pdf'         , ## generalized normal v1  
    'GenGaussV2_pdf'         , ## generalized normal v2 
    'SkewGauss_pdf'          , ## skewed gaussian
    'Bukin_pdf'              , ## generic Bukin PDF: skewed gaussian with exponential tails     
    'StudentT_pdf'           , ## Student-T function 
    'BifurcatedStudentT_pdf' , ## bifurcated Student-T function 
    #
    ## specializations:
    # 
    'D0_pdf'  , ## PDF for D0        : Bukin 
    'Dp_pdf'  , ## PDF for D+        : Bukin
    'Ds_pdf'  , ## PDF for Ds+       : Bukin 
    'Lc_pdf'  , ## PDF for Lambda_c+ : Gauss
    #
    'B0_pdf'  , ## pdf for B0        : double-sided Crystal Ball 
    'Bd_pdf'  , ## pdf for B0        : double-sided Crystal Ball 
    'Bu_pdf'  , ## pdf for B+        : double-sided Crystal Ball 
    'Bs_pdf'  , ## pdf for Bs        : double-sided Crystal Ball 
    'Bc_pdf'  , ## pdf for Bc+       : double-sided Crystal Ball 
    #
    'Manca_pdf'   , ## Manca function to fit Y->mu mu spectrum  [Y(1S),Y(2S),Y(3S)]
    'Manca2_pdf'  , ## Manca function to fit Y->mu mu spectrum  [Y(1S),Y(2S),Y(3S)]
    #
    ## pdfs for "wide" peaks, to be used with care - phase space corrections are large!
    # 
    'BreitWigner_pdf'      , ## (relativistic) 2-body Breit-Wigner
    'Flatte_pdf'           , ## Flatte-function  (pipi)
    'Flatte2_pdf'          , ## Flatte-function  (KK) 
    'LASS_pdf'             , ## kappa-pole
    'Bugg_pdf'             , ## sigma-pole
    #
    ## 1D-background models
    # 
    'Bkg_pdf'              , ## Background: exponential modified by positive polynom
    'PSPol_pdf'            , ## phase space modulated by positive polynomial
    #
    ## 2D non-factorazeable models
    #
    #
    ## 2D non-factorazeable symmetric models
    #
    #
    ## helpers
    #
    'H1D_dset'             , ## convertor of 1D-histogram to dataset 
    'H1D_pdf'              , ## convertor of 1D-histogram to PDF 
    'H2D_dset'             , ## convertor of 2D-histogram to dataset 
    'H2D_pdf'              , ## convertor of 2D-histogram to PDF 
    #
    'Fit1D'   , ## generic model for 1D-fit
    'Fit2D'   , ## generic model for 2D-fit
    )
# =============================================================================
import ROOT, math
from   AnalysisPython.Logger     import getLogger
from   PyPAW.PyRoUts             import VE, hID, allInts, cpp, rootID
from   PyPAW.Utils               import RooSilent, NoContext 
from   GaudiKernel.SystemOfUnits import GeV 
# =============================================================================
logger = getLogger ( __name__ ) 
# =============================================================================
from PyPAW.FitBasic          import *
logger.debug ('Import signal models from FitSignalModels')
from PyPAW.FitSignalModels   import * 
logger.debug ('Import background  models from FitBkgModels')
from PyPAW.FitBkgModels      import * 
logger.debug ('Import 2D background models from Fit2DModels')
from PyPAW.Fit2DModels       import * 
logger.debug ('Import specialized models from FitSpecificModels')
from PyPAW.FitSpecificModels import * 
logger.debug ('Import ``other'' models from FitOtherModels')
from PyPAW.FitOtherModels    import *

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
