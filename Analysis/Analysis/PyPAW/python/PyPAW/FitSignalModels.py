#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file FitSignalModels.py
#
#  Set of useful PDFs for various 1D and 2D fits
#  It includes
#  - soeme empricial PDFs to describe narrow peaks: Gauss, CrystalBall, ....
#  - some PDF to describe "wide" peaks: BreitWigner,LASS, Bugg, Flatter, ...
#  - some useful PDFs to describe smooth background: phase space ;
#    expo times polynomial; phase space times polynomial, ...
#  - set of smooth non-facrorizeable model for 2D fits 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
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
    'Needham_pdf'            , ## Needham function for J/psi or Y fits 
    'Apolonios_pdf'          , ## Apolonios function         
    'BifurcatedGauss_pdf'    , ## bifurcated Gauss
    'GenGaussV1_pdf'         , ## generalized normal v1  
    'GenGaussV2_pdf'         , ## generalized normal v2 
    'SkewGauss_pdf'          , ## skewed gaussian
    'Bukin_pdf'              , ## generic Bukin PDF: skewed gaussian with exponential tails     
    'StudentT_pdf'           , ## Student-T function 
    'BifurcatedStudentT_pdf' , ## bifurcated Student-T function 
    #
    ## pdfs for "wide" peaks, to be used with care - phase space corrections are large!
    # 
    'BreitWigner_pdf'      , ## (relativistic) 2-body Breit-Wigner
    'Flatte_pdf'           , ## Flatte-function  (pipi)
    'Flatte2_pdf'          , ## Flatte-function  (KK) 
    'LASS_pdf'             , ## kappa-pole
    'Bugg_pdf'             , ## sigma-pole
    #
    )
# =============================================================================
import ROOT, math
from   PyPAW.PyRoUts             import cpp
from   GaudiKernel.SystemOfUnits import GeV 
from   PyPAW.FitBasic            import makeVar,Mass_pdf
# =============================================================================
from   AnalysisPython.Logger     import getLogger
logger = getLogger ( __name__ ) 
# =============================================================================


# =============================================================================
## @class Gauss_pdf
#  simple wrapper over Gaussian-pdf
#  @see RooGaussian
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Gauss_pdf(Mass_pdf) :
    """
    Simple Gaussian function 
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixSigma  = None ,
                   mass      = None ,
                   mean      = None ,
                   sigma     = None ) :
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigma   ,
                             fixMass , fixSigma ) 
        
        #
        ## build pdf
        # 
        self.pdf = ROOT.RooGaussian ( "gau_"      + name ,
                                      "Gauss(%s)" % name ,
                                      self.mass  ,
                                      self.mean  ,
                                      self.sigma )

# =============================================================================
## @class CrystalBall_pdf
#  @see Analysis::Models::CrystalBall
#  @see Gaudi::Math::CrystalBall
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class CrystalBall_pdf(Mass_pdf) :
    """
    Simple Crystal Ball
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               , 
                   fixMass  = None  ,
                   fixSigma = None  ,
                   fixAlpha = None  ,
                   fixN     = None  ,
                   mass     = None  ,
                   mean     = None  , 
                   sigma    = None  ,
                   alpha    = None  ,
                   n        = None  ) : 
                   
        
        Mass_pdf.__init__ ( self    , name     ,
                            mn      , mx       ,
                            mass    , mean     , sigma ,
                            fixMass , fixSigma ) 
        
        self.alpha = makeVar ( alpha ,
                               'alphaCB_%s'      % name ,
                               '#alpha_{CB}(%s)' % name ,
                               fixAlpha   , 0 , 10      )
        self.n     = makeVar ( alpha ,
                               'nCB_%s'      % name ,
                               'n_{CB}(%s)'  % name ,
                               fixN       , 0 , 20      )

        #
        ## finally build PDF 
        #
        self.pdf = cpp.Analysis.Models.CrystalBall (
            'cb_%s'           % name ,
            'CrystalBall(%s)' % name ,
            self.mass  ,
            self.mean  ,
            self.sigma ,
            self.alpha ,
            self.n     )

