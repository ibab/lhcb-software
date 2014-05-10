#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
## @file FitModels.py
#
#  Set of useful PDFs for varous 1D and 2D fits
#  It includes
#  - soem empricial PDFs to describe narrow peaks: Gauss, CrystalBall, ....
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
    'Gauss_pdf'            , ## simple     Gauss
    'CrystalBall_pdf'      , ## Crystal-ball function
    'CrystalBallRS_pdf'    , ## right-side Crystal-ball function
    'CB2_pdf'              , ## double-sided Crystal Ball function    
    'Needham_pdf'          , ## Needham function for J/psi or Y (CB function with alpha=alpha(sigma))
    'Apolonios_pdf'        , ## Apolonios function         
    'BifurcatedGauss_pdf'  , ## bifurcated Gauss
    'GenGaussV1_pdf'       , ## generalized normal v1  
    'GenGaussV2_pdf'       , ## generalized normal v2 
    'SkewGauss_pdf'        , ## skewed gaussian
    'Bukin_pdf'            , ## generic Bukin PDF: skewed gaussian with exponential tails     
    'StudentT_pdf'         , ## Student-T function 
    #
    ## specializations:
    # 
    'D0_pdf'  , ## PDF for D0        : Bukin 
    'Dp_pdf'  , ## PDF for D+        : Bukin
    'Ds_pdf'  , ## PDF for Ds+       : Bukin 
    'Lc_pdf'  , ## PDF for Lambda_c+ : Gauss
    #
    'B0_pdf'  , ## pdf for B0        : double-sided Crystal Ball 
    'Bu_pdf'  , ## pdf for B+        : double-sided Crystal Ball 
    'Bs_pdf'  , ## pdf for Bs        : double-sided Crystal Ball 
    'Bc_pdf'  , ## pdf for Bc+       : double-sided Crystal Ball 
    #
    'Manca_pdf'            , ## Manca function to fit Y->mu mu spectrum  [Y(1S),Y(2S),Y(3S)]
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
## create/modify  the variable
#  Helper function for creation/modification/adjustment of variable
#  @code
#    v = makeVar ( 10   , 'myvar' , 'mycomment' )
#    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' ,     -1 , 1 )
#    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
#    v = makeVar ( None , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
#    v = makeVar ( None , 'myvar' , 'mycomment' , 10 , 0 , -1 , 1 )
#    v = makeVar ( v    , 'myvar' , 'mycomment' , 10 )
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
def makeVar ( var , name , comment , fix  , *args ) :
    """
    Make/modify  the variable:
    
    v = makeVar ( 10   , 'myvar' , 'mycomment' )
    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' ,     -1 , 1 )
    v = makeVar ( 10   , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
    v = makeVar ( None , 'myvar' , 'mycomment' , '' , 0 , -1 , 1 )
    v = makeVar ( None , 'myvar' , 'mycomment' , 10 , 0 , -1 , 1 )
    v = makeVar ( v    , 'myvar' , 'mycomment' , 10 )
    
    """
    ## create the fixed variable 
    if isinstance   ( var , ( float , int , long ) ) : var = ROOT.RooRealVar ( name , comment , var   )
    ## create the variable from parameters 
    if not var                                       : var = ROOT.RooRealVar ( name , comment , *args )
    ## fix it if needed 
    if isinstance ( fix , ( float , int , long ) )  and not var.isConstant () :
        if var.getMin() <= fix <= var.getMax() :  var.fix ( fix )
        else : logger.error ( 'Unable to fix %s at %s ' % ( var.GetName() , fix ) ) 
        #
        
    return var


# =============================================================================
## helper base class for implementaiton of various helper pdfs 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class Mass_pdf(object) :
    """
    helper base class for implementation of various pdfs 
    """
    def __init__ ( self            ,
                   name            ,
                   mn              ,
                   mx              , 
                   mass     = None ,
                   mean     = None ,
                   sigma    = None ,
                   fixMass  = None ,
                   fixSigma = None ) :
        
        #
        ## adjust the mass and edges, create if needed
        #
        self.mass = makeVar ( mass , "m_%s" % name , "mass(%s)" % name , None ,  min ( mn , mx ) , max( mn , mx ) )
        #
        self._mn = self.mass.getMin ()
        self._mx = self.mass.getMax ()
        #
        _dm      = self._mx - self._mn 
        
        #
        ## mean-value
        # 
        self.mean = makeVar ( mean , "mean_%s" % name , "mean(%s)" % name , fixMass ,  self._mn  , self._mx )
        #
        if self.mean.isConstant() :
            if not self._mn <= self.mean.getVal() <= self._mx :
                raise AttributeError( 'Fixed mass is not in mass-range (%s,%s)' % ( self._mn , self._mx ) )
        elif hasattr ( self.mean , 'setMin' ) and hasattr( self.mean , 'setMax' ) : 
            self.mean.setMin ( max ( self.mean.getMin () , self.mass.getMin() - 0.1 * _dm ) )
            self.mean.setMax ( min ( self.mean.getMax () , self.mass.getMax() + 0.1 * _dm ) )
            logger.debug ( 'Mean range is redefined to be (%s,%s)' % ( self.mean.getMin() , self.mean.getMax() ) ) 
            
        #
        ## check the fixed mass
        # 
        if  isinstance ( fixMass , float ) and not self.mean.isConstant() : 
            
            if not self.mass.getMin() <= fixMass <= self.mass.getMax() :
                raise AttributeError ( 'Fixed mass is not in mass-range (%s,%s)' % ( self.mass.getMin() , self.mass.getMax() ) )
            if not self.mean.getMin() <= fixMass <= self.mean.getMax() :
                raise AttributeError ( 'Fixed mass is not in mean-range (%s,%s)' % ( self.mean.getMin() , self.mean.getMax() ) )
            
            self.mean.fix ( fixMass )
            
        #
        ## sigma
        #
        sigma_max  = 1.2 * _dm / math.sqrt ( 12 )
        self.sigma = makeVar ( sigma ,  "sigma_%s" % name , "#sigma(%s)" % name  , fixSigma , 0 , sigma_max ) 
        #
        
    ## fit 
    def fitTo ( self             ,
                dataset          ,
                draw   = False   ,
                nbins  = 100     ,
                silent = False   ,
                *args , **kwargs ) :
        """
        Perform the fit
        """
        
        context = NoContext () 
        if silent : context = RooSilent() 
        
        with context :
            
            result = self.pdf.fitTo ( dataset , ROOT.RooFit.Save() , *args, **kwargs )
            
            if not draw :
                return result, None
            
            frame = self.mass.frame ( nbins )
            #
            
            dataset  .plotOn ( frame )
            
            self.pdf .plotOn ( frame  , ROOT.RooFit.LineColor  ( ROOT.kRed ) )

            
            frame.SetXTitle('')
            frame.SetYTitle('')
            frame.SetZTitle('')
            
            frame.Draw()
        
        return result, frame 
    
    
    ## fit the histogram 
    def fitHisto ( self , histo , draw = False , silent = False , *args , **kwargs ) :
        """
        Fit the histogram
        """
        
        context = NoContext () 
        if silent : context = RooSilent() 
        
        ## convert it! 
        hdset     = H1D_dset ( '',  histo , self.mass )
        self.hset = hdset.dset
        
        ## fit it! 
        return  self.fitTo ( self.hset     ,
                             draw          ,
                             len ( histo ) ,
                             silent        ,                             
                             *args         ,
                             **kwargs      )
    
