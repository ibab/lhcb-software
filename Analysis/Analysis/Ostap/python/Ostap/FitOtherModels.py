#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file FitOtherModels.py
#
#  A set of variosu smooth shapes and PDFs 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""A set of various smooth shapes and PDFs"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    'GammaDist_pdf'      , ## Gamma-distributuon in shape/scale parameterization
    'GenGammaDist_pdf'   , ## Generalized Gamma-distribution
    'Amoroso_pdf'        , ## another view of generalized Gamma distribution
    'LogGammaDist_pdf'   , ## Gamma-distributuon in shape/scale parameterization
    'Log10GammaDist_pdf' , ## Gamma-distributuon in shape/scale parameterization
    'LogGamma_pdf'       , ## 
    'BetaPrime_pdf'      , ## Beta-prime distribution 
    'Landau_pdf'         , ## Landau distribution 
    'Argus_pdf'          , ## ARGUS distribution 
    'SinhAsinh_pdf'      , ## "Sinh-asinh" distribution
    )
# =============================================================================
import ROOT, math
from   Ostap.PyRoUts             import VE,cpp
from   GaudiKernel.SystemOfUnits import GeV 
# =============================================================================
from   AnalysisPython.Logger     import getLogger
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.FitOtherModels' )
else                       : logger = getLogger ( __name__               )
# =============================================================================
# Specializations of double-sided Crystal Ball function 
# =============================================================================
from   Ostap.FitBasic             import makeVar, PDF
# =============================================================================
## @class GammaDist_pdf
#  Gamma-distribution with shape/scale parameters
#  http://en.wikipedia.org/wiki/Gamma_distribution
#  It suits nicely for fits of multiplicity and/or chi2 distributions
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::GammaDist 
#  @see Gaudi::Math::GammaDist 
class GammaDist_pdf(PDF) :
    """
    Gamma-distribution with shape/scale parameters
    http://en.wikipedia.org/wiki/Gamma_distribution
    It suits nicely for firs of multiplicity and/or chi2 distributions
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   k     = None     ,   ## k-parameter
                   theta = None     ) : ## theta-parameter
        #
        PDF.__init__ ( self , name )
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'k_%s'                % name ,
                               'k_{#Gamma}(%s)'      % name , k     , 1 , 1.e-3 , 100 )
        self.theta = makeVar ( theta   ,
                               'theta_%s'            % name ,
                               '#theta_{#Gamma}(%s)' % name , theta , 1 , 1.e-3 , 100 )
        
        if self.k.getMin() <= 0 :
            self.k.setMin ( 1.e-3 ) 
            logger.warning( 'GammaDist(%s): min(k)     is set %s ' % ( name , self.k.getMin() ) )
            
        if self.theta.getMin() <= 0 :
            theta.setMin ( 1.e-3 ) 
            logger.warning( 'GammaDist(%s): min(theta) is set %s ' % ( name , self.theta.getMin() ) )
            
        self.pdf  = cpp.Analysis.Models.GammaDist (
            'gd_%s'         % name ,
            'GammaDist(%s)' % name ,
            self.x                 ,
            self.k                 ,
            self.theta             )

# =============================================================================
## @class GenGammaDist_pdf 
#  Generalized Gamma-distribution with additional shift parameter 
#  http://en.wikipedia.org/wiki/Generalized_gamma_distribution
#  Special cases : 
#  - p == 1      : Gamma  distribution
#  - p == k      : Weibull distribution
#  - p == k == 1 : Exponential distribution
#  - p == k == 2 : Rayleigh    distribution
#  @see Gaudi::Math::GenGammaDist
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Gaudi::Math::GenGammaDist 
#  @see Analysis::Models::GenGammaDist 
class GenGammaDist_pdf(PDF) :
    """
    Generalized Gamma-distribution with additional shift parameter 
    http://en.wikipedia.org/wiki/Generalized_gamma_distribution
    Special cases : 
    - p == 1      : Gamma  distribution
    - p == k      : Weibull distribution
    - p == k == 1 : Exponential distribution
    - p == k == 2 : Rayleigh    distribution
    """
    ## constructor
    def __init__ ( self                ,
                   name                ,   ## the name 
                   x                   ,   ## the variable
                   k     = None  ,   ## k-parameter
                   theta = None  ,   ## theta-parameter
                   p     = None  ,   ## p-parameter
                   low   = None  ) : ## low-parameter
        #
        PDF.__init__ ( self , name )
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'k_%s'                % name ,
                               'k_{#Gamma}(%s)'      % name , k     , 1 , 1.e-3 , 100 )
        self.theta = makeVar ( theta   ,
                               'theta_%s'            % name ,
                               '#theta_{#Gamma}(%s)' % name , theta , 1 , 1.e-3 , 100 )
        self.p     = makeVar ( p       ,
                               'p_%s'                % name ,
                               'p_{#Gamma}(%s)'      % name , p     , 1 , 1.e-3 ,   6 )
        
        self.low   = makeVar ( low      ,
                               'low_%s'              % name ,
                               'l_{#Gamma}(%s)'      % name ,
                               low      ,
                               min ( 0 , x.getMin() ) , x.getMax() ) 
        
        if self.k    .getMin() <= 0 :
            self.k   .setMin ( 1.e-3 ) 
            logger.warning( 'GenGammaDist(%s): min(k)     is set %s ' % ( name , self.k.getMin() ) ) 
            
        if self.theta.getMin() <= 0 :
            self.theta.setMin ( 1.e-3 ) 
            logger.warning( 'GenGammaDist(%s): min(theta) is set %s ' % ( name , self.theta.getMin() ) ) 
            
        if self.p    .getMin() <= 0 :
            self.p   .setMin ( 1.e-3 ) 
            logger.warning( 'GenGammaDist(%s): min(p)     is set %s ' % ( name , self.p.getMin() ) ) 
            
        self.pdf  = cpp.Analysis.Models.GenGammaDist (
            'ggd_%s'           % name ,
            'GenGammaDist(%s)' % name ,
            self.x         ,
            self.k         ,
            self.theta     ,
            self.p         , 
            self.low       )

# =============================================================================
## @class Amoroso_pdf
#  Another view on generalized gamma distribution
#  http://arxiv.org/pdf/1005.3274
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Gaudi::Math::Amoroso
#  @see Analysis::Models::Amoroso
class Amoroso_pdf(PDF) :
    """
    Another view on generalized gamma distribution
    http://arxiv.org/pdf/1005.3274
    """
    ## constructor
    def __init__ ( self          ,
                   name          ,   ## the name 
                   x             ,   ## the variable
                   theta = None  ,   ## theta-parameter
                   alpha = None  ,   ## alpha-parameter
                   beta  = None  ,   ## beta-parameter
                   a     = None  ) : ## s-parameter
        
        #
        PDF.__init__ ( self , name )
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.theta = makeVar ( theta   ,
                               'theta_%s'             % name ,
                               '#theta_{Amoroso}(%s)' % name , theta , 1 , 1.e-3 , 100 )
        self.alpha = makeVar ( alpha   ,
                               'alpha_%s'             % name ,
                               '#alpha_{Amoroso}(%s)' % name , alpha , 1 , 1.e-3 , 100 )
        self.beta  = makeVar ( beta    ,
                               'beta_%s'              % name ,
                               '#beta_{Amoroso}(%s) ' % name , beta  , 1 , 1.e-3 ,  10 )        
        self.a     = makeVar ( a       ,
                               'a_%s'                 % name ,
                               'a_{Amoroso}(%s)'      % name , a     , 1 , -10   ,  10  )
           
        logger.debug ('Amoroso theta  %s' % self.theta  )
        logger.debug ('Amoroso alpha  %s' % self.alpha  )
        logger.debug ('Amoroso beta   %s' % self.beta   )
        logger.debug ('Amoroso a      %s' % self.a      )

        self.pdf  = cpp.Analysis.Models.Amoroso (
            'amo_%s'      % name ,
            'Amoroso(%s)' % name ,
            self.x         ,
            self.theta     ,
            self.alpha     ,
            self.beta      ,
            self.a         )

# =============================================================================
## @class LogGammaDist_pdf
#  Distribution for log(x), where x follows Gamma distribution
#  It suits nicely for fits of log(multiplicity) and/or log(chi2) distributions
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::LogGammaDist 
#  @see Gaudi::Math::LogGammaDist 
#  @see AnalysisModels::GammaDist 
#  @see Gaudi::Math::GammaDist 
class LogGammaDist_pdf(PDF) :
    """
    Gamma-distribution for log(x), where x follows Gamma distribution
    It suits nicely for fits of log(multiplicity) and/or log(chi2) distributions
    """
    ## constructor
    def __init__ ( self         ,
                   name         ,   ## the name 
                   x            ,   ## the variable
                   k     = None ,   ## k-parameter
                   theta = None ) : ## theta-parameter
        #
        PDF.__init__ ( self , name )
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'k_%s'                   % name ,
                               'k_{log#Gamma}(%s)'      % name , k     , 1 , 1.e-5 , 1000 )
        self.theta = makeVar ( theta   ,
                               'theta_%s'               % name ,
                               '#theta_{log#Gamma}(%s)' % name , theta , 1 , 1.e-5 , 1000 )

        if self.k.getMin() <= 0 :
            self.k.setMin ( 1.e-3 ) 
            logger.warning( 'LogGammaDist(%s): min(k)     is set %s ' % ( name  , self.k.getMin() ) ) 
            
        if self.theta.getMin() <= 0 :
            theta.setMin ( 1.e-3 ) 
            logger.warning( 'LogGammaDist(%s): min(theta) is set %s ' % ( name , self.theta.getMin() ) )
            
        logger.debug ('LogGammaDist k      %s' % self.k      )
        logger.debug ('LogGammaDist theta  %s' % self.theta  )

        self.pdf  = cpp.Analysis.Models.LogGammaDist (
            'lgd_%s'           % name ,
            'LogGammaDist(%s)' % name ,
            self.x                 ,
            self.k                 ,
            self.theta             )

# =============================================================================
## @class Log10GammaDist_pdf
#  Distribution for log10(x), where x follows Gamma distribution
#  It suits nicely for fits of log10(multiplicity) and/or log10(chi2) distributions
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::Log10GammaDist 
#  @see Gaudi::Math::Log10GammaDist 
#  @see AnalysisModels::LogGammaDist 
#  @see Gaudi::Math::LogGammaDist 
#  @see AnalysisModels::GammaDist 
#  @see Gaudi::Math::GammaDist 
class Log10GammaDist_pdf(PDF) :
    """
    Gamma-distribution for log10(x), where x follows Gamma distribution
    It suits nicely for fits of log10(multiplicity) and/or log10(chi2) distributions
    """
    ## constructor
    def __init__ ( self         ,
                   name         ,   ## the name 
                   x            ,   ## the variable
                   k     = None ,   ## k-parameter
                   theta = None ) :  ## theta-parameter
        #
        PDF.__init__ ( self , name )
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.k     = makeVar ( k       ,
                               'k_%s'                     % name ,
                               'k_{log10#Gamma}(%s)'      % name , k     , 1 , 1.e-4 , 10000 )
        self.theta = makeVar ( theta   ,
                               'theta_%s'                 % name ,
                               '#theta_{log10#Gamma}(%s)' % name , theta , 1 , 1.e-4 , 10000 )
        
        if self.k.getMin() <= 0 :
            self.k.setMin ( 1.e-4 ) 
            logger.warning( 'Log10GammaDist(%s): min(k)     is set %s ' % ( name , self.k.getMin() ) ) 
            
            if self.theta.getMin() <= 0 :
                theta.setMin ( 1.e-4 ) 
            logger.warning( 'Log10GammaDist(%s): min(theta) is set %s ' % ( name , self.theta.getMin() ) ) 
            
        logger.debug ('Log10GammaDist k      %s' % self.k      )
        logger.debug ('Log10GammaDist theta  %s' % self.theta  )

        self.pdf  = cpp.Analysis.Models.Log10GammaDist (
            'lgd10_%s'           % name ,
            'Log10GammaDist(%s)' % name ,
            self.x                 ,
            self.k                 ,
            self.theta             )

# =============================================================================
## @class LogGamma_pdf
#  - http://arxiv.org/pdf/1005.3274
#  - Prentice, R. L. (1974). A log gamma model and its maximum likelihood
#                            estimation. Biometrika 61, 539
#  - Johnson, N. L., Kotz, S., and Balakrishnan, N. (1995). Continuous
#            univariate distributions, 2nd ed. Vol. 2. Wiley, New York.
#  - Bartlett, M. S. and G., K. M. (1946). The statistical analysis of
#                  variance-heterogeneity and the logarithmic transformation. 
#                 J. Roy. Statist. Soc. Suppl. 8, 1, 128.
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see AnalysisModels::LogGamma
#  @see Gaudi::Math::LogGamma
class LogGamma_pdf(PDF) :
    """
    - http://arxiv.org/pdf/1005.3274
    """
    ## constructor
    def __init__ ( self         ,
                   name         ,   ## the name 
                   x            ,   ## the variable
                   nu    = None ,   ## nu-parameter
                   lam   = None ,   ## lambda-parameter
                   alpha = None ) : ## nu-parameter
        #
        PDF.__init__ ( self , name )
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        xmin = self.x.getMin()
        xmax = self.x.getMax()
        dx   = xmax - xmin
        # 
        self.nu     = makeVar ( nu       ,
                                'nu_%s'                    % name ,
                                '#nu_{#log#Gamma}(%s)'     % name , nu    ,
                                0.5 * ( xmin + xmax ) ,
                                xmin - 10 * dx ,
                                xmax + 10 * dx ) 
        
        self.lam    = makeVar ( lam      ,
                                'lambda_%s'                % name ,
                                '#lambda_{#log#Gamma}(%s)' % name , lam   , 2 , -1000 , 1000 )
        
        self.alpha  = makeVar ( alpha    ,
                                'alpha_%s'                 % name ,
                                '#alpha_{#log#Gamma}(%s)'  % name , alpha , 1 , 1.e-3 , 1000 )
        
        if self.alpha.getMin() <= 0 :
            self.alpha.setMin ( 1.e-3 ) 
            logger.warning( 'LogGamma(%s): min(alpha) is set %s ' % ( name , self.alpha.getMin() ) ) 
            
        logger.debug ('LogGamma nu     %s' % self.nu     )
        logger.debug ('LogGamma lambda %s' % self.lam    )
        logger.debug ('LogGamma alpha  %s' % self.alpha  )

        self.pdf  = cpp.Analysis.Models.LogGamma (
            'lg_%s'        % name ,
            'LogGamma(%s)' % name ,
            self.x     ,
            self.nu    ,
            self.lam   ,
            self.alpha )

# =============================================================================
## @class BetaPrime_pdf
#  http://en.wikipedia.org/wiki/Beta_prime_distribution
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Analysis::Models::BetaPrime
#  @see Gaudi::Math::BetaPrime
class BetaPrime_pdf(PDF) :
    """
    - http://en.wikipedia.org/wiki/Beta_prime_distribution
    """
    ## constructor
    def __init__ ( self         ,
                   name         ,   ## the name 
                   x            ,   ## the variable
                   alpha = None ,   ## alpha-parameter
                   beta  = None ,   ## beta-parameter
                   scale = 1    ,   ## scale-parameter 
                   delta = 0    ) : ## shift-parameter 
        #
        PDF.__init__ ( self , name )
        # 
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.alpha  = makeVar ( alpha    ,
                                'alpha_%s'                 % name ,
                                '#alpha_{#beta#prime}(%s)' % name , alpha , 1 , 1.e-3 , 1000 )
        self.beta   = makeVar ( beta     ,
                                'beta_%s'                  % name ,
                                '#beta_{#beta#prime}(%s)'  % name , beta  , 1 , 1.e-3 , 1000 )

        
        if self.alpha.getMin() <= 0 :
            self.alpha.setMin ( 1.e-3 ) 
            logger.warning( 'BetaPrime(%s): min(alpha) is set %s ' % ( name , self.alpha.getMin() ) ) 
            
        if self.beta .getMin() <= 0 :
            self.beta.setMin ( 1.e-3 ) 
            logger.warning( 'BetaPrime(%s): min(beta) is set %s ' %  ( name , self.beta.getMin  () ) ) 
    
        self.scale  = makeVar ( scale     ,
                                'scale_%s'                 % name ,
                                '#theta_{#beta#prime}(%s)' % name , scale ,
                                1 , -1000 , 1000 )

        _dm = self.x.getMax()  - self.x.getMin() 
        self.delta  = makeVar ( delta     ,
                                'delta_%s'                 % name ,
                                '#delta_{#beta#prime}(%s)' % name , delta ,
                                0 ,
                                self.x.getMin() - 10 * _dm ,
                                self.x.getMax() + 10 * _dm ) 

        logger.debug ("Beta' alpha     %s" % self.alpha     )
        logger.debug ("Beta' beta      %s" % self.beta      )
        logger.debug ("Beta' scale     %s" % self.scale     )
        logger.debug ("Beta' sdelta    %s" % self.delta     )

        self.pdf  = cpp.Analysis.Models.BetaPrime (
            'bp_%s'         % name ,
            'BetaPrime(%s)' % name ,
            self.x     ,
            self.alpha ,
            self.beta  ,
            self.scale ,
            self.delta )

# =============================================================================
## @class Landau_pdf
#  http://en.wikipedia.org/wiki/Landau_distribution
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Analysis::Models::Landau
#  @see Gaudi::Math::Landau
class Landau_pdf(PDF) :
    """
    - http://en.wikipedia.org/wiki/Landau_distribution
    """
    ## constructor
    def __init__ ( self      ,
                   name      ,   ## the name 
                   x         ,   ## the variable
                   scale = 1 ,   ## scale-parameter 
                   delta = 0 ) : ## shift-parameter 
        #
        PDF.__init__ ( self , name ) 
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.scale  = makeVar ( scale     ,
                                'scale_%s'            % name ,
                                '#theta_{Landau}(%s)' % name , scale ,
                                1 , -1000 , 1000 )
        
        _dm = self.x.getMax()  - self.x.getMin() 
        self.delta  = makeVar ( delta     ,
                                'delta_%s'            % name ,
                                '#delta_{Landau}(%s)' % name , delta ,
                                0 ,
                                self.x.getMin() - 10 * _dm ,
                                self.x.getMax() + 10 * _dm ) 
                                
        logger.debug ('Landau scale  %s' % self.scale  )
        logger.debug ('Landau delta  %s' % self.delta  )
        
        self.pdf  = cpp.Analysis.Models.Landau (
            'land_%s'    % name ,
            'Landau(%s)' % name ,
            self.x     ,
            self.scale ,
            self.delta )

# =============================================================================
## @class Argus_pdf
#  http://en.wikipedia.org/wiki/ARGUS_distribution
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-11
#  @see Analysis::Models::Argus
#  @see Gaudi::Math::Argus
class Argus_pdf(PDF) :
    """
    - http://en.wikipedia.org/wiki/ARGUS_distribution
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   x                ,   ## the variable
                   shape = None     ,   ## shape-parameter 
                   high  = None     ,   ## high-parameter 
                   low   = 0        ) : ## low-parameter 
        #
        PDF.__init__ ( self , name ) 
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.shape  = makeVar ( shape     ,
                                'shape_%s'         % name ,
                                '#chi_{Argus}(%s)' % name , shape ,
                                1     ,
                                1.e-4 , 20 )
        
        self.high  = makeVar ( high      ,
                               'high_%s'          % name ,
                               'high_{Argus}(%s)' % name , high ,
                               0.1 * self.x.getMin ()  + 0.9 * self.x.getMax () , 
                               self.x.getMin () ,
                               self.x.getMax () )
        
        _dm  = self.x.getMax()  - self.x.getMin()
        lmin = min ( 0 , self.x.getMin() - 10 * _dm )
        lmax =           self.x.getMax() + 10 * _dm 
        self.low   = makeVar ( low      ,
                               'low_%s'          % name ,
                               'low_{Argus}(%s)' % name , low ,
                               0.9 * self.x.getMin ()  + 0.1 * self.x.getMax () , 
                               lmin , lmax ) 
        
        logger.debug ('ARGUS shape  %s' % self.shape  )
        logger.debug ('ARGUS high   %s' % self.high   )
        logger.debug ('ARGUS low    %s' % self.low    )

        self.pdf  = cpp.Analysis.Models.Argus (
            'arg_%s'    % name ,
            'Argus(%s)' % name ,
            self.x     ,
            self.shape ,
            self.high  ,
            self.low   )

