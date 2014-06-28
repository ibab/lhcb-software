#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id: Fit2DModels.py 173561 2014-06-09 10:04:20Z ibelyaev $
# =============================================================================
## @file Fit2DModels.py
#
#  Set of useful non-factorazable 2D-models to describe background distribtions
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision: 173561 $
#  Last modification $Date: 2014-06-09 12:04:20 +0200 (Mon, 09 Jun 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Set of useful non-factorazable 2D-models to describe background distribtions
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    'PolyPos2D_pdf'   , ## A positive polynomial in 2D  
    'PolyPos2Dsym_pdf', ## A positive symmetric polynomial in 2D
    'PSPol2D_pdf'     , ## Product of phase spaces, modulated with 2D polynomial
    'PSPol2Dsym_pdf'  , ## Symmetric product of phase spaces, modulated with 2D polynomial
    'ExpoPSPol2D_pdf' , ## Exponential times  phase space times positive 2D-polynomial
    'ExpoPol2D_pdf'   , ## Product of exponents times positive 2D-polynomial
    'ExpoPol2Dsym_pdf', ## Symmetric version of above
    )
# =============================================================================
import ROOT, math
from   Ostap.PyRoUts             import cpp 
from   Ostap.FitBasic            import makeVar
# =============================================================================
from   AnalysisPython.Logger     import getLogger
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.Fit2DModels' )
else                       : logger = getLogger ( __name__            )
# =============================================================================
## @class PolyPos2D_pdf
#  positive polynomial in 2D:
#  \f$  f(x,y) = \sum^{i=n}_{i=0}\sum{j=k}_{j=0} a^2_{\ij} B^n_i(x) B^k_j(y) \f$,
#  where \f$ B^n_i(x)\f$ denotes the basic bersntein polynomial 
#  @see Analysis::Models::Poly2DPositive
#  @see Gaudi::Math::Poly2DPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class PolyPos2D_pdf(object) :
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
## @class PolyPos2Dsym_pdf
#  Positive symetric polynomial in 2D:
#  \f$  f(x,y) = \sum^{i=n}_{i=0}\sum{j=n}_{j=0} a^2_{\ij} B^n_i(x) B^n_j(y) \f$,
#  where \f$ B^n_i(x)\f$ denotes the basic bersntein polynomial and
#  \f$a_{ij} = a_{ji}\f$
#  @see Analysis::Models::Poly2DSymPositive
#  @see Gaudi::Math::Poly2DSymPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class PolyPos2Dsym_pdf(object) :
    """
    Positive polynomial in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   n  = 2           ) : ##  polynomial degree 

        if x.getMin() != y.getMin() :
            logger.warning( 'PolyPos2Dsym: x&y have different low  edges %s vs %s'
                            % ( x.getMin() , y.getMin() ) )
        if x.getMax() != y.getMax() :
            logger.warning( 'PolyPos2Dsym: x&y have different high edges %s vs %s'
                            % ( x.getMax() , y.getMax() ) )
        self.x  = x
        self.y  = y
        
        self.m1 = x ## ditto 
        self.m2 = y ## ditto 
        
        self.phis     = []
        self.phi_list = ROOT.RooArgList() 
        
        num = ( n + 1 ) * ( n + 2 ) / 2 
        for i in range ( 1 ,  num ) :
            
            phi_i = makeVar   ( None ,
                                'phi_2ds_%d_%s'      % ( i , name ) ,
                                '#phi(2Dsym)_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            
        #
        ## finally build PDF 
        #
        self.pdf = cpp.Analysis.Models.Poly2DSymPositive (
            'p2Dsp_%s'              % name ,
            'Poly2DSymPositive(%s)' % name ,
            self.x        ,
            self.y        ,
            n             ,
            self.phi_list )


# =============================================================================
## @class PSPol2D_pdf
#  Product of phase space factors, modulated by positiev polynomial in 2D 
#  \f$  f(x,y) = \Phi_1(x) \times \Phi_2(y) \times P^+(x,y) \f$,
#  where \f$ P^+(x,y)\f$ denotes the positive polynomial,
#  @see Analysis::Models::PS2DPol
#  @see Gaudi::Math::PS2DPol
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class PSPol2D_pdf(object) :
    """
    Product of phase space factors, modulated by the positive polynom in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   psx              ,   ##  phase space in X, Gaudi::Math::PhaseSpaceNL 
                   psy              ,   ##  phase space in Y, Gaudi::Math::PhaseSpaceNL 
                   nx  = 2          ,   ##  polynomial degree in X 
                   ny  = 2          ) : ##  polynomial degree in Y 
        
        self.x   = x
        self.y   = y
        
        self.m1  = x ## ditto 
        self.m2  = y ## ditto

        self.psx = psx  
        self.psy = psy
            
            
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
        self.pdf = cpp.Analysis.Models.PS2DPol (
            'ps2D_%s'     % name ,
            'PS2DPol(%s)' % name ,
            self.x        ,
            self.y        ,
            psx           ,
            psy           ,
            nx            ,
            ny            , 
            self.phi_list )
        