# =============================================================================
_nemax = 80000  ## number of events per CPU-core 
_ncmax =     6  ## maximal number of CPUs: there are some problems with >= 7
                ## @see https://sft.its.cern.ch/jira/browse/ROOT-4897
#
_ncpus = []
# 
def ncpu (  events ) :
    #
    #
    ### return  ROOT.RooFit.Save()
    #
    n  = events // _nemax
    if n       <= 1 : return ROOT.RooFit.Save() ## fake!!! 
    # 
    import multiprocessing
    n_cores = multiprocessing.cpu_count()
    if n_cores <= 1 : return ROOT.RooFit.Save () ## fake!!! 
    #
    num = min ( n , n_cores , _ncmax )
    if not _ncpus :
        _ncpus.append ( num )   
    #
    return ROOT.RooFit.NumCPU ( num )

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
        self.aL    = makeVar ( alphaL ,  "aL_%s"   % name , "#alpha_{L}(%s)" % name , fixAlphaL , 1.5 , 0 , 10 )
        self.nL    = makeVar (     nL ,  "nL_%s"   % name ,      "n_{L}(%s)" % name , fixNL     , 1   , 0 , 10 )
        self.aR    = makeVar ( alphaR ,  "aR_%s"   % name , "#alpha_{R}(%s)" % name , fixAlphaR , 1.5 , 0 , 10 )
        self.nR    = makeVar (     nR ,  "nR_%s"   % name ,      "n_{R}(%s)" % name , fixNR     , 2   , 0 , 10 )
        
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
        self.a0 = makeVar ( a0   , "a0n_%s" % name , "a_{0}(%s)" % name ,  1.975              ,   0           , 10           )
        self.a1 = makeVar ( a1   , "a1n_%s" % name , "a_{1}(%s)" % name , -0.0011   * unit    , -10 * unit    , 10 * unit    )
        self.a2 = makeVar ( a2   , "a2n_%s" % name , "a_{2}(%s)" % name , -0.00018  * unit**2 , -10 * unit**2 , 10 * unit**2 )
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
                   fixBeta   = None , ## beta=2 is gaussian distribution 
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
    def __init__ ( self             ,
                   name             ,
                   mn               ,
                   mx               ,
                   fixLocation = None ,
                   fixScale    = None ,
                   fixShape    = None , ## 0 corresponds to gaussian distribution 
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

        self.alpha = self.sigma
        self.xi    = self.mean 
        
        self.kappa = makeVar ( kappa ,
                               'kappaV2_%s'      % name  ,
                               '#kappa_{v2}(%s)' % name  ,
                               fixShape , -10  , 10 ) 
        
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
        self.xi    = makeVar ( xi   ,   "xi_%s" % name ,      "#xi(%s)" % name , fixXi   , 0  , -1 , 1 )
        self.rhol  = makeVar ( rhol , "rhol_%s" % name , "#rho_{L}(%s)" % name , fixRhoL , -1 , -2 , 0 )
        self.rhor  = makeVar ( rhor , "rhor_%s" % name , "#rho_{R}(%s)" % name , fixRhoR , -1 , -2 , 0 )
        # 
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
# Specializations 
# =============================================================================

# =============================================================================
## @class B0_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @attention: mass is mandatory variable! 
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bd_pdf(CB2_pdf) :
    """
    Define PDF for Bd: Double sided Crystall Ball
    
    Attention: mass is mandatory argument!
    
    """
    def __init__ ( self                   ,
                   mass                   , ## mass is mandatory here! 
                   name      = 'Bd'       ,
                   fixMass   = 5.2791e+00 , ## to be released later 
                   fixSigma  = 7.2938e-03 , ## to be released later 
                   fixAlphaL = 1.4499e+00 , ## to be released later 
                   fixAlphaR = 1.9326e+00 , ## to be released later
                   fixNL     = 8.7234e+00 , ## to be released later 
                   fixNR     = 2.0377e+00 , ## to be released later 
                   mean      = None       ,
                   sigma     = None       ,
                   alphaL    = None       ,
                   alphaR    = None       ,
                   nL        = None       ,
                   nR        = None       ) :
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           fixMass          ,
                           fixSigma         ,
                           fixAlphaL        ,
                           fixAlphaR        ,
                           fixNL            ,
                           fixNR            ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

# =============================================================================
## @class Bu_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bu_pdf(CB2_pdf) :
    """
    Define PDF for B+: Double sided Crystall Ball 
    """
    def __init__ ( self                   ,
                   mass                   , ## mass is mandatory here! 
                   name      = 'Bu'       ,
                   fixMass   = 5.2791e+00 , ## to be released later 
                   fixSigma  = 7.2938e-03 , ## to be released later 
                   fixAlphaL = 1.4499e+00 , ## to be released later 
                   fixAlphaR = 1.9326e+00 , ## to be released later
                   fixNL     = 8.7234e+00 , ## to be released later 
                   fixNR     = 2.0377e+00 , ## to be released later 
                   mean      = None       ,
                   sigma     = None       ,
                   alphaL    = None       ,
                   alphaR    = None       ,
                   nL        = None       ,
                   nR        = None       ) :
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           fixMass          ,
                           fixSigma         ,
                           fixAlphaL        ,
                           fixAlphaR        ,
                           fixNL            ,
                           fixNR            ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

# =============================================================================
## @class Bs_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bs_pdf(CB2_pdf) :
    """
    Define PDF for Bs: Double sided Crystall Ball 
    """
    def __init__ ( self                   ,
                   mass                   , ## mass is mandatory here! 
                   name      = 'Bs'       ,
                   fixMass   = 5.3661e+00 , ## to be released later 
                   fixSigma  = 7.2938e-03 , ## to be released later 
                   fixAlphaL = 1.4499e+00 , ## to be released later 
                   fixAlphaR = 1.9326e+00 , ## to be released later
                   fixNL     = 8.7234e+00 , ## to be released later 
                   fixNR     = 2.0377e+00 , ## to be released later 
                   mean      = None       ,
                   sigma     = None       ,
                   alphaL    = None       ,
                   alphaR    = None       ,
                   nL        = None       ,
                   nR        = None       ) :
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           fixMass          ,
                           fixSigma         ,
                           fixAlphaL        ,
                           fixAlphaR        ,
                           fixNL            ,
                           fixNR            ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

# =============================================================================
## @class Bc_pdf
#  simple wrapper over CB2-pdf
#  @see Analysis::Models::CrystalBallDS
#  @see Gaudi::Math::CrystalBallDS
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Bc_pdf(CB2_pdf) :
    """
    Define PDF for Bc+ : Double sided Crystall Ball 
    """
    def __init__ ( self                   ,
                   mass                   , ## mass is mandatory here! 
                   name      = 'Bc'       ,
                   fixMass   = 6.267e+00  , ## to be released later 
                   fixSigma  = 7.2938e-03 , ## to be released later 
                   fixAlphaL = 1.4499e+00 , ## to be released later 
                   fixAlphaR = 1.9326e+00 , ## to be released later
                   fixNL     = 8.7234e+00 , ## to be released later 
                   fixNR     = 2.0377e+00 , ## to be released later 
                   mean      = None       ,
                   sigma     = None       ,
                   alphaL    = None       ,
                   alphaR    = None       ,
                   nL        = None       ,
                   nR        = None       ) :
        ## 
        CB2_pdf.__init__ ( self             ,
                           name             ,
                           mass.getMin()    ,
                           mass.getMax()    ,
                           fixMass          ,
                           fixSigma         ,
                           fixAlphaL        ,
                           fixAlphaR        ,
                           fixNL            ,
                           fixNR            ,
                           mass             , 
                           mean             ,
                           sigma            ,
                           alphaL           ,
                           alphaR           ,
                           nL               ,
                           nR               )

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
                self.cnv_mean  = makeVar          ( 0.0  , 'CnvMean'  + name , 'cnv_mean (%s)' % name ) 
                self.cnv_sigma = makeVar          ( None , 'CnvSigma' + name , 'cnv_sigma(%s)' % name ,
                                                    convolution , convolution , 0.25 * convolution , 2.00 * convolution )
                self.cnv_gauss = ROOT.RooGaussian ( 'CnvGauss'     + name , 
                                                    'CnvGauss(%s)' % name ,
                                                    self.mass , self.cnv_mean , self.cnv_sigma )
                self.convolution = self.cnv_gauss
            else :
                logger.error('Unknown convolution type %s ' % convolution )
                
            if useFFT : self.pdf = ROOT.RooFFTConvPdf ( 'fft'     + name , 'FFT(%s)' % name , self.mass , self.breit , self.convolution )
            else      : self.pdf = ROOT.RooNumConvPdf ( 'cnv'     + name , 'CNV(%s)' % name , self.mass , self.breit , self.convolution )
                
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
## @class  Bkg_pdf
#  simple wrapper over exponential modified with positive polynomial 
#  @see Analysis::Models::ExpoPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Bkg_pdf(object) :
    """
    Define pdf for background : Exponential
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,
                   mass             ,
                   power = 0        ) :
        
        self.mass  = mass
        self.power = power

        change = 1.e+16        
        taumin = math.log ( change ) / ( mass.getMax() - mass.getMin() ) 
        taumin = -1 * abs ( taumin ) 
        taumax =      abs ( taumin ) 

        #
        ## the exponential slope
        #
        self.tau  = makeVar ( None , "tau_%s" % name ,"tau(%s)" % name , None , 0 , taumin , taumax )
        #
        self.phis     = []
        # 
        if 0 >= self.power :
            
            self.pdf = ROOT.RooExponential ( 'exp_%s' % name  , 'exp(%s)' % name , mass , self.tau )
            
        elif  1 == power :
            
            self.phi = makeVar ( None , 'phi_%s' % name  , '#phi(%s)' % name  , None , 0 , -3.5 , 3.5 ) 
            #
            self.phis.append ( self.phi )
            #
            self.pdf = cpp.Analysis.Models.ExpoPositive ( 'expopos_%s'  % name ,
                                                          'expopos(%s)' % name ,
                                                          mass                 ,
                                                          self.tau             ,
                                                          self.phi             ,
                                                          mass.getMin()        ,
                                                          mass.getMax()        )
        elif 2 == power :

            self.phi1 = makeVar ( None , 'phi1_%s' % name  , '#phi_1(%s)' % name  ,  None , 0 , -3.5 , 3.5 ) 
            self.phi2 = makeVar ( None , 'phi2_%s' % name  , '#phi_2(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            #
            self.phis.append ( self.phi1 ) 
            self.phis.append ( self.phi2 )
            #
            self.pdf  = cpp.Analysis.Models.ExpoPositive ( 'expopos_%s'  % name ,
                                                           'expopos(%s)' % name ,
                                                           mass                 ,
                                                           self.tau             ,
                                                           self.phi1            ,
                                                           self.phi2            ,
                                                           mass.getMin()        ,
                                                           mass.getMax()        )
        elif 3 == power :
            
            self.phi1 = makeVar ( None , 'phi1_%s' % name  , '#phi_1(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            self.phi2 = makeVar ( None , 'phi2_%s' % name  , '#phi_2(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            self.phi3 = makeVar ( None , 'phi3_%s' % name  , '#phi_3(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            #
            self.phis.append ( self.phi1 ) 
            self.phis.append ( self.phi2 ) 
            self.phis.append ( self.phi3 )
            #
            self.pdf  = cpp.Analysis.Models.ExpoPositive ( 'expopos_%s'  % name ,
                                                           'expopos(%s)' % name ,
                                                           mass                 ,
                                                           self.tau             ,
                                                           self.phi1            ,
                                                           self.phi2            ,
                                                           self.phi3            ,
                                                           mass.getMin()        ,
                                                           mass.getMax()        )
        elif 4 == power :
            
            self.phi1 = makeVar ( None , 'phi1_%s' % name  , '#phi_1(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            self.phi2 = makeVar ( None , 'phi2_%s' % name  , '#phi_2(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            self.phi3 = makeVar ( None , 'phi3_%s' % name  , '#phi_3(%s)' % name  ,  None , 0 , -3.5 , 3.5 )
            self.phi4 = makeVar ( None , 'phi4_%s' % name  , '#phi_4(%s)' % name  ,  None , 0 , -3.5 , 3.5 )

            self.phis.append ( self.phi1 ) 
            self.phis.append ( self.phi2 ) 
            self.phis.append ( self.phi3 ) 
            self.phis.append ( self.phi4 )
            #
            self.pdf  = cpp.Analysis.Models.ExpoPositive ( 'expopos_%s'  % name ,
                                                           'expopos(%s)' % name ,
                                                           mass                 ,
                                                           self.tau             ,
                                                           self.phi1            ,
                                                           self.phi2            ,
                                                           self.phi3            ,
                                                           self.phi4            ,
                                                           mass.getMin()        ,
                                                           mass.getMax()        )

        elif 5 == power :
            
            self.phi1 = makeVar ( None , 'phi1_%s' % name  , '#phi_1(%s)' % name  , None , 0 , -3.5 , 3.5 )
            self.phi2 = makeVar ( None , 'phi2_%s' % name  , '#phi_2(%s)' % name  , None , 0 , -3.5 , 3.5 )
            self.phi3 = makeVar ( None , 'phi3_%s' % name  , '#phi_3(%s)' % name  , None , 0 , -3.5 , 3.5 )
            self.phi4 = makeVar ( None , 'phi4_%s' % name  , '#phi_4(%s)' % name  , None , 0 , -3.5 , 3.5 )
            self.phi5 = makeVar ( None , 'phi5_%s' % name  , '#phi_5(%s)' % name  , None , 0 , -3.5 , 3.5 )
            
            self.phis.append ( self.phi1 ) 
            self.phis.append ( self.phi2 ) 
            self.phis.append ( self.phi3 ) 
            self.phis.append ( self.phi4 ) 
            self.phis.append ( self.phi5 )
            #
            self.pdf  = cpp.Analysis.Models.ExpoPositive (
                'expopos_%s'  % name ,
                'expopos(%s)' % name ,
                mass                 ,
                self.tau             ,
                self.phi1            ,
                self.phi2            ,
                self.phi3            ,
                self.phi4            ,
                self.phi5            ,
                mass.getMin()        ,
                mass.getMax()        )
            
        elif 6 <= power :

            
            self.phi_list = ROOT.RooArgList () 
            for i in range ( 1 , power + 1 ) :
                #
                phi_i = makeVar   ( None , 'phi%d_%s' %  ( i , name ) , '#phi_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
                self.phis.append  ( phi_i )
                self.phi_list.add ( phi_i )
                #

            self.pdf  = cpp.Analysis.Models.ExpoPositive (
                'expopos_%s'  % name ,
                'expopos(%s)' % name ,
                mass                 ,
                self.tau             ,
                self.phi_list        ,
                mass.getMin()        ,
                mass.getMax()        )
            


# =============================================================================
## @class  PSPol_pdf
#  simple wrapper over phase space function modified with positive polynomial 
#  @see Analysis::Models::PhaseSpacePol
#  @see Maudi::Math::PhaseSpacePol
#  @see Maudi::Math::PhaseSpaceNL 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSPol_pdf(Mass_pdf) :
    """
    Simple wrapper over phase space function modified with positive polynomial 
    define pdf for background : Exponential
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,
                   mass             ,
                   phasespace       , 
                   power = 1        ) :
        
        #
        ## initialize the base
        # 
        Mass_pdf.__init__  ( self    , name , mass.getMin() , mass.getMax() , 
                             mass    ,
                             None    ,
                             None    ,
                             None    , None ) 
 
        self.power = power
        self.ps    = phasespace  ## Gaudi::Math::PhaseSpaceNL

        # 
        self.phis     = []
        # 
        self.phi_list = ROOT.RooArgList () 
        for i in range ( 1 , power + 1 ) :
            #
            phi_i = makeVar   ( None , 'phi%d_%s' %  ( i , name ) , '#phi_PSP_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            #
            
        self.pdf  = cpp.Analysis.Models.PhaseSpacePol (
            'pspol_%s'          % name ,
            'PhaseSpacePol(%s)' % name ,
            self.mass            ,
            self.ps              ,  ## Gaudi::Math::PhaseSpaceNL 
            self.phi_list        )

# =============================================================================
## simple class to adjust certaint PDF to avoid zeroes 
class Adjust1D(object) :
    """
    Simple class to adjust certaint PDF to avoid zeroes 
    """
    ## constructor
    def __init__ ( self          ,
                   name          ,
                   mass          ,
                   pdf           ,
                   value = 1.e-2 ) :
        
        self.mass    = mass
        self.old_pdf = pdf

        self.p0_pdf  = ROOT.RooPolynomial( 'p0_%s'     % name ,
                                           'poly0(%s)' % name , self.mass ) 
        
        self.num_s   = makeVar ( None , 'valueT_%s'    % name ,
                                 'value/true(%s)'      % name ,
                                 None ,
                                 0    , 1.e+6 )
        self.num_v   = makeVar ( None , 'valF_%s'       % name ,
                                 'value/fixtive(%s)'    % name ,
                                 value ,
                                 value , 0 , 10 )  
        
        #
        self.alist1 = ROOT.RooArgList (
            self.old_pdf ,
            self.p0_pdf    
            )
        
        self.alist2 = ROOT.RooArgList (
            self.num_s  ,
            self.num_v
            )
        
        #
        ## final PDF
        # 
        self.pdf  = ROOT.RooAddPdf  ( "adjust_"    + name ,
                                      "Adjust(%s)" % name ,
                                      self.alist1 ,
                                      self.alist2 )
        
# =============================================================================
## simple convertor of 1D-histo to data set
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H1D_dset(object) :
    """
    Simple convertor of 1D-histogram into data set
    """
    def __init__ ( self         ,
                   name         ,
                   histo        ,
                   mass  = None ) :
        #
        ## use mass-variable
        #
        if mass :
            mmin = mass.getMin ()
            mmax = mass.getMax ()
            
        self.mass = makeVar ( mass , 'm_%s' % name , 'mass(%s)' % name , None , *(histo.xminmax()) )

        if mass :
            mass.setMin ( mmin )
            mass.setMax ( mmax )
            
        self.vlst  = ROOT.RooArgList    ( self.mass )
        self.vimp  = ROOT.RooFit.Import ( histo     )
        self.dset  = ROOT.RooDataHist   (
            rootID ( 'hds_' ) ,
            "Data set for histogram '%s'" % histo.GetTitle() ,
            self.vlst  ,
            self.vimp  )

# =============================================================================
## simple convertor of 1D-histogram into PDF
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H1D_pdf(H1D_dset) :
    """
    Simple convertor of 1D-histogram into PDF 
    """
    def __init__ ( self         ,
                   name         ,
                   histo        ,
                   mass  = None ) :
        
        H1D_dset.__init__ ( self , name , histo , mass )
        
        self.vset  = ROOT.RooArgSet  ( self.mass )
        
        #
        ## finally create PDF :
        #
        self.pdf    = ROOT.RooHistPdf (
            'hpdf_%s'            % name ,
            'HistoPDF(%s/%s/%s)' % ( name , histo.GetName() , histo.GetTitle() ) , 
            self.vset  , 
            self.dset  ) 
        

# =============================================================================
## simple convertor of 2D-histo to data set
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H2D_dset(object) :
    """
    Simple convertor of 2D-histogram into data set
    """
    def __init__ ( self         ,
                   name         ,
                   histo2       ,
                   mass  = None ,
                   mass2 = None ) :
        #
        ## use mass-variable
        #
        self.mass  = makeVar ( mass  , 'm_%s'  % name , 'mass (%s)' % name , None , *(histo2.xminmax()) )
        self.mass1 = self.mass 
        self.mass2 = makeVar ( mass2 , 'm2_%s' % name , 'mass2(%s)' % name , None , *(histo2.yminmax()) )
        
        self.vlst  = ROOT.RooArgList    ( self.mass1 , self.mass2 )
        self.vimp  = ROOT.RooFit.Import ( histo2    )
        self.dset  = ROOT.RooDataHist   (
            rootID ( 'hds_' ) ,
            "Data set for histogram '%s'" % histo2.GetTitle() ,
            self.vlst  ,
            self.vimp  )


# =============================================================================
## simple convertor of 2D-histogram into PDF
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-12-01
class H2D_pdf(H2D_dset) :
    """
    Simple convertor of 2D-histogram into PDF 
    """
    def __init__ ( self         ,
                   name         ,
                   histo2       ,
                   mass  = None , 
                   mass2 = None ) :
        
        H2D_dset.__init__ ( name , histo2 , mass , mass2 )
        
        self.vset  = ROOT.RooArgSet  ( self.mass , self.mass2 )
        
        #
        ## finally create PDF :
        #
        self.pdf    = ROOT.RooHistPdf (
            'hpdf_%s'            % name ,
            'HistoPDF(%s/%s/%s)' % ( name , histo2.GetName() , histo2.GetTitle() ) , 
            self.vset  , 
            self.dset  )

        
# =============================================================================
## @class D0_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class D0_pdf(Bukin_pdf) :
    """
    Define PDF for D0
    """
    def __init__ ( self                 ,
                   name                 ,
                   fixMass  = None      ,
                   fixSigma = None      ,
                   fixXi    = -0.00044  ,
                   fixRhoL  = -0.170619 ,
                   fixRhoR  = -0.289993 ,
                   mass     = None      , 
                   mean     = None      ,
                   sigma    = None      ,
                   xi       = None      ,
                   rhol     = None      ,
                   rhor     = None      ) :
        
        Bukin_pdf.__init__ ( self     ,
                             name     ,
                             1.82     ,
                             1.92     ,
                             fixMass  ,
                             fixSigma ,
                             fixXi    ,
                             fixRhoL  , 
                             fixRhoR  , 
                             mass     , 
                             mean     ,
                             sigma    ,
                             xi       , 
                             rhol     ,
                             rhor     ) 
                             
# =============================================================================
## @class Dp_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Dp_pdf(Bukin_pdf) :
    """
    Define PDF for D+
    """
    def __init__ ( self                    ,
                   name                    ,
                   fixMass  = None         ,
                   fixSigma = None         ,
                   fixXi    = -2.44474e-04 ,
                   fixRhoL  = -1.07796e-01 , 
                   fixRhoR  = -2.84132e-01 ,
                   mass     = None         , 
                   mean     = None         ,
                   sigma    = None         ,
                   xi       = None         ,
                   rhol     = None         ,
                   rhor     = None         ) :
        
        Bukin_pdf.__init__ ( self     ,
                             name     ,
                             1.82     ,
                             1.91     ,
                             fixMass  ,
                             fixSigma ,
                             fixXi    ,
                             fixRhoL  , 
                             fixRhoR  , 
                             mass     , 
                             mean     ,
                             sigma    ,
                             xi       ,                            
                             rhol     ,
                             rhor     ) 
        
# =============================================================================
## @class Ds_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Ds_pdf(Bukin_pdf) :
    """
    Define PDF for Ds+
    """
    def __init__ ( self                    ,
                   name                    ,
                   fixMass  = 1.9672       ,
                   fixSigma = 0.0068       ,
                   fixXi    = -6.45755e-04 ,
                   fixRhoL  = -9.25349e-02 ,
                   fixRhoR  = -1.86051e-01 ,
                   mass     = None         , 
                   mean     = None         ,
                   sigma    = None         ,
                   xi       = None         ,
                   rhol     = None         ,
                   rhor     = None         ) :
        
        Bukin_pdf.__init__ ( self     ,
                             name     ,
                             1.90     ,
                             2.04     ,
                             fixMass  ,
                             fixSigma ,
                             fixXi    ,
                             fixRhoL  , 
                             fixRhoR  , 
                             mass     , 
                             mean     ,
                             sigma    ,
                             xi       ,
                             rhol     ,
                             rhor     ) 
       
# =============================================================================
## @class Lc_pdf
#  simple wrapper over Bukin-pdf
#  @see RooBukinPdf
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
class Lc_pdf(Bukin_pdf) :
    """
    Define PDF for Lc+
    """
    def __init__ ( self                     ,
                   name                     ,
                   fixMass  = 2.28590e+00   ,
                   fixSigma = 5.11874e-03   ,
                   fixXi    =  1.82493e-03  ,
                   fixRhoL  = -1.83140e-01  ,
                   fixRhoR  = -2.40318e-01  , 
                   mass     = None          , 
                   mean     = None          ,
                   sigma    = None          ,
                   xi       = None          ,
                   rhol     = None          ,
                   rhor     = None          ) :
        
        Bukin_pdf.__init__ ( self     ,
                             name     ,
                             2.24     , 
                             2.33     ,
                             fixMass  ,
                             fixSigma ,
                             fixXi    ,
                             fixRhoL  , 
                             fixRhoR  , 
                             mass     , 
                             mean     ,
                             sigma    ,
                             xi       ,
                             rhol     ,
                             rhor     ) 
        
        
# =============================================================================
## @class Fit1DBase
#  The base class for implementation of 1D-PDFs
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Fit1DBase (object) :
    """
    The PDF for general 1D-fit
    """
    def __init__ ( self              ,
                   signalset         ,
                   backgroundset     ) : 
        
        #
        self._signal_set     = signalset
        self._background_set = backgroundset
        #
        self._splots = []

    def signal_set     ( self ) : return self._signal_set
    def background_set ( self ) : return self._background_set
    
    ## make the actual fit (and optionally draw it!)
    def fitTo ( self , dataset , draw = False , nbins = 100 , silent = False , *args ) :
        """
        Perform the actual fit (and draw it)
        """
        context = NoContext () 
        if silent : context = RooSilent() 

        #
        ## define silent context
        #
        with context :
            
            result =  self.pdf.fitTo ( dataset   ,
                                       ROOT.RooFit.Save (   ) ,
                                       ncpu ( len ( dataset ) ) ,
                                       *args     )

        if 0 != result.status() and silent :
            logger.warning ( 'Fit status is %s. Refit in non-silent regime ' % result.status() )
            return self.fitTo ( dataset , draw , nbins , False , *args )

        if hasattr ( self , 'alist2' ) :

            nsum = VE()            
            for i in self.alist2 :
                nsum += i.as_VE() 
                if i.getVal() > 0.9 * i.getMax() :
                    logger.warning ( 'Variable %s == %s [close to maximum %s]' % ( i.GetName() , i.getVal () , i.getMax () ) )
                    
            if not dataset.isWeighted() : 
                nl = nsum.value() - 0.10 * nsum.error()
                nr = nsum.value() + 0.10 * nsum.error()
                if not nl <= len ( dataset ) <= nr :
                    logger.error ( 'Fit is problematic:  sum %s != %s ' % ( nsum , len( dataset ) ) )  
                    
        if not draw :
            return result, None 
        
        #
        ## again the context
        # 
        context = NoContext () 
        if silent : context = RooSilent() 
        
        with context :

            frame = self.mass.frame( nbins )
            #
            dataset  .plotOn ( frame )
            #
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.Components ( self.background_set () ) ,
                ROOT.RooFit.LineStyle  ( ROOT.kDashed           ) ,
                ROOT.RooFit.LineColor  ( ROOT.kBlue             ) )
            #
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.Components ( self.signal_set     () ) , 
                ROOT.RooFit.LineWidth  ( 2                      ) ,
                ROOT.RooFit.LineStyle  ( ROOT.kDotted           ) ,
                ROOT.RooFit.LineColor  ( ROOT.kMagenta          ) )
            
            self.pdf .plotOn (
                frame ,
                ROOT.RooFit.LineColor  ( ROOT.kRed              ) )
            
            frame.SetXTitle('')
            frame.SetYTitle('')
            frame.SetZTitle('')
            
            frame.Draw()
            
            return result, frame 

    ## fit the histogram (and draw it)
    def fitHisto ( self , histo , draw = False , silent = False , *args ) :
        """
        Fit the histogram (and draw it)
        """
        context = NoContext () 
        if silent : context = RooSilent() 

        ## convert it! 
        with context : 
            hdset     = H1D_dset ( '',  histo , self.mass )
            self.hset = hdset.dset
            
        ## fit it!!
        return self.fitTo ( self.hset , draw , len ( histo ) , silent , *args )

    ## perform sPlot-analysis 
    def sPlot ( self , dataset , *args    ) : 
        """
        Make sPlot analysis 
        """
        splot = ROOT.RooStats.SPlot ( rootID( "sPlot_" ) ,
                                      "sPlot"            ,
                                      dataset            ,
                                      self.pdf           ,
                                      self.alist2        )
        
        self._splots += [ splot ]
        
        return splot 