# =============================================================================
## @class CrystalBallRS_pdf
#  @see Analysis::Models::CrystalBallRS
#  @see Gaudi::Math::CrystalBallRS
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class CrystalBallRS_pdf(Mass_pdf) :
    """
    Simple right-side CrystalBall
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               , 
                   fixMass  = None  ,
                   fixSigma = None  ,
                   fixAlpha = None  ,
                   fixN     = None  ,
                   mass     = None  ,
                   mean     = None  , 
                   sigma    = None  ,
                   alpha    = None  ,
                   n        = None  ) : 
                   
        
        Mass_pdf.__init__ ( self    , name     ,
                            mn      , mx       ,
                            mass    , mean     , sigma ,
                            fixMass , fixSigma ) 
        
        self.alpha = makeVar ( alpha ,
                               'alphaCBRS_%s'      % name ,
                               '#alpha_{CBRS}(%s)' % name ,
                               fixAlpha   , 0 , 10      )
        self.n     = makeVar ( alpha ,
                               'nCBRS_%s'      % name ,
                               'n_{CBRS}(%s)'  % name ,
                               fixN       , 0 , 20      )

        #
        ## finally build PDF 
        #
        self.pdf = cpp.Analysis.Models.CrystalBallRS (
            'cbrs_%s'           % name ,
            'CrystalBallRS(%s)' % name ,
            self.mass  ,
            self.mean  ,
            self.sigma ,
            self.alpha ,
            self.n     )
        
# =============================================================================
## @class CB2_pdf
#  simple wrapper over double-sided Cristal Ball function
#  @see Analysis::Models::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class CB2_pdf(Mass_pdf) :
    """
    Define double sided Crystal Ball
    """
    def __init__ ( self            ,
                   name            ,
                   mn              ,
                   mx              ,
                   fixMass   = None ,
                   fixSigma  = None ,
                   fixAlphaL = None ,
                   fixAlphaR = None ,
                   fixNL     = None ,
                   fixNR     = None ,
                   mass      = None , 
                   mean      = None ,
                   sigma     = None ,
                   alphaL    = None ,
                   alphaR    = None ,
                   nL        = None ,
                   nR        = None ) : 
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name     ,
                             mn      , mx       ,
                             mass    , mean     , sigma   ,
                             fixMass , fixSigma )
        #
        ## treat the specific parameters
        #
        self.aL    = makeVar ( alphaL                  ,
                               "aL_%s"          % name ,
                               "#alpha_{L}(%s)" % name , fixAlphaL , 1.5 , 0 , 10 )
        self.nL    = makeVar ( nL                      ,                     
                               "nL_%s"          % name ,
                               "n_{L}(%s)"      % name , fixNL     , 1   , 0 , 10 )
        self.aR    = makeVar ( alphaR ,
                               "aR_%s"          % name ,
                               "#alpha_{R}(%s)" % name , fixAlphaR , 1.5 , 0 , 10 )
        self.nR    = makeVar ( nR                      ,
                               "nR_%s"          % name ,
                               "n_{R}(%s)"      % name , fixNR     , 2   , 0 , 10 )
        
        self.pdf = cpp.Analysis.Models.CrystalBallDS(
            "cb2_"       + name ,
            "CB_{2}(%s)" % name ,
            self.mass    ,
            self.mean    ,
            self.sigma   ,
            self.aL      ,
            self.nL      ,
            self.aR      ,
            self.nR      )
        

# =============================================================================
## @class Needham_pdf
#  Wrapper over CrystalBall function, using
#  alpha/sigma parameterization by Matt Needham
#  @thank Matthew Needham
#  @see Gaudi::Math::Needham 
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Needham_pdf(Mass_pdf) :
    """
    Define PDF for J/psi, psi' or Y-signals 
    """
    def __init__ ( self             ,
                   name             ,
                   mn       = 3.0   ,
                   mx       = 3.2   , 
                   fixMass  = 3.096 ,
                   fixSigma = 0.013 ,
                   mass     = None  ,
                   mean     = None  , 
                   sigma    = None  ,
                   a0       = None  ,
                   a1       = None  ,
                   a2       = None  ) : 
        
        Mass_pdf.__init__ ( self    ,
                            name    ,
                            mn      , mx       ,
                            mass    , mean     , sigma ,
                            fixMass , fixSigma ) 
        
        #
        unit = 1000
        #
        if   self.mass.getMin() <= 3.096 <= self.mass.getMax() : unit = 1000 
        elif self.mass.getMin() <=  3096 <= self.mass.getMax() : unit = 1
        elif self.mass.getMin() <= 9.460 <= self.mass.getMax() : unit = 1000 
        elif self.mass.getMin() <=  9460 <= self.mass.getMax() : unit = 1
        #
        self.a0 = makeVar ( a0                 ,
                            "a0n_%s"    % name ,
                            "a_{0}(%s)" % name ,
                            1.975              ,   0           , 10           )
        self.a1 = makeVar ( a1                 ,
                            "a1n_%s"    % name ,
                            "a_{1}(%s)" % name ,
                            -0.0011   * unit    , -10 * unit    , 10 * unit    )
        self.a2 = makeVar ( a2                 ,
                            "a2n_%s"    % name ,
                            "a_{2}(%s)" % name ,
                            -0.00018  * unit**2 , -10 * unit**2 , 10 * unit**2 )
        #
        self.pdf = cpp.Analysis.Models.Needham (
            'needham_%s'  % name ,
            'needham(%s)' % name ,
            self.mass  ,
            self.mean  ,
            self.sigma ,
            self.a0    ,
            self.a1    ,
            self.a2
            )
        
# =============================================================================
## @class Apolonios_pdf
#  simple wrapper over Apolonios PDF 
#  @see Analysis::Models::Apolonios 
#  The function is proposed by Diego Martinez Santos 
#  https://indico.itep.ru/getFile.py/access?contribId=2&resId=1&materialId=slides&confId=262633
#  Here a bit modified version is used with redefined parameter <code>n</code>
#  to be coherent with local definitions of Crystal Ball
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Apolonios_pdf(Mass_pdf) :
    """
    Simple wrapper over Apolonios PDF 
    The function is proposed by Diego Martinez Santos 
    https://indico.itep.ru/getFile.py/access?contribId=2&resId=1&materialId=slides&confId=262633
    Here a bit modified version is used with redefined parameter <code>n</code>
    to be coherent with local definitions of Crystal Ball
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixSigma  = None ,
                   fixAlpha  = None ,
                   fixN      = None ,
                   fixB      = None ,
                   mass      = None ,
                   mean      = None ,
                   sigma     = None ,
                   alpha     = None ,
                   n         = None ,
                   b         = None ) : 
                   
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigma   ,
                             fixMass , fixSigma ) 
        
        self.alpha = makeVar ( alpha                    ,
                               'alphaAp_%s'      % name ,
                               '#alpha_{Ap}(%s)' % name ,
                               fixAlpha , 0 , 10 )
        
        self.n     = makeVar ( n                   ,
                               'nAp_%s'     % name ,
                               'n_{Ap}(%s)' % name ,
                               fixN     , 0 , 20 )
        
        self.b     = makeVar ( b                     ,
                               'bAp_%s'     % name   ,
                               'b_{Ap}(%s)' % name   ,
                               fixB     , 0.01 , 100 ) 
        
        
        #
        ## finally build PDF
        #
        self.pdf  = cpp.Analysis.Models.Apolonios (
            "apolo_"        + name ,
            "Apolonios(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.sigma  ,
            self.alpha  ,
            self.n      ,
            self.b      ) 