# =============================================================================
## @class SinhAsinh_pdf
#
#  Jones, M. C.; Pewsey, A. (2009). 
#  "Sinh-arcsinh distributions". Biometrika 96 (4): 761. 
#  doi:10.1093/biomet/asp053
#  http://oro.open.ac.uk/22510
#
#  Location & scale  parameters are the 
#  usual representation of the family of 
#  distributions 
#  - \f$\epsilon\f$ parameter control the skewness 
#  - \f$\delta\f$   parameter control the kurtosis 
#  Normal distribtion reappears as \f$\epsilon=0\f$ 
#  and \f$\delta=1\f$ 
#  The heavy tails correspond to \f$\delta<1\f$, 
#  light tails correpond to \f$\delta>1\f$
#  @date   2013-05-11
#  @see Analysis::Models::SinhAsinh
#  @see Gaudi::Math::SingAsinh
class SinhAsinh_pdf(PDF) :
    """
    Jones, M. C.; Pewsey, A. (2009). 
    'Sinh-arcsinh distributions'. Biometrika 96 (4): 761. 
    doi:10.1093/biomet/asp053
    http://oro.open.ac.uk/22510
    
    Location & scale  parameters are the 
    usual representation of the family of 
    distributions 
    - \f$\epsilon\f$ parameter control the skewness 
    - \f$\delta\f$   parameter control the kurtosis 
    Normal distribtion reappears as \f$\epsilon=0\f$ 
    and \f$\delta=1\f$ 
    The heavy tails correspond to \f$\delta<1\f$, 
    light tails correpond to \f$\delta>1\f$
    """
    ## constructor
    def __init__ ( self              ,
                   name              ,   ## the name 
                   x                 ,   ## the variable
                   mu      = None    ,   ## location parameter
                   sigma   = None    ,   ## scale/sigma parameter
                   epsilon = 0       ,
                   delta   = 1       ) : 
        #
        PDF.__init__ ( self , name ) 
        #
        self.x     = x
        self.mass  = x  ## ditto
        #
        self.mu       = makeVar ( mu               ,
                                  'mu_%s'           % name ,
                                  '#mu_{shash}(%s)' % name ,
                                  mu         ,
                                  x.getMin() ,
                                  x.getMax() )  
        
        self.sigma    = makeVar ( sigma                       ,
                                  'sigma_%s'           % name ,
                                  '#sigma_{shash}(%s)' % name ,
                                  sigma ,
                                  0.100 * ( x.getMax() - x.getMin() ) ,
                                  0.002 * ( x.getMax() - x.getMin() ) ,
                                  0.501 * ( x.getMax() - x.getMin() ) ) 
        
        self.epsilon  = makeVar ( epsilon                     ,
                                  'epsilon_%s'           % name ,
                                  '#epsilon_{shash}(%s)' % name ,
                                  epsilon ,
                                  0       , -20     , +20    ) 
        
        self.delta    = makeVar ( delta                       ,
                                  'delta_%s'            % name ,
                                  '#delta_{shash}(%s) ' % name ,
                                  delta   ,
                                  1       , 1.e-3   , 20     )
        
        logger.debug ('SinhAsinh mu      %s' % self.mu      )
        logger.debug ('SinhAsinh sigma   %s' % self.sigma   )
        logger.debug ('SinhAsinh epsilon %s' % self.epsilon )
        logger.debug ('SinhAsinh delta   %s' % self.delta   )

        self.pdf  = cpp.Analysis.Models.SinhAsinh (
            'shash_%s'    % name ,
            'SinhAsinh(%s)' % name ,
            self.x       ,
            self.mu      ,
            self.sigma   ,
            self.epsilon ,
            self.delta   )

# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