# =============================================================================
## @class PSPol2Dsym_pdf
#  Symmetric product of phase space factors, modulated by positiev polynomial in 2D 
#  \f$  f(x,y) = \Phi(x) \times \Phi(y) \times P^+_{sym}(x,y) \f$,
#  where \f$ P^+_{sym}(x,y)\f$ denotes the symmetric positive polynomial,
#  @see Analysis::Models::PS2DPolSym
#  @see Gaudi::Math::PS2DPolSym
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class PSPol2Dsym_pdf(object) :
    """
    Symmetric Product of phase space factors, modulated by the positive polynom in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   ps               ,   ##  phase space in X, Gaudi::Math::PhaseSpaceNL 
                   n   = 2          ) : ##  polynomial degree in Y 
        
        if x.getMin() != y.getMin() :
            logger.warning( 'PSPos2Dsym: x&y have different low  edges %s vs %s'
                            % ( x.getMin() , y.getMin() ) )
            
        if x.getMax() != y.getMax() :
            logger.warning( 'PSPos2Dsym: x&y have different high edges %s vs %s'
                            % ( x.getMax() , y.getMax() ) )
                
        self.x   = x
        self.y   = y
        
        self.m1  = x ## ditto 
        self.m2  = y ## ditto

        self.ps  = ps
        self.psx = ps
        self.psy = ps
            
        self.phis     = []
        self.phi_list = ROOT.RooArgList() 
        
        for i in range ( 1 ,  ( n + 1 ) * ( n + 2 ) / 2 ) :
            
            phi_i = makeVar   ( None ,
                                'phi_2d_%d_%s'    % ( i , name ) ,
                                '#phi(2D)_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            
        #
        ## finally build PDF 
        #
        self.pdf = cpp.Analysis.Models.PS2DPolSym (
            'ps2Ds_%s'       % name ,
            'PS2DPolSym(%s)' % name ,
            self.x        ,
            self.y        ,
            ps            ,
            n             ,
            self.phi_list )
        
# =============================================================================
## @class ExpoPSPol2D_pdf
#  Product of phase space factors, modulated by positiev polynomial in 2D 
#  \f$  f(x,y) = exp(\tau x) \times \Phi (y) \times P^+(x,y) \f$,
#  where \f$ P^+(x,y)\f$ denotes the positive polynomial,
#  @see Analysis::Models::ExpoPS2DPol
#  @see Gaudi::Math::ExpoPS2DPol
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class ExpoPSPol2D_pdf(object) :
    """
    Exnponential times phase space, modulated by the positive polynom in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   psy = None       ,   ##  phase space in Y, Gaudi::Math::PhaseSpaceNL 
                   nx  = 2          ,   ##  polynomial degree in X 
                   ny  = 2          ) : ##  polynomial degree in Y 
        
        self.x   = x
        self.y   = y

        #
        ## get tau
        # 
        change = 1.e+16        
        taumin = math.log ( change ) / ( x.getMax() - x.getMin() ) 
        taumin = -1 * abs ( taumin ) 
        taumax =      abs ( taumin ) 
        
        #
        ## the exponential slope
        #
        self.tau  = makeVar ( None ,
                              "tau_%s"  % name ,
                              "tau(%s)" % name , None , 0 , taumin , taumax )
        #
        
        self.m1  = x ## ditto 
        self.m2  = y ## ditto

        self.psy = psy
                        
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
        self.pdf = cpp.Analysis.Models.ExpoPS2DPol (
            'ps2D_%s'     % name ,
            'PS2DPol(%s)' % name ,
            self.x        ,
            self.y        ,
            self.tau      ,
            self.psy      , 
            nx            ,
            ny            , 
            self.phi_list )
        