# =============================================================================
## @class BifurcatedGauss_pdf
#  simple wrapper over bifurcated-gaussian
#  @see RooGaussian
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class BifurcatedGauss_pdf(Mass_pdf) :
    """
    bifurcated Gauss 
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixSigma  = None ,
                   fixAsym   = None ,
                   mass      = None ,
                   mean      = None ,
                   sigma     = None ,
                   asymmetry = None ) : 
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigma   ,
                             fixMass , fixSigma ) 
        
        self.asym = makeVar ( asymmetry                 ,
                              'asymmetry_%s'     % name ,
                              'sigma_{asym}(%s)' % name ,
                              fixAsym , -1 , 1  ) 
        
        self._lst_R = ROOT.RooArgList ( self.sigma , self.asym ) 
        self.sigmaR = ROOT.RooFormulaVar (
            "sigmaR_%s"     % name   ,
            "sigma_{R}(%s)" % name   ,
            "%s*(1+%s)"     % ( self.sigma.GetName() , self.asym.GetName() ) ,
            self._lst_R   )
        
        self._lst_L = ROOT.RooArgList ( self.sigma , self.asym ) 
        self.sigmaL = ROOT.RooFormulaVar (
            "sigmaL_%s"     % name   ,
            "sigma_{L}(%s)" % name   ,
            "%s*(1-%s)"     % ( self.sigma.GetName() , self.asym.GetName() ) ,
            self._lst_L   )
        
        #
        ## finally build pdf
        # 
        self.pdf = cpp.Analysis.Models.BifurcatedGauss (
            "fbgau_"         + name ,
            "BufurGauss(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.sigmaL ,
            self.sigmaR )

# =============================================================================
## @class GenGaussV1_pdf
#  Simple class that implements the generalized normal distribution v1
#  @see http://en.wikipedia.org/wiki/Generalized_normal_distribution#Version_1
#  @see Analysis::Models::GenGaussV1 
#  @see Gaudi::Math::GenGaussV1 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class GenGaussV1_pdf(Mass_pdf) :
    """
    Simple class that implements the generalized normal distribution v1
    see http://en.wikipedia.org/wiki/Generalized_normal_distribution#Version_1
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixAlpha  = None ,
                   fixBeta   = 2    , ## beta=2 is gaussian distribution 
                   mass      = None ,
                   mean      = None ,
                   alpha     = None ,
                   beta      = None ) : 
        
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             alpha   ,
                             fixMass , fixAlpha ) 
        
        #
        ## rename it!
        #
        self.alpha = self.sigma
        sname  = self.alpha.GetName  ()
        stitle = self.alpha.GetTitle ()
        gname  = sname .replace ( 'sigma' , 'alpha' )
        gtitle = stitle.replace ( 'sigma' , 'alpha' )
        self.alpha.SetName  ( gname  ) 
        self.alpha.SetTitle ( gtitle )

        self.beta  = makeVar ( beta ,
                               'betaV1_%s'      % name  ,
                               '#beta_{v1}(%s)' % name  ,
                               fixBeta , 1.e-4  , 1.e+6 ) 

        #
        ## finally build PDF
        #
        self.pdf = cpp.Analysis.Models.GenGaussV1 (
            "gengV1_"        + name ,
            "GenGaussV1(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.alpha  ,
            self.beta   )

