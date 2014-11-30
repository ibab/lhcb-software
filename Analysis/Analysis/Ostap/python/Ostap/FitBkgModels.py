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
"""Set of useful smooth 1D-models to describe background distributions"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    'Bkg_pdf'     , ## An exponential function, modulated by positiev polynomial
    'PSPol_pdf'   , ## A phase space  function, modulated by positive polynomial
    'PolyPos_pdf' , ## A positive polynomial
    ##
    'PSpline_pdf' , ## positive                spline 
    'ISpline_pdf' , ## positive non-decreasing spline 
    'DSpline_pdf' , ## positive non-increasing spline 
    ##
    'PS2_pdf'     , ## 2-body phase space (no parameters)
    'PSLeft_pdf'  , ## Low  edge of N-body phase space 
    'PSRight_pdf' , ## High edge of L-body phase space from N-body decays  
    'PSNL_pdf'    , ## L-body phase space from N-body decays  
    'PS23L_pdf'   , ## 2-body phase space from 3-body decays with orbital momenta
    )
# =============================================================================
import ROOT, math
from   Ostap.PyRoUts             import cpp,iszero  
from   Ostap.FitBasic            import makeVar, PDF 
# =============================================================================
from   AnalysisPython.Logger     import getLogger
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.FitBkgModels' )
else                       : logger = getLogger ( __name__             )
# =============================================================================
logger.debug ( __doc__ ) 
# =============================================================================
## @class  Bkg_pdf
#  The exponential modified with the positive polynomial 
#  @see Analysis::Models::ExpoPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Bkg_pdf(PDF) :
    """
    Exponential function, modulated by the positive polynomial
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   power = 0        ,
                   tau   = None     ) : ## degree of polynomial 
        #
        PDF.__init__  ( self , name )
        #                
        self.mass  = mass
        self.power = power
        #
        mn,mx   = mass.minmax()
        mc      = 0.5 * ( mn + mx )
        taumax  = 100
        #
        if not iszero ( mn ) : taumax =                100.0 / abs ( mn ) 
        if not iszero ( mc ) : taumax = min ( taumax , 100.0 / abs ( mc ) )
        if not iszero ( mx ) : taumax = min ( taumax , 100.0 / abs ( mx ) )
        # 
        ## the exponential slope
        #
        self.tau  = makeVar ( tau              ,
                              "tau_%s"  % name ,
                              "tau(%s)" % name , tau , 0 , -taumax, taumax )
        #
        self.phis     = []
        self.phi_list = ROOT.RooArgList ()
        # 
        if 0 >= self.power :
            
            self.pdf = ROOT.RooExponential (
                'exp_%s' % name  , 'exp(%s)' % name , mass , self.tau )
            
        else :
            
            for i in range ( 1 , power + 1 ) :
                #
                phi_i = makeVar   ( None                         ,
                                    'phi%d_%s'      % ( i , name ) ,
                                    '#phi_{%d}(%s)' % ( i , name ) ,  None , 0 , -6.5 , 6.5 )
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
#  Phase space function modulated with the positive polynomial 
#  @see Analysis::Models::PhaseSpacePol
#  @see Gaudi::Math::PhaseSpacePol
#  @see Gaudi::Math::PhaseSpaceNL 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSPol_pdf(PDF) :
    """
    The phase space function modified with positive polynomial 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale 
                   phasespace       ,   ## Gaudi::Math::PhaseSpaceNL 
                   power = 1        ) : ## degree of the polynomial
        
        #
        PDF.__init__  ( self , name )
        #
        self.mass  = mass  
        self.ps    = phasespace  ## Gaudi::Math::PhaseSpaceNL
        self.power = power
        
        # 
        self.phis     = []
        self.phi_list = ROOT.RooArgList () 
        for i in range ( 1 , power + 1 ) :
            #
            phi_i = makeVar   ( None ,
                                'phi%d_%s'      % ( i , name ) ,
                                '#phi_{%d}(%s)' % ( i , name ) ,  None , 0 , -6.5 , 6.5 )
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
## @class  PolyPos_pdf
#  A positive polynomial 
#  @see Analysis::Models::PolyPositive 
#  @see Gaudi::Math::Positive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PolyPos_pdf(PDF) :
    """
    A positive polynomial 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale 
                   power = 1        ) : ## degree of the polynomial
        #
        PDF.__init__ ( self , name )
        #
        self.power = power
        self.mass  = mass 
        
        # 
        self.phis     = []
        self.phi_list = ROOT.RooArgList ()
        for i in range ( 1 , power + 1 ) :
            #
            phi_i = makeVar   ( None ,
                                'phi%d_%s'      % ( i , name ) ,
                                '#phi_{%d}(%s)' % ( i , name ) ,  None , 0 , -6.5 , 6.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            #
            
        self.pdf  = cpp.Analysis.Models.PolyPositive (
            'pp_%s'            % name ,
            'PolyPositive(%s)' % name ,
            self.mass            ,
            self.phi_list        ,
            self.mass.getMin()   ,
            self.mass.getMax()   ) 
        

# =============================================================================
## @class  PSpline_pdf
#  The special spline for non-negative function
#  Actually it is a sum of M-splines with non-negative coefficients 
#  \f$ f(x) = \sum_i \alpha_i * M_i^k(x) \f$,
#  with constraints  \f$  \sum_i \alpha_i=1\f$ and \f$ 0 \le \alpha_i\f$.
#  @see Analysis::Models::PositiveSpline 
#  @see Gaudi::Math::PositiveSpline 
#  @see http://en.wikipedia.org/wiki/M-spline
#  @see http://en.wikipedia.org/wiki/B-spline
#  The constrains are implemented as N-sphere
#  @see Gaudi::Math::NSphere
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSpline_pdf(PDF) :
    """
    A positive spline, a composion of M-splines with non-negative coefficients

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.PositiveSpline( mass.xmin() , mass.xmax() , inner , order )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.PositiveSpline( knots , order )

    >>> bkg = PSpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::PositiveSpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.phis     = []
        self.phi_list = ROOT.RooArgList ()
        for i in range ( 1 , spline.npars() + 1 ) :
            #
            phi_i = makeVar   ( None ,
                                'phi%d_%s'      % ( i , name ) ,
                                '#phi_{%d}(%s)' % ( i , name ) ,  None , 0 , -6.5 , 6.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            #
            
        self.pdf  = cpp.Analysis.Models.PositiveSpline (
            'ps_%s'              % name ,
            'PositiveSpline(%s)' % name ,
            self.mass            ,
            self.spline          , 
            self.phi_list        )

# =============================================================================
## @class  ISpline_pdf
#  The special spline for non-negative non-decreasing function
#  It is a sum of I-splines with non-negative coefficients 
#  \f$ f(x) = \sum_i \alpha_i * I_i^k(x) \f$,
#  that is equivalent to the reparameterized sum of B-splines 
#  \f$ f(x) = \sum_i \alpha^{\prime}_i * B_i^k(x) \f$,
#  with constraints  \f$ 0 \le \alpha^{\prime}_i \le \alpha^{\prime}_{i+1}\f$.
#  @see Analysis::Models::IncreasingSpline 
#  @see Gaudi::Math::IncreasingSpline 
#  @see http://en.wikipedia.org/wiki/I-spline
#  @see http://en.wikipedia.org/wiki/M-spline
#  @see http://en.wikipedia.org/wiki/B-spline
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class ISpline_pdf(PDF) :
    """
    A positive on-decreasing spline, a composion of I-splines with non-negative coefficients

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.IncreasingSpline( mass.xmin() , mass.xmax() , inner , order )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.IncreasingSpline( knots , order )

    >>> bkg = ISpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::IncreasingSpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.phis     = []
        self.phi_list = ROOT.RooArgList ()
        for i in range ( 1 , spline.npars() + 1 ) :
            #
            phi_i = makeVar   ( None ,
                                'phi%d_%s'      % ( i , name ) ,
                                '#phi_{%d}(%s)' % ( i , name ) ,  None , 0 , -6.5 , 6.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            #
            
        self.pdf  = cpp.Analysis.Models.IncreasingSpline (
            'is_%s'              % name ,
            'IncreasingSpline(%s)' % name ,
            self.mass            ,
            self.spline          , 
            self.phi_list        )

# =============================================================================
## @class  DSpline_pdf
#  The special spline for non-negative non-increasing function, 
#  implemented as sum of B-splines 
#  \f$ f(x) = \sum_i \alpha_i * B_i^k(x) \f$
#  with constraints
#  \f$ \alpha^{\prime}_i \ge \alpha^{\prime}_{i+1} \ge 0 \f$.
#  @see Analysis::Models::DecreasingSpline 
#  @see Gaudi::Math::DecreasingSpline 
#  @see http://en.wikipedia.org/wiki/I-spline
#  @see http://en.wikipedia.org/wiki/M-spline
#  @see http://en.wikipedia.org/wiki/B-spline
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class DSpline_pdf(PDF) :
    """
    A positive non-increasing spline as special composition of B-splines

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.DecreasingSpline( mass.xmin() , mass.xmax() , inner , order )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.DecreasingSpline( knots , order )
    
    >>> bkg = ISpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::DecreasingSpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.phis     = []
        self.phi_list = ROOT.RooArgList ()
        for i in range ( 1 , spline.npars() + 1 ) :
            #
            phi_i = makeVar   ( None ,
                                'phi%d_%s'      % ( i , name ) ,
                                '#phi_{%d}(%s)' % ( i , name ) ,  None , 0 , -6.5 , 6.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            #
            
        self.pdf  = cpp.Analysis.Models.DecreasingSpline (
            'is_%s'                % name ,
            'DecreasingSpline(%s)' % name ,
            self.mass                     ,
            self.spline                   , 
            self.phi_list                 )


# =============================================================================
## @class  PS2_pdf
#  Primitive 2-body phase space function 
#  @see Analysis::Models::PhaseSpace2
#  @see Gaudi::Math::PhaseSpace2
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PS2_pdf(PDF) :
    """
    Primitive 2-body phase space function 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale
                   m1               ,   ## the first  mass (constant)
                   m2               ) : ## the second mass (constant)
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        if mass.getMax() < abs ( m1 ) + abs ( m2 ) :
            logger.error('PS2_pdf(%s): senseless setting of edges/threshold' % self.name ) 

        self.mass = mass
        
        self.pdf  = cpp.Analysis.Models.PhaseSpace2 (
            'ps2_%s'          % name ,
            'PhaseSpace2(%s)' % name ,
            self.mass            ,
            m1  , m2 )

# =============================================================================
## @class  PSLeft_pdf
#  Left edge of N-body phase space
#  @see Analysis::Models::PhaseSpaceLeft
#  @see Gaudi::Math::PhaseSpaceLeft
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSLeft_pdf(PDF) :
    """
    Left edge of B-body phase space function 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   N                ,   ## N 
                   left   = None    ) : 
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass = mass
        self.left = makeVar ( left                ,
                              'left_%s'    % name ,
                              'm_left(%s)' % name ,
                              None , mass.getMin() , mass.getMax() )

        if self.left.getMin() >= self.mass.getMax() :
            logger.error('PSLeft_pdf(%s): invalid setting!' % name )
            
        self.pdf  = cpp.Analysis.Models.PhaseSpaceLeft (
            'psl_%s'             % name ,
            'PhaseSpaceLeft(%s)' % name ,
            self.mass ,
            self.left ,
            N         ) 

# =============================================================================
## @class  PSRight_pdf
#  Right edge of L-body phase space fro N-body decay 
#  @see Analysis::Models::PhaseSpaceRight
#  @see Gaudi::Math::PhaseSpaceRight
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSRight_pdf(PDF) :
    """
    Right edge of L-body phase space for N-body decay 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   L                ,   ## L  
                   N                ,  ## N
                   right   = None   ) : 
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass  = mass
        self.right = makeVar ( right ,
                               'right_%s'      % name ,
                               'm_{right}(%s)' % name ,
                               None , mass.getMin() , mass.getMax() )
        
        if self.right.getMax() <= self.mass.getMax() :
            logger.error('PSRight_pdf(%s): invalid setting!' % name )
            
        self.pdf  = cpp.Analysis.Models.PhaseSpaceRight (
            'psr_%s'              % name ,
            'PhaseSpaceRight(%s)' % name ,
            self.mass  ,
            self.right ,
            L          , 
            N          ) 