# =============================================================================
## @class ExpoPol2D_pdf
#  Product of phase space factors, modulated by positiev polynomial in 2D 
#  \f$  f(x,y) = exp(\tau_x x) \times exp ( \tau_y y) \times P^+(x,y) \f$,
#  where \f$ P^+(x,y)\f$ denotes the positive polynomial,
#  @see Analysis::Models::Expo2DPol
#  @see Gaudi::Math::Expo2DPol
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class ExpoPol2D_pdf(object) :
    """
    Exnponential times phase space, modulated by the positive polynom in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   nx  = 2          ,   ##  polynomial degree in X 
                   ny  = 2          ) : ##  polynomial degree in Y 
        
        self.x   = x
        self.y   = y

        #
        ## get tau_x
        # 
        change = 1.e+16        
        tauxmin = math.log ( change ) / ( x.getMax() - x.getMin() ) 
        tauxmin = -1 * abs ( tauxmin ) 
        tauxmax =      abs ( tauxmin ) 
        tauymin = math.log ( change ) / ( y.getMax() - y.getMin() ) 
        tauymin = -1 * abs ( tauxmin ) 
        tauymax =      abs ( tauxmin ) 
        
        #
        ## the exponential slopes
        #
        self.taux  = makeVar ( None ,
                               "taux_%s"  % name ,
                               "taux(%s)" % name , None , 0 , tauxmin , tauxmax )
        #
        self.tauy  = makeVar ( None ,
                               "tauy_%s"  % name ,
                               "tauy(%s)" % name , None , 0 , tauymin , tauymax )
        #
        
        self.m1  = x ## ditto 
        self.m2  = y ## ditto

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
        self.pdf = cpp.Analysis.Models.Expo2DPol (
            'exp2D_%s'      % name ,
            'Expo2DPol(%s)' % name ,
            self.x        ,
            self.y        ,
            self.taux     ,
            self.tauy     ,
            nx            ,
            ny            , 
            self.phi_list )
        

# =============================================================================
## @class ExpoPol2Dsym_pdf
#  Product of phase space factors, modulated by positiev polynomial in 2D 
#  \f$  f(x,y) = exp(\tau x) \times exp ( \tau y) \times P^+_{sym}(x,y) \f$,
#  where \f$ P^+_{sym}(x,y)\f$ denotes the symmetric positive polynomial,
#  @see Analysis::Models::Expo2DPolSym
#  @see Gaudi::Math::Expo2DPolSym
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-01-10
class ExpoPol2Dsym_pdf(object) :
    """
    Symmetric produict of two exponentials and the positive polynom in 2D 
    """
    def __init__ ( self             ,
                   name             ,
                   x                ,   ##  the first  dimension  
                   y                ,   ##  the second dimension
                   n   = 2          ) : ##  polynomial degree in X and Y 
        
        if x.getMin() != y.getMin() :
            logger.warning( 'PSPos2Dsym: x&y have different low  edges %s vs %s'
                            % ( x.getMin() , y.getMin() ) )
            
        if x.getMax() != y.getMax() :
            logger.warning( 'PSPos2Dsym: x&y have different high edges %s vs %s'
                            % ( x.getMax() , y.getMax() ) )
                
        self.x   = x
        self.y   = y
        
        self.m1  = x ## ditto
        self.m2  = y ## ditto 

        #
        ## get tau_x
        # 
        change = 1.e+16        
        tauxmin = math.log ( change ) / ( x.getMax() - x.getMin() ) 
        tauxmin = -1 * abs ( tauxmin ) 
        tauxmax =      abs ( tauxmin ) 
        #
        ## the exponential slopes
        #
        self.tau  = makeVar ( None ,
                              "tau_%s"  % name ,
                              "tau(%s)" % name , None , 0 , tauxmin , tauxmax )
        #
        self.m1  = x ## ditto 
        self.m2  = y ## ditto

        self.phis     = []
        self.phi_list = ROOT.RooArgList() 
        
        for i in range ( 1 ,  ( n + 1 ) * ( n + 2 ) / 2 ) :
            
            phi_i = makeVar   ( None ,
                                'phi_2d_%d_%s'    % ( i , name ) ,
                                '#phi(2D)_%d(%s)' % ( i , name ) ,  None , 0 , -3.5 , 3.5 )
            self.phis.append  ( phi_i )
            self.phi_list.add ( phi_i )
            
        #
        ## finally build PDF 
        #
        self.pdf = cpp.Analysis.Models.Expo2DPolSym (
            'exp2Ds_%s'        % name ,
            'Expo2DPolSym(%s)' % name ,
            self.x        ,
            self.y        ,
            self.tau      ,
            n             ,
            self.phi_list )
        


# =============================================================================
# some tiny decoration of underlying classes 
# =============================================================================
def _2d_get_pars_ ( self ) :
    """
    Get parameters of underlying positive Berstein polynomial
    """
    if   hasattr ( self , 'pdf'      ) :
        return _2d_get_pars_ ( self.pdf        )        
    elif hasattr ( self , 'function' ) :
        return _2d_get_pars_ ( self.function() )
    elif hasattr ( self , 'positive' ) :
        return _2d_get_pars_ ( self.positive() )
    elif hasattr ( self , 'polynom' ) :
        return _2d_get_pars_ ( self.polynom () )
    elif hasattr ( self , 'bernstein' ) :
        b = self.bernstein()
        m = ROOT.TMatrix ( b.nX() + 1 , b.nY() + 1 )
        for i in range ( 0 , b.nX() + 1 ) :
            for j in range ( 0 , b.nY() + 1 ) :
                m[i][j] = b.par(i,j) 
        return m 
        
    return ROOT.TMatrix()

for t in ( PolyPos2D_pdf    ,
           PolyPos2Dsym_pdf ,
           PSPol2D_pdf      ,
           PSPol2Dsym_pdf   ,
           ExpoPSPol2D_pdf  ,
           ExpoPol2D_pdf    ,
           ExpoPol2Dsym_pdf ) :

    t.pars = _2d_get_pars_ 
    
    
           
# =============================================================================
if '__main__' == __name__ :
         
    import ostapline
    logger.info ( __file__ + '\n' + ostapline.line  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