# =============================================================================
## @class GenGaussV2_pdf
#  Simple class that implements the generalized normal distribution v2
#  @see http://en.wikipedia.org/wiki/Generalized_normal_distribution#Version_2
#  @see Analysis::Models::GenGaussV2
#  @see Gaudi::Math::GenGaussV2 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class GenGaussV2_pdf(Mass_pdf) :
    """
    Simple class that implements the generalized normal distribution v1
    see http://en.wikipedia.org/wiki/Generalized_normal_distribution#Version_1
    """
    def __init__ ( self               ,
                   name               ,
                   mn                 ,
                   mx                 ,
                   fixLocation = None ,
                   fixScale    = None ,
                   fixShape    = 0    , ## 0 corresponds to gaussian distribution 
                   mass        = None ,
                   mean        = None ,
                   alpha       = None ,
                   kappa       = None ) : 
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             alpha   ,
                             fixLocation , fixScale ) 
        
        #
        ## rename it!
        #
        self.alpha = self.sigma        
        sname  = self.alpha.GetName  ()
        stitle = self.alpha.GetTitle ()
        gname  = sname .replace ( 'sigma' , 'alpha' )
        gtitle = stitle.replace ( 'sigma' , 'alpha' )
        self.alpha.SetName  ( gname  ) 
        self.alpha.SetTitle ( gtitle )
        
        self.xi    = self.mean 
        # sname  = self.xi.GetName  ()
        # stitle = self.xi.GetTitle ()
        # gname  = sname .replace ( 'mean' , 'xi' )
        # gtitle = stitle.replace ( 'mean' , 'xi' )
        # self.xi.SetName     ( gname  ) 
        # self.xi.SetTitle    ( gtitle )
        
        self.kappa = makeVar ( kappa ,
                               'kappaV2_%s'      % name  ,
                               '#kappa_{v2}(%s)' % name  ,
                               fixShape , -4  , 4 ) 
        
        #
        ## finally build PDF
        #
        self.pdf = cpp.Analysis.Models.GenGaussV2 (
            "gengV2_"        + name ,
            "GenGaussV2(%s)" % name ,
            self.mass   ,
            self.xi     ,
            self.alpha  ,
            self.kappa  )