# =============================================================================
## @class Fit1D
#  The PDF for 1D-mass fits 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Fit1D (Fit1DBase) :
    """
    The PDF for general 1D-fit
    """
    def __init__ ( self              , 
                   signal            ,
                   background = None ,
                   components = []   , ## components
                   suffix     = ''   ,
                   bpower     = 0    ) :  
        
        self.signal     =      signal 
        self.mass       = self.signal.mass
        #
        if background : self.background = background 
        else          : self.background = Bkg_pdf  ( 'Background' + suffix , self.mass , power = bpower )

        #
        self.s = makeVar ( None , "S"+suffix , "Signal"     + suffix  , None ,  0  ,  1.e+6 )
        self.b = makeVar ( None , "B"+suffix , "Background" + suffix  , None ,  0  ,  1.e+6 )
        
        self.S = self.s
        self.B = self.b
        
        #
        self.alist1 = ROOT.RooArgList (
            self.signal     .pdf ,
            self.background .pdf 
            )
        
        # 
        self.alist2 = ROOT.RooArgList (
            self.s  ,
            self.b
            )
        
        self.nums = [ self.s , self.b ]
        
        i = 1

        self.more_components = components
        
        for c in components :
            
            if   isinstance ( c ,  ROOT.RooAbsPdf ) : self.alist1.add ( c     ) 
            elif hasattr    ( c ,'pdf' )            : self.alist1.add ( c.pdf )
            else :
                logger.warning('Unknown component type %s, skip it!' % type(c) )
                continue 
            
            si = makeVar ( None , "S%s_%d" % ( suffix , i ) , "Signal(%d)%s" % ( i , suffix ) , None  ,  0  ,  1.e+6 )            
            self.alist2.add  ( si )
            
            setattr ( self , si.GetName() , si ) 
            
            self.nums.append ( si ) 
            i += 1
            
        #
        ## final PDF
        # 
        self.pdf  = ROOT.RooAddPdf  ( "model_"    + suffix ,
                                      "model(%s)" % suffix ,
                                      self.alist1 ,
                                      self.alist2 )
        #
        ## finally initialize the base
        #
        self.ss_ = ROOT.RooArgSet ( self.signal    .pdf )
        self.bs_ = ROOT.RooArgSet ( self.background.pdf ) 
        # 
        Fit1DBase.__init__ ( self , self.ss_ , self.bs_ ) 
        