# =============================================================================
## @class  PSNL_pdf
#  L-body phase space from N-body decay 
#  @see Analysis::Models::PhaseSpaceNL
#  @see Gaudi::Math::PhaseSpaceNL
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSNL_pdf(PDF) :
    """
    L-body phase space from N-body decay 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable 
                   L                ,   ## L  
                   N                ,   ## N
                   left  = None     , 
                   right = None     ) : 
        ##
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass  = mass
        #
        mmin = mass.getMin()
        mmax = mass.getMax()
        #
        self.left  = makeVar ( left ,
                               'left_%s'        % name ,
                               'm_{left}(%s)'   % name , left  , 
                               0.9 * mmin + 0.1 * mmax ,
                               mmin ,
                               mmax ) 
        
        self.right = makeVar ( right ,
                               'right_%s'       % name ,
                               'm_{right}(%s)'  % name , right , 
                               0.1 * mmin + 0.9 * mmax ,
                               mmin ,
                               mmax ) 
        
        if self.left.getMin()  >= self.mass.getMax() :
            logger.error('PSNL_pdf(%s): invalid setting!' % name )
            
        if self.right.getMax() <= self.mass.getMax() :
            logger.error('PSNL_pdf(%):  invalid setting!' % name )

        self.pdf  = cpp.Analysis.Models.PhaseSpaceNL (
            'psnl_%s'          % name ,
            'PhaseSpaceNL(%s)' % name ,
            self.mass  ,
            self.left  ,
            self.right ,
            L          , 
            N          ) 

# =============================================================================
## @class  PS23L_pdf
#  2-body phase space from 3-body decay with orbital momenta 
#  @see Analysis::Models::PhaseSpace23L
#  @see Gaudi::Math::PhaseSpace23L
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PS23L_pdf(PDF) :
    """
    2-body phase space from 3-body decay with orbital momenta 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   m1               ,   ## mass the first particle  (const)
                   m2               ,   ## mass the second particle (const)
                   m3               ,   ## mass the third particle  (const)
                   m                ,   ## mass of the whole system (const)
                   L                ,   ## orbital momenutm between (1,2) and 3
                   l  = 0           ) : ## orbital momentum between 1 and 2

        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass = mass        
        self.pdf  = cpp.Analysis.Models.PhaseSpaceNL (
            'ps23l_%s'          % name ,
            'PhaseSpace23L(%s)' % name ,
            self.mass  ,
            m1 , m2 , m3 , m , L , l )
        
# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*'   )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