# =============================================================================
## @class SkewGauss_pdf
#  Simple class that implements the skew normal distribution
#  @see http://en.wikipedia.org/wiki/Skew_normal_distribution
#  @see Analysis::Models::SkewGauss 
#  @see Gaudi::Math::SkewGauss 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class SkewGauss_pdf(Mass_pdf) :
    """
    Simple class that implements the skew normal distribution
    see http://en.wikipedia.org/wiki/Skew_normal_distribution
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixOmega  = None ,
                   fixAlpha  = None , ## alpha=0 correspond to gaussian 
                   mass      = None ,
                   mean      = None ,
                   omega     = None ,
                   alpha     = None ) :
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             omega   ,
                             fixMass , fixOmega ) 

        self.omega = self.sigma

        
        self.alpha = makeVar ( alpha                    ,
                               'alphaSG_%s'      % name ,
                               '#alpha_{SG}(%s)' % name ,
                               fixAlpha , -100 , 100  ) 
        #
        ## finally build pdf
        # 
        self.pdf = cpp.Analysis.Models.SkewGauss (
            "skewg_"         + name ,
            "SkewGauss(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.omega  ,
            self.alpha  )

  
# =============================================================================
## @class Bukin_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bukin_pdf(Mass_pdf) :
    """
    Define PDFs for D0,D+,Ds+
    """
    def __init__ ( self            ,
                   name            ,
                   mn              , ## low-edge   (not used if 'mass' is specified)
                   mx              , ## high-edge  (not used if 'mass' is specified) 
                   fixMass  = None ,
                   fixSigma = None ,
                   fixXi    = None ,
                   fixRhoL  = None ,
                   fixRhoR  = None ,
                   mass     = None , 
                   mean     = None ,
                   sigma    = None ,
                   xi       = None ,
                   rhol     = None ,
                   rhor     = None ) :
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigma   ,
                             fixMass , fixSigma )
        #
        ## treat the specific parameters
        #
        ## asymmetry 
        self.xi    = makeVar ( xi                    ,
                               "xi_%s"        % name ,
                               "#xi(%s)"      % name , fixXi   , 0  , -1 , 1 )
        self.rhol  = makeVar ( rhol                  ,
                               "rhol_%s"      % name ,
                               "#rho_{L}(%s)" % name , fixRhoL , -1 , -2 , 0 )
        self.rhor  = makeVar ( rhor                  ,
                               "rhor_%s"      % name ,
                               "#rho_{R}(%s)" % name , fixRhoR , -1 , -2 , 0 )
        # 
        ## create PDF
        # 
        self.pdf = ROOT.RooBukinPdf (
            "bkn_"      + name ,
            "Bukin(%s)" % name ,
            self.mass  ,
            self.mean  ,
            self.sigma ,
            self.xi    ,
            self.rhol  ,
            self.rhor  )

# =============================================================================
## @class StudentT_pdf
#  Student-T distribution
#
#  \f[  f(y) = \frac{1}{\sqrt{\pi n}} \frac { \Gamma( \frac{n+1}{2}) } { \Gamma( \frac{n}{2}  ) }
#  \left( 1 + \frac{y^2}{n} \right)^{ -\frac{n+1}{2}} \f], 
#  where \f$ y = \frac{x - \mu}{\sigma} \f$  
# 
#  @see Analysis::Models::StudentT
#  @see Gaudi::Math::StudentT
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class StudentT_pdf(Mass_pdf) :
    """
    Student-T distribution
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixSigma  = None ,
                   fixN      = None , 
                   mass      = None ,
                   mean      = None ,
                   sigma     = None ,
                   n         = None ) :
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigma   ,
                             fixMass , fixSigma ) 
        
        # 
        self.n  = makeVar ( n                    ,
                            'nST_%s'      % name ,
                            '#n_{ST}(%s)' % name ,
                            fixN , 0 , 50  ) 
        #
        ## finally build pdf
        # 
        self.pdf = cpp.Analysis.Models.StudentT (
            "stT_"         + name ,
            "StudentT(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.sigma  ,
            self.n      )


# =============================================================================
## @class BifurcatedStudentT_pdf
#  bifurcated Student-T distribution
# 
#  @see Analysis::Models::BifurcatedStudentT
#  @see Gaudi::Math::BifurcatedStudentT
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class BifurcatedStudentT_pdf(Mass_pdf) :
    """
    Bifurcated Student-T distribution
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixSigmaL = None ,
                   fixSigmaR = None ,
                   fixNL     = None , 
                   fixNR     = None , 
                   mass      = None ,
                   mean      = None ,
                   sigmaL    = None ,
                   sigmaR    = None ,
                   nL        = None , 
                   nR        = None ) :
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigmaL  ,
                             fixMass , fixSigmaL )
        
        self.sigmaL = self.sigma
        if hasattr ( self , 'sigma' ) : delattr ( self , 'sigma' )
        self.sigmaR = makeVar ( sigmaR    ,
                                'sigmaR_BST_%s'      % name ,
                                '#sigma_{R,BST}(%s)' % name ,
                                fixSigmaR ,
                                self.sigmaL.getVal () ,
                                self.sigmaL.getMin () ,
                                self.sigmaL.getMax () ) 
        
        # 
        self.nL =  makeVar ( nL                    ,
                             'nLBST_%s'      % name ,
                             '#nL_{BST}(%s)' % name ,
                             fixNL , 0 , 50  ) 
        self.nR =  makeVar ( nR                    ,
                             'nRBST_%s'      % name ,
                             '#nR_{BST}(%s)' % name ,
                             fixNR , 0 , 50  ) 
        #
        ## finally build pdf
        # 
        self.pdf = cpp.Analysis.Models.BifurcatedStudentT (
            "bstT_"         + name ,
            "BStudentT(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.sigmaL ,
            self.sigmaR ,
            self.nL     ,
            self.nR     ) 
        
# =============================================================================
## @class Voigt_pdf
#  Voigt-pdf distribution
#  @see Analysis::Models::Voigt
#  @see Gaudi::Math::Voigt
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Voigt_pdf(Mass_pdf) :
    """
    Voigt function
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass   = None ,
                   fixSigma  = None ,
                   fixGamma  = None , 
                   mass      = None ,
                   mean      = None ,
                   sigma     = None ,
                   gamma     = None ) :
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mn , mx ,
                             mass    ,
                             mean    ,
                             sigma   ,
                             fixMass , fixSigma ) 
        
        # 
        self.gamma  = makeVar ( gamma                    ,
                                'gamma_%s'   % name ,
                                '#gamma(%s)' % name ,
                                fixGamma , 0 , 0.2 * ( mass.getMax()  - mass.getMin() ) ) 
        #
        ## finally build pdf
        # 
        self.pdf = cpp.Analysis.Models.Voigt (
            "vgt_"       + name ,
            "Voigt(%s)" % name ,
            self.mass   ,
            self.mean   ,
            self.sigma  ,
            self.gamma  )