# =============================================================================
## @class Fit2D
#  the final full PDF for 2D-fits 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Fit2D (object) :
    """
    The final full PDF for 2D-fits 
    """
    def __init__ ( self              ,
                   #
                   signal_1          , 
                   signal_2          ,
                   suffix = ''       ,
                   #
                   bkg1       = None ,
                   bkg2       = None ,
                   bkgA       = None ,
                   bkgB       = None ,
                   bkg2D      = None ,
                   #
                   power1     = 0    ,
                   power2     = 0    ,
                   powerA     = 0    ,
                   powerB     = 0    ,
                   ## additional components 
                   components = []   ) :
        
        self.signal1   = signal_1
        self.signal2   = signal_2
        
        self.m1        = signal_1.mass
        self.m2        = signal_2.mass

        #
        ## First component: Signal(1) and Signal(2)
        # 
        self.ss_pdf = ROOT.RooProdPdf ( "S1S2pdf" + suffix , "Sig(1) x Sig(2)"  , self.signal1.pdf , self.signal2.pdf )
        
        if bkg1 : self.bkg1 = bkg1
        else    : self.bkg1 = Bkg_pdf ( 'Bkg(1)' + suffix , self.m1 , power1 )
        #
        ## Second component: Background(1) and Signal(2)
        # 
        self.bs_pdf = ROOT.RooProdPdf ( "B1S2pdf" + suffix  , "Bkg(1) x Sig(2)"  , self.bkg1.pdf , self.signal2.pdf )
        
        if bkg2 : self.bkg2 = bkg2 
        else    : self.bkg2 = Bkg_pdf ( 'Bkg(2)' + suffix , self.m2 , power2 )
        #
        ## Third component:  Signal(1) and Background(2)
        # 
        self.sb_pdf = ROOT.RooProdPdf ( "S1B2pdf" + suffix  , "Sig(1) x Bkg(2)"  , self.signal1.pdf , self.bkg2.pdf )
        
        ## 
        self._bkgs = ( bkg1 , bkg2 , bkgA , bkgB ) 
        #
        ## fourth component: Background(1) and Background(2) 
        # 
        if   bkg2D and isinstance ( bkg2D , ROOT.RooAbsPdf ) : self.bb_pdf = bkg2D 
        elif bkg2D and hasattr    ( bkg2D , 'pdf'          ) : self.bb_pdf = bkg2D.pdf
        else     :
            
            if bkgA : self.bkgA = bkgA
            else    : self.bkgA = Bkg_pdf ( 'Bkg(A)' + suffix , self.m1 , powerA )
            if bkgB : self.bkgB = bkgB
            else    : self.bkgB = Bkg_pdf ( 'Bkg(B)' + suffix , self.m2 , powerB )
            
            self.bb_pdf = ROOT.RooProdPdf ( "B1B2pdf" + suffix , "Bkg(A) x Bkg(B)"  , self.bkgA.pdf , self.bkgB.pdf )
            
        #
        ## coefficients
        #
        self.ss = makeVar ( None , "S1S2" + suffix , "Sig(1)&Sig(2)" + suffix , None , 1000  , 0 ,  1.e+6 )
        self.sb = makeVar ( None , "S1B2" + suffix , "Sig(1)&Bkg(2)" + suffix , None ,  100  , 0 ,  1.e+6 )
        self.bs = makeVar ( None , "B1S2" + suffix , "Bkg(1)&Sig(2)" + suffix , None ,  100  , 0 ,  1.e+6 )
        self.bb = makeVar ( None , "B1B2" + suffix , "Bkg(1)&Bkg(2)" + suffix , None ,   10  , 0 ,  1.e+6 )

        self.alist1 = ROOT.RooArgList ( self.ss_pdf , self.sb_pdf ,
            self.bs_pdf ,
            self.bb_pdf )
        self.alist2 = ROOT.RooArgList (
            self.ss ,
            self.sb ,
            self.bs ,
            self.bb )

        #
        ## treat additional components (if specified)
        # 
        self.components  = components
        self._cmps       = []
        icmp = 0 
        for cmp in self.components :

            icmp += 1
            
            if   isinstance ( cmp , ROOT.RooAbsPdf         ) : pass 
            elif hasattr    ( cmp , 'pdf'                  ) : cmp = cmp.pdf 
            elif isinstance ( cmp , ( float , int , long ) ) and not isinstance ( cmp , bool ) :
                px  = ROOT.RooPolynomial ( 'Px%d'    % icmp + suffix ,
                                           'Px(%d)'  % icmp + suffix , self.m1 ) 
                py  = ROOT.RooPolynomial ( 'Py%d'    % icmp + suffix ,
                                           'Py(%d)'  % icmp + suffix , self.m2 ) 
                cmp = ROOT.RooProdPdf    ( "Pxy%d"   % icmp + suffix ,
                                           "Pxy(%d)" % icmp + suffix , px , py )  
                self._cmps += [ px,py,cmp]
            else :
                logger.error( 'Unknown type of component %d %d ' % ( imcp , type(cmp) ) )

                
            nn = makeVar ( None ,
                           'Cmp%d'   % icmp + suffix ,
                           'Cmp(%d)' % icmp + suffix ,
                           None ,  100  ,  0 , 1.e+6 )  
            self._cmps += [ nn ]

            self.alist1.add ( cmp )
            self.alist2.add ( nn  )

        #
        ## build final PDF 
        # 
        self.pdf  = ROOT.RooAddPdf  ( "model2D"      + suffix ,
                                      "Model2D(%s)"  % suffix ,
                                      self.alist1 ,
                                      self.alist2 )
        
        self._splots = []

    ## fit 
    def fitTo ( self            ,
                dataset         ,
                draw   = False  ,
                xbins  = 50     ,
                ybins  = 50     ,
                silent = False  , *args ) :
        """
        Perform the fit
        """
        result = self.pdf.fitTo ( dataset                  , 
                                  ROOT.RooFit.Save ()      ,
                                  ncpu ( len ( dataset ) ) ,
                                  *args                    )

        #
        ## keep dataset (for drawing)
        #
        self.dataset = dataset
        
        if hasattr ( self , 'alist2' ) :
            
            nsum = VE()            
            for i in self.alist2 :
                nsum += i.as_VE() 
                if i.getVal() > 0.9 * i.getMax() :
                    logger.warning ( 'Variable %s == %s [close to maximum %s]' % ( i.GetName() , i.getVal () , i.getMax () ) )
                    
            if not dataset.isWeighted() : 
                nl = nsum.value() - 0.05 * nsum.error()
                nr = nsum.value() + 0.05 * nsum.error()
                if not nl <= len ( dataset ) <= nr :
                    logger.error ( 'Fit is problematic:  sum %s != %s ' % ( nsum , len( dataset ) ) )  
                    
        nbins = xbins 
        if not draw :
            return result,None
        
        return result, self.draw ( None , dataset , nbins , ybins , silent , *args ) 
    
    ## draw the projection over 1st variable 
    def draw1 ( self , dataset = None , nbins = 100 , silent = True , *args ) :
        """Draw the projection over 1st variable"""
        return self.draw ( self.m1 , dataset , nbins , 20    , silent , *args ) 
    ## draw the projection over 12nd variable 
    def draw2 ( self , dataset = None , nbins = 100 , silent = True , *args ) :
        """Draw the projection over 2nd variable"""
        return self.draw ( self.m2 , dataset , 100   , nbins , silent , *args ) 

    ## make 1D-plot 
    def draw ( self           ,
               drawvar = None ,
               dataset = None ,
               nbins   = 100  ,
               ybins   =  20  ,
               silent  = True ,
               *args )  : 
        """
        Make 1D-plot:
        """
        
        context = NoContext () 
        if silent : context = RooSilent()
        
        if not dataset :
            if hasattr ( self , 'dataset' ) : dataset = self.dataset 
            
        with context :
                
            if not drawvar :
                
                _xbins = ROOT.RooFit.Binning ( nbins ) 
                _ybins = ROOT.RooFit.Binning ( ybins ) 
                _yvar  = ROOT.RooFit.YVar    ( self.m2 , _ybins )
                _clst  = ROOT.RooLinkedList  ()
                hdata  = self.pdf.createHistogram ( hID() , self.m1 , _xbins , _yvar )
                hpdf   = self.pdf.createHistogram ( hID() , self.m1 , _xbins , _yvar )
                hdata.SetTitle(';;;')
                hpdf .SetTitle(';;;')
                _lst   = ROOT.RooArgList ( self.m1 , self.m2 )  
                if dataset : dataset.fillHistogram( hdata , _lst ) 
                self.pdf.fillHistogram  ( hpdf , _lst )

                hdata.lego ()
                hpdf .Draw ( 'same surf')
                
                return hpdf , hdata 
            
            frame = drawvar.frame( nbins )
            
            if dataset : dataset  .plotOn ( frame , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.sb_pdf.GetName() ) ,
                               ROOT.RooFit.LineStyle  ( ROOT.kDashed   ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kGreen    ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.bs_pdf.GetName() ) ,
                               ROOT.RooFit.LineStyle  ( ROOT.kDotted   ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kMagenta  ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.bb_pdf.GetName() ) ,                           
                               ROOT.RooFit.LineWidth  ( 1              ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kBlack    ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.Components ( self.ss_pdf.GetName() ) ,
                               ROOT.RooFit.LineWidth  ( 1              ) ,
                               ROOT.RooFit.LineColor  ( ROOT.kRed      ) , *args )
            
            self.pdf .plotOn ( frame ,
                               ROOT.RooFit.LineColor  ( ROOT.kRed      ) , *args )
            
            frame.SetXTitle ( '' )
            frame.SetYTitle ( '' )
            frame.SetZTitle ( '' )
            
            frame.Draw()
            
            return frame

    ## fit the histogram (and draw it)
    def fitHisto ( self , histo , draw = False , silent = False , *args ) :
        """
        Fit the histogram (and draw it)
        """
        context = NoContext () 
        if silent : context = RooSilent() 
        
        ## convert it! 
        with context : 
            self.hdset = H2D_dset ( '',  histo , self.m1 , self.m2  )
            self.hset  = self.hdset.dset
            
        ## fit it!!
        return self.fitTo ( self.hset      ,
                            draw           ,
                            histo.nbinsx() ,
                            histo.nbinsy() ,
                            silent         , *args ) 
    
    ## make splot-analysis 
    def sPlot ( self     ,
                dataset  ,
                *args    ) : 
        """
        make sPlot analysis 
        """
        splot = ROOT.RooStats.SPlot ( rootID ( "sPlot_" ) ,
                                      "sPlot"             ,
                                      dataset             ,
                                      self.pdf            ,
                                      self.alist2         , *args ) 
        
        self._splots += [ splot ]
        
        return splot 
    
