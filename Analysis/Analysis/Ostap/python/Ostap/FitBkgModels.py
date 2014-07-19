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
    'PS2_pdf'     , ## 2-body phase space (no parameters)
    'PSLeft_pdf'  , ## Low  edge of N-body phase space 
    'PSRight_pdf' , ## High edge of L-body phase space from N-body decays  
    'PSNL_pdf'    , ## L-body phase space from N-body decays  
    'PS23L_pdf'   , ## 2-body phase space from 3-body decays with orbital momenta
    )
# =============================================================================
import ROOT, math
from   Ostap.PyRoUts             import cpp 
from   Ostap.FitBasic            import makeVar, Fit1DBase 
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
class Bkg_pdf(Fit1DBase) :
    """
    Exponential function, modulated by the positive polynomial
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   power = 0        ) : ## degree of polynomial 
        #
        self.mass  = mass
        self.power = power
        # 
        change = 1.e+16        
        taumin = math.log ( change ) / ( mass.getMax() - mass.getMin() ) 
        taumin = -1 * abs ( taumin ) 
        taumax =      abs ( taumin ) 
        #
        ## the exponential slope
        #
        self.tau  = makeVar ( None ,
                              "tau_%s"  % name ,
                              "tau(%s)" % name , None , 0 , taumin , taumax )
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
                                    'phi%d_%s'    % ( i , name ) ,
                                    '#phi_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
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
        
        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 

# =============================================================================
## @class  PSPol_pdf
#  Phase space function modulated with the positive polynomial 
#  @see Analysis::Models::PhaseSpacePol
#  @see Gaudi::Math::PhaseSpacePol
#  @see Gaudi::Math::PhaseSpaceNL 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSPol_pdf(Fit1DBase) :
    """
    The phase space function modified with positive polynomial 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale 
                   phasespace       ,   ## Gaudi::Math::PhaseSapceNL 
                   power = 1        ) : ## degree of the polynomial
        
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
                                'phi%d_%s'        % ( i , name ) ,
                                '#phi_PSP_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            #
            
        self.pdf  = cpp.Analysis.Models.PhaseSpacePol (
            'pspol_%s'          % name ,
            'PhaseSpacePol(%s)' % name ,
            self.mass            ,
            self.ps              ,  ## Gaudi::Math::PhaseSpaceNL 
            self.phi_list        )
        
        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 

# =============================================================================
## @class  PolyPos_pdf
#  A positive polynomial 
#  @see Analysis::Models::PolyPositive 
#  @see Gaudi::Math::Positive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PolyPos_pdf(Fit1DBase) :
    """
    A positive polynomial 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale 
                   power = 1        ) : ## degree of the polynomial
        
 
        self.power = power
        self.mass  = mass 
        
        # 
        self.phis     = []
        self.phi_list = ROOT.RooArgList ()
        for i in range ( 1 , power + 1 ) :
            #
            phi_i = makeVar   ( None ,
                                'phi%d_%s'        % ( i , name ) ,
                                '#phi_PSP_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
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
        
        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 
        
# =============================================================================
## @class  PS2_pdf
#  Primitive 2-body phase space function 
#  @see Analysis::Models::PhaseSpace2
#  @see Gaudi::Math::PhaseSpace2
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PS2_pdf(Fit1DBase) :
    """
    Primitive 2-body phase space function 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale
                   m1               ,   ## the first  mass (constant)
                   m2               ) : ## the second mass (constant)
        ##
        if mass.getMax() < abs ( m1 ) + abs ( m2 ) :
            logger.warning('PS2_pdf: senseless setting of edges/thereshold') 

        self.mass = mass
        
        self.pdf  = cpp.Analysis.Models.PhaseSpace2 (
            'ps2_%s'          % name ,
            'PhaseSpace2(%s)' % name ,
            self.mass            ,
            m1  , m2 )
        
        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 

# =============================================================================
## @class  PSLeft_pdf
#  Left edge of N-body phase space
#  @see Analysis::Models::PhaseSpaceLeft
#  @see Gaudi::Math::PhaseSpaceLeft
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSLeft_pdf(Fit1DBase) :
    """
    Left edge of B-body phase space function 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   N                ,   ## N 
                   left   = None    ) : 
        ##
        
        self.mass = mass
        self.left = makeVar ( left                ,
                              'left_%s'    % name ,
                              'm_left(%s)' % name ,
                              None , mass.getMin() , mass.getMax() )

        if self.left.getMin() >= self.mass.getMax() :
            logger.warning('PSLeft_pdf: invalid setting!' )
            
        self.pdf  = cpp.Analysis.Models.PhaseSpaceLEft (
            'psl_%s'             % name ,
            'PhaseSpaceLeft(%s)' % name ,
            self.mass ,
            self.left ,
            N         ) 

        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 

# =============================================================================
## @class  PSRight_pdf
#  Right edge of L-body phase space fro N-body decay 
#  @see Analysis::Models::PhaseSpaceRight
#  @see Gaudi::Math::PhaseSpaceRight
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSRight_pdf(Fit1DBase) :
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
        ##

        self.mass  = mass
        self.right = makeVar ( right ,
                               'right_%s'      % name ,
                               'm_{right}(%s)' % name ,
                               None , mass.getMin() , mass.getMax() )
        
        if self.right.getMax() <= self.mass.getMax() :
            logger.warning('PSRight_pdf: invalid setting!' )
            
        self.pdf  = cpp.Analysis.Models.PhaseSpaceRight (
            'psr_%s'             % name ,
            'PhaseSpaceRight(%s)' % name ,
            self.mass  ,
            self.right ,
            L          , 
            N          ) 
    
        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 

# =============================================================================
## @class  PSNL_pdf
#  L-body phase space from N-body decay 
#  @see Analysis::Models::PhaseSpaceNL
#  @see Gaudi::Math::PhaseSpaceNL
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSNL_pdf(Fit1DBase) :
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
        
        self.mass  = mass
        self.left  = makeVar ( left ,
                               'left_%s'      % name ,
                               'm_{left}(%s)' % name ,
                               None , mass.getMin() , mass.getMax() )
        self.right = makeVar ( right ,
                               'right_%s'      % name ,
                               'm_{right}(%s)' % name ,
                               None , mass.getMin() , mass.getMax() )
        
        if self.left.getMin() >= self.mass.getMax() :
            logger.warning('PSNL_pdf: invalid setting!' )
            
        if self.right.getMax() <= self.mass.getMax() :
            logger.warning('PSNL_pdf: invalid setting!' )

        self.pdf  = cpp.Analysis.Models.PhaseSpaceNL (
            'psnl_%s'          % name ,
            'PhaseSpaceNL(%s)' % name ,
            self.mass  ,
            self.left  ,
            self.right ,
            L          , 
            N          ) 

        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 

# =============================================================================
## @class  PS23L_pdf
#  2-body phase space from 3-body decay with orbital momenta 
#  @see Analysis::Models::PhaseSpace23L
#  @see Gaudi::Math::PhaseSpace23L
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PS23L_pdf(Fit1DBase) :
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


        self.mass = mass        
        self.pdf  = cpp.Analysis.Models.PhaseSpaceNL (
            'ps23l_%s'          % name ,
            'PhaseSpace23L(%s)' % name ,
            self.mass  ,
            m1 , m2 , m3 , m , L , l )

        Fit1DBase.__init__ ( self, ROOT.RooArgSet ( self.pdf ) , ROOT.RooArgSet() ) 
        
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