# =============================================================================
## @class BreitWigner_pdf 
#  @see Analysis::Models::BreitWigner 
#  @see Gaudi::Math::BreitWigner
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class BreitWigner_pdf(Mass_pdf) :
    """
    Simple wrapper over relativistic Breit-Wigner function
    """
    def __init__ ( self               ,
                   name               ,
                   mn                 ,
                   mx                 ,
                   bw                 , ## Gaudi::Math::BreitWeigner object
                   fixMass     = None ,
                   fixGamma    = None ,
                   mass        = None ,
                   mean        = None , 
                   gamma       = None ,
                   convolution = None ,
                   useFFT      = True ) :
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name     ,
                             mn      , mx       ,
                             mass    , mean     , gamma   ,
                             fixMass , fixGamma )
        
        self.gamma = self.sigma
        sname  = self.gamma.GetName  ()
        stitle = self.gamma.GetTitle ()
        gname  = sname .replace ( 'sigma' , 'gamma' )
        gtitle = stitle.replace ( 'sigma' , 'Gamma' )
        self.gamma.SetName  ( gname  ) 
        self.gamma.SetTitle ( gtitle )
        
        #
        ## define the actual BW-shape using
        #      Gaudi::Math::BreitWeigner object
        #
        self.bw = bw                   ## Gaudi::Math::BreitWeigner object
        
        
        ## create PDF 
        self.breit = cpp.Analysis.Models.BreitWigner ( 
            "rbw_"    + name ,
            "RBW(%s)" % name ,
            self.mass    ,
            self.mean    ,
            self.gamma   ,
            self.bw      )

        if not convolution :
            self.pdf = self.breit 
        else :

            if   isinstance ( convolution , ROOT.RooAbsPdf ) :                
                self.convolution = convolution 
            elif isinstance ( convolution , (float,int,long) ) and not isinstance ( convolution , bool ) :
                self.cnv_mean  = makeVar (
                    0.0  ,
                    'CnvMean'       + name ,
                    'cnv_mean (%s)' % name ) 
                self.cnv_sigma = makeVar (
                    None ,
                    'CnvSigma'      + name ,
                    'cnv_sigma(%s)' % name ,
                    convolution , convolution , 0.25 * convolution , 2.00 * convolution )
                self.cnv_gauss = ROOT.RooGaussian (
                    'CnvGauss'     + name , 
                    'CnvGauss(%s)' % name ,
                    self.mass , self.cnv_mean , self.cnv_sigma )
                self.convolution = self.cnv_gauss
            else :
                logger.error('Unknown convolution type %s ' % convolution )
                
            if useFFT : self.pdf = ROOT.RooFFTConvPdf ( 'fft'     + name ,
                                                        'FFT(%s)' % name ,
                                                        self.mass , self.breit , self.convolution )
            else      : self.pdf = ROOT.RooNumConvPdf ( 'cnv'     + name ,
                                                        'CNV(%s)' % name ,
                                                        self.mass , self.breit , self.convolution )
                
            if isinstance ( self.convolution , ROOT.RooGaussian ) :
                if hasattr ( self , 'cnv_mean' ) and hasattr ( self , 'cnv_sigma' ) :
                    self.pdf.setConvolutonWindow( self.cnv_mean , self.cnv_sigma , 5 )

                    