# =============================================================================
## @class Manca_pdf 
#  the final full PDF for Y->mu+mu- fit 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-08-02
class Manca_pdf (Fit1DBase) :
    """
    The final full PDF for Y->mu+mu- fit 
    """
    def __init__ ( self          ,
                   mass          ,
                   name   = 'Y'  ,
                   power  = 0    ) :
        
        if     mass.getMin() <  9.460 and   9.60  <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() < 10.    and  10.500 <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() < 10.0   and  10.200 <= mass.getMax()  : gev_ =    1
        elif   mass.getMin() <  9460  and  10355  <= mass.getMax()  : gev_ = 1000
        elif   mass.getMin() < 10000  and  10500  <= mass.getMax()  : gev_ = 1000
        elif   mass.getMin() < 10000  and  10200  <= mass.getMax()  : gev_ = 1000
        else : raise TypeError ( "Illegal mass range %s<m<%s" % ( mass.getMin() , mass.getMax() ) ) 

        m_y1s  =  9.46030     * gev_ 
        s_y1s  =  4.3679e-02  * gev_ 
        dm_y2s = 10.02326     * gev_ - m_y1s
        dm_y3s = 10.3552      * gev_ - m_y1s

        # 
        self.mass = mass

        # =====================================================================
        ## Y(1S)
        # =====================================================================
        
        self.a0   = makeVar ( None , 'a0m_%s' % name , "a0 for Needham's function" , 1.91               ,    0.1             ,   3.0            )
        self.a1   = makeVar ( None , 'a1m_%s' % name , "a1 for Needham's function" , 1.1174 / gev_      ,  -10.0  / gev_     ,  10.0  /gev_     ) 
        self.a2   = makeVar ( None , 'a2m_%s' % name , "a2 for Needham's function" , -5.299 / gev_**2   , -100.0  / gev_**2  , 100.0  /gev_**2  )

        self.Y1S  = Needham_pdf (
            name + '1S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = m_y1s      ,
            fixSigma = s_y1s      ,
            mass     = self.mass  ,
            mean     = None       ,
            sigma    = None       ,
            a0       = self.a0    ,
            a1       = self.a1    ,
            a2       = self.a2    ) 
        #
        ## adjust a bit the values
        #
        self.m1s  = self.Y1S.mean
        self.m1s.release   ()
        self.m1s.setVal    ( m_y1s                )
        self.m1s.setMin    ( m_y1s - 0.15 * s_y1s )
        self.m1s.setMax    ( m_y1s + 0.15 * s_y1s )
        #
        self.sigma = self.Y1S.sigma
        self.sigma.release () 
        self.sigma.setVal  (       s_y1s )
        self.sigma.setMin  ( 0.3 * s_y1s )
        self.sigma.setMax  ( 3.5 * s_y1s )
        self.s1s  = self.sigma
        
        # =====================================================================
        ## Y(2S)
        # =====================================================================
        self.dm2s  = makeVar ( None ,
                               "dm2s"      + name ,
                               "dm2s(%s)"  % name    ,
                               dm_y2s                ,
                               dm_y2s - 0.20 * s_y1s , 
                               dm_y2s + 0.20 * s_y1s )
        
        self.aset11 = ROOT.RooArgList ( self.m1s , self.dm2s )
        self.m2s    = ROOT.RooFormulaVar (
            "m_" + name + '2S'   ,
            "m2s(%s)"  % name    ,
            "%s+%s" % ( self.m1s.GetName() , self.dm2s.GetName()  ) , 
            self.aset11       )
        
        self.aset12 = ROOT.RooArgList ( self.sigma , self.m1s , self.m2s ) 
        self.s2s    = ROOT.RooFormulaVar (
            "sigma_"  + name + '2S'    ,
            "#sigma_{Y2S}(%s)" % name  ,
            "%s*(%s/%s)"  % ( self.sigma.GetName() ,
                              self.m2s  .GetName() ,
                              self.m1s  .GetName() ) ,
            self.aset12  )
        
        self.Y2S   = Needham_pdf (
            name + '2S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = None       ,
            fixSigma = None       ,
            mass     = self.mass  ,
            mean     = self.m2s   ,
            sigma    = self.s2s   ,
            a0       = self.a0    ,
            a1       = self.a1    ,
            a2       = self.a2    ) 
        
        # =====================================================================
        ## Y(3S)
        # =====================================================================
        self.dm3s  = makeVar ( None ,
                               "dm3s"      + name ,
                               "dm3s(%s)"  % name    ,
                               dm_y3s                ,
                               dm_y3s - 0.20 * s_y1s , 
                               dm_y3s + 0.20 * s_y1s )
        
        self.aset21 = ROOT.RooArgList ( self.m1s , self.dm3s )
        self.m3s    = ROOT.RooFormulaVar (
            "m_"       + name + '(3S)' ,
            "m3s(%s)"  % name          ,
            "%s+%s" % ( self.m1s.GetName() , self.dm3s.GetName() ) ,
            self.aset21       )
        
        
        self.aset22 = ROOT.RooArgList ( self.sigma , self.m1s , self.m3s ) 
        self.s3s    = ROOT.RooFormulaVar (
            "sigma_"  + name + '3S'    ,
            "#sigma_{Y3S}(%s)" % name  ,
            "%s*(%s/%s)"  % ( self.sigma.GetName() ,
                              self.m3s  .GetName() ,
                              self.m1s  .GetName() ) , 
            self.aset22       )
        
        self.Y3S   = Needham_pdf (
            name + '3S'           ,
            mass.getMin()         ,
            mass.getMax()         ,
            fixMass  = None       ,
            fixSigma = None       ,
            mass     = self.mass  ,
            mean     = self.m3s   ,
            sigma    = self.s3s   ,
            a0       = self.a0    ,
            a1       = self.a1    ,
            a2       = self.a2    ) 
        
        #
        ## the actual signal PDFs
        # 
        self.y1s   = self.Y1S.pdf
        self.y2s   = self.Y2S.pdf
        self.y3s   = self.Y3S.pdf
        
        
        self.background = Bkg_pdf  ( 'Bkg%s' % name , self.mass , power = power )
        
        self.n1s = makeVar ( None , "N1S" + name , "Signal(Y1S)" ,  None , 1000 ,  0 ,  1.e+7 )
        self.n2s = makeVar ( None , "N2S" + name , "Signal(Y2S)" ,  None ,  300 ,  0 ,  1.e+6 )
        self.n3s = makeVar ( None , "N3S" + name , "Signal(Y3S)" ,  None ,  100 ,  0 ,  1.e+5 )
        self.b   = makeVar ( None , "B"   + name , "Background"  ,  None ,  100 ,  0 ,  1.e+8 )
        
        self.alist1 = ROOT.RooArgList ( self.y1s , self.y2s , self.y3s ) 
        self.alist2 = ROOT.RooArgList ( self.n1s , self.n2s , self.n3s ) 
        
        self.alist1 . add ( self.background.pdf )
        self.alist2 . add ( self.b              )
        
        self.pdf  = ROOT.RooAddPdf  (
            "manca_%s"  % name ,
            "manca(%s)" % name ,
            self.alist1 ,
            self.alist2 )
        
        self.dm2s.setConstant ( True )
        self.dm3s.setConstant ( True )
        
        self._splots = []
        
        self.s1_name = self.n1s.GetName ()
        self.s2_name = self.n2s.GetName ()
        self.s3_name = self.n3s.GetName ()

        #
        ## finally initialize the base
        # 
        Fit1DBase.__init__ ( self ,
                             ROOT.RooArgSet ( self.y1s , self.y2s , self.y3s) ,
                             ROOT.RooArgSet ( self.background.pdf           ) )
        
    def alpha_1S ( self ) : return self.Y1S.pdf.alpha ()
    def alpha_2S ( self ) : return self.Y2S.pdf.alpha ()
    def alpha_3S ( self ) : return self.Y3S.pdf.alpha ()