# =============================================================================
## @class Flatte_pdf
#  Flatte function to describe dipion system near dikaon threshold
#
#  S.M.Flatte, 
#    "Coupled-channel analysis of the \f$\pi\eta\f$ 
#    and \f$K\bar{K}\f$ systems near \f$K\bar{K}\f$ threshold  
#    Phys. Lett. B63, 224 (1976
#  @see Analysis::Models::Flatte
#  @see Gaudi::Math::Flatte
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-18
class Flatte_pdf(Mass_pdf) :
    """
    Flatte function to describe dipion system near dikaon threshold
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass  = None  ,
                   fixM0G1  = 165   ,
                   fixG2oG1 = 4.21  ,
                   mass     = None  ,
                   m0_980   = None  ,    ## mass  of f0(980) resonance
                   m0g1     = None  ,    ## m0(f0(980))*gamma_1
                   g2og1    = None  ,    ## gamma2/gamma1 
                   mKaon    = 493.7 ,    ## kaon mass 
                   mPion    = 139.6 ) :  ## pion mass 
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name     ,
                             mn      , mx       ,
                             mass    , m0_980   , None ,
                             fixMass , None     )
        
        del self.sigma 

        self.m0_980 = self.mean 
        sname  = self.mean.GetName  ()
        stitle = self.mean.GetTitle ()
        gname  = sname .replace ( 'mean' , 'm0_980' )
        gtitle = stitle.replace ( 'mean' , 'm0_980' )
        self.m0_980.SetName  ( gname  ) 
        self.m0_980.SetTitle ( gtitle ) 
        
        self.m0g1 = makeVar  ( m0g1                        ,
                               'm0g1_flatte_%s'      % name ,
                               'm0*gamma1_Flatte_%s' % name ,
                               fixM0G1                      ,
                               1.e-5                        ,
                               1.e+5                        )
        
        self.g2og1 = makeVar ( g2og1    ,
                               'g2og1_flatte_%s'         % name ,
                               'gamma2/gamma1_Flatte_%s' % name ,
                               fixG2oG1 , 
                               0.01     , 100 ) 

        self.mKaon = mKaon
        self.mPion = mPion
        
        #
        ## build the actual pdf
        # 
        ## create PDF 
        self.pdf = cpp.Analysis.Models.Flatte ( 
            "flatte_"    + name ,
            "Flatte(%s)" % name ,
            self.mass    ,
            self.m0_980  ,
            self.m0g1    ,
            self.g2og1   ,
            self.mKaon   ,
            self.mPion   )
        
        
# =============================================================================
## @class Flatte2_pdf
#  Flatte function to describe dikaon system near threshold
#
#  S.M.Flatte, 
#    "Coupled-channel analysis of the \f$\pi\eta\f$ 
#    and \f$K\bar{K}\f$ systems near \f$K\bar{K}\f$ threshold  
#    Phys. Lett. B63, 224 (1976
#  @see Analysis::Models::Flatte2
#  @see Gaudi::Math::Flatte2
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-18
class Flatte2_pdf(Flatte_pdf) :
    """
    Flatte function to describe dipion system near dikaon threshold
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass  = None  ,
                   fixM0G1  = 165   ,
                   fixG2oG1 = 4.21  ,
                   mass     = None  ,
                   m0_980   = None  ,    ## mass  of f0(980) resonance
                   m0g1     = None  ,    ## m0(f0(980))*gamma_1
                   g2og1    = None  ,    ## gamma2/gamma1 
                   mKaon    = 493.7 ,    ## kaon mass 
                   mPion    = 139.6 ) :  ## pion mass 

        
        #
        ## initialize the base
        # 
        Flatte_pdf.__init__  ( self , name , mn , mx ,
                               fixMass  ,
                               fixM0G1  ,
                               fixG2oG1 ,
                               mass     ,
                               m0_980   , m0g1 , g2og1 ,
                               mKaon    , mPion )
        
        #
        ## build the actual pdf
        # 
        ## create PDF 
        self.pdf = cpp.Analysis.Models.Flatte2 ( 
            "flatte2_"    + name ,
            "Flatte2(%s)" % name ,
            self.mass    ,
            self.m0_980  ,
            self.m0g1    ,
            self.g2og1   ,
            self.mKaon   ,
            self.mPion   )

        
# =============================================================================
## @class LASS_pdf
#  kappa-pole 
#  @see Analysis::Models::LASS
#  @see Gaudi::Math::LASS
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class LASS_pdf(Mass_pdf) :
    """
    Kappa pole 
    """
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixMass  = None  ,
                   fixGamma = None  ,
                   mass     = None  ,
                   m0_1430  = None  ,     ## mass  of K*(1430)
                   g0_1430  = None  ,     ## width of K*(1430)
                   a_lass   = None  , 
                   r_lass   = None  ,
                   e_lass   = None  ,    ## elasticity                    
                   mKaon    = 493.7 ,    ## kaon mass 
                   mPion    = 139.6 ) :  ## pion mass 
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name     ,
                             mn      , mx       ,
                             mass    , m0_1430  , g0_1430 ,
                             fixMass , fixGamma )
        
        self.gamma = self.sigma
        sname  = self.gamma.GetName  ()
        stitle = self.gamma.GetTitle ()
        gname  = sname .replace ( 'sigma' , 'gamma_1430' )
        gtitle = stitle.replace ( 'sigma' , 'Gamma_1440' )
        self.gamma.SetName  ( gname  ) 
        self.gamma.SetTitle ( gtitle )
        
        self.g0_1430 = self.gamma 
        
        self.m0_1430 = self.mean 
        sname  = self.mean.GetName  ()
        stitle = self.mean.GetTitle ()
        gname  = sname .replace ( 'mean' , 'm0_1430' )
        gtitle = stitle.replace ( 'mean' , 'm0_1440' )
        self.m0_1430.SetName  ( gname  ) 
        self.m0_1430.SetTitle ( gtitle ) 
        
        self.a_lass = makeVar ( a_lass             ,
                                'aLASS_%s'  % name ,
                                "aLASS(%s)" % name ,
                                1.94e-3            ,
                                1.94e-3            ) 
        self.r_lass = makeVar ( r_lass             ,
                                'rLASS_%s'  % name ,
                                "rLASS(%s)" % name ,
                                1.76e-3            ,
                                1.76e-3            ) 
        self.e_lass = makeVar ( e_lass             ,
                                'eLASS_%s'  % name ,
                                "eLASS(%s)" % name ,
                                1.0                ,
                                1.0                )

        self.mKaon = mKaon
        self.mPion = mPion
        
        ## create PDF 
        self.pdf = cpp.Analysis.Models.LASS ( 
            "lass_"    + name ,
            "LASS(%s)" % name ,
            self.mass    ,
            self.m0_1430 ,
            self.g0_1430 ,
            self.a_lass  ,
            self.r_lass  ,
            self.e_lass  ,
            self.mKaon   ,
            self.mPion   ) 