# =============================================================================
## 2D-non-factorizable models 
# =============================================================================

# =============================================================================
## @class Poly2DPositive_pdf
#  positive polynomial in 2D:
#  \f$  f(x,y) = \sum^{i=n}_{i=0}\sum{j=k}_{j=0} a^2_{\ij} B^n_i(x) B^k_j(y) \f$,
#  where \f$ B^n_i(x)\f$ denotes the basic bersntein polynomial 
#  @see Analysis::Models::Poly2DPositive
#  @see Gaudi::Math::Poly2DPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class Poly2DPositive_pdf(object) :
    """
    Positive polynomial in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   nx = 2           ,   ##  polynomial degree in X 
                   ny = 2           ) : ##  polynomial degree in Y 
        
        self.x  = x
        self.y  = y
        
        self.m1 = x ## ditto 
        self.m2 = y ## ditto 
        
        self.phis     = []
        self.phi_list = ROOT.RooArgList() 
        
        for i in range ( 1 ,  ( nx + 1 ) * ( ny + 1 ) ) :
            
            phi_i = makeVar   ( None ,
                                'phi_2d_%d_%s'    % ( i , name ) ,
                                '#phi(2D)_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            
        #
        ## finally build PDF 
        #
        self.pdf = cpp.Analysis.Models.Poly2DPositive (
            'p2Dp_%s'            % name ,
            'Poly2DPositive(%s)' % name ,
            self.x        ,
            self.y        ,
            nx            ,
            ny            , 
            self.phi_list )




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