# =============================================================================
## @class Bugg_pdf
#  sigma-pole 
#  @see Analysis::Models::Bugg
#  @see Gaudi::Math::Bugg
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Bugg_pdf(Mass_pdf) :
    """
    Bugg pole 
    """
    def __init__ ( self              ,
                   name              ,
                   mn                ,
                   mx                ,
                   fixMass  = 0.9264 , ## 
                   fixG2    = 0.0024 , ## g2-parameter
                   fixB1    = 0.5848 , ## b1-parameter [GeV]
                   fixB2    = 1.6663 , ## b2-parameter [GeV^-1]
                   fixA     = 1.082  , ##  a-parameter [GeV^2]
                   fixS1    = 2.8    , ## s1-parameter [GeV^2]
                   fixS2    = 3.5    , ## s2-parameter
                   mass     = None   ,
                   bugg_m   = None   ,
                   bugg_g2  = None   ,
                   bugg_b1  = None   ,
                   bugg_b2  = None   ,
                   bugg_a   = None   ,
                   bugg_s1  = None  ,
                   bugg_s2  = None   ,
                   mPion    = 0.1396 ) :  ## pion mass 
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name     ,
                             mn      , mx       ,
                             mass    , bugg_m   , bugg_g2 ,
                             fixMass , fixG2 )
        
        self.bugg_g2 = self.sigma
        sname  = self.gamma.GetName  ()
        stitle = self.gamma.GetTitle ()
        gname  = sname .replace ( 'sigma' , 'gamma2_Bugg' )
        gtitle = stitle.replace ( 'sigma' , 'Gamma2_Bugg' )
        self.bugg_g2.SetName  ( gname  ) 
        self.bugg_g2.SetTitle ( gtitle )
        self.gamma = self.bugg_g2 
        
        self.bugg_m = self.mean 
        sname  = self.mean.GetName  ()
        stitle = self.mean.GetTitle ()
        gname  = sname .replace ( 'mean' , 'm_Bugg' )
        gtitle = stitle.replace ( 'mean' , 'm_Bugg' )
        self.bugg_m.SetName  ( gname  ) 
        self.bugg_m.SetTitle ( gtitle ) 
        
        self.bugg_b1 = makeVar ( bugg_b1             ,
                                 'b1Bugg_%s'  % name ,
                                 "b1Bugg(%s)" % name ,
                                 fixB1               ,
                                 0 , 2               )
        
        self.bugg_b2 = makeVar ( bugg_b2             ,
                                 'b2Bugg_%s'  % name ,
                                 "b2Bugg(%s)" % name ,
                                 fixB2               ,
                                 1 , 2               ) 
        
        self.bugg_a  = makeVar ( bugg_a             ,
                                 'aBugg_%s'  % name ,
                                 "aBugg(%s)" % name ,
                                 fixA               ,
                                 0.5 , 5            ) 

        self.bugg_s1  = makeVar ( bugg_s1           ,
                                  's1Bugg_%s'  % name ,
                                  "s1Bugg(%s)" % name ,
                                  fixS1               ,
                                  1 , 5            ) 
        
        self.bugg_s2  = makeVar ( bugg_s2           ,
                                  's2Bugg_%s'  % name ,
                                  "s2Bugg(%s)" % name ,
                                  fixS2               ,
                                  1 , 5            ) 

        self.mPion = mPion
        
        ## create PDF 
        self.pdf = cpp.Analysis.Models.Bugg ( 
            "bugg_"    + name ,
            "Bugg(%s)" % name ,
            self.mass    ,
            self.bugg_m  ,
            self.bugg_g2 ,
            self.bugg_b1 ,
            self.bugg_b2 ,
            self.bugg_a  ,
            self.bugg_s1 ,
            self.bugg_s2 ,
            self.mPion   ) 
        

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
