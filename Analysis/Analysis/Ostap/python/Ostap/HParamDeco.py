#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with utilities for parameterization of historgams 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Module with utilities for parameterization of historgams"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT, cppyy              ## attention here!!
cpp = cppyy.makeNamespace('')
VE  = cpp.Gaudi.Math.ValueWithError 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.HParamDeco' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'Some parameterization utilities for Histo objects')
# =============================================================================
## @class H_fit
#  simple function to fit/represent the histogram with bernstein/spline
#  expansion 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-09
class H_fit(object) :
    """
    Simple function to fit/represent the histogram with sum of
    bernstein/b-spline functions 
    """
    def __init__ ( self ,  hfit ) :
        self._hfit = hfit
        from Ostap.PyRoUts import funID
        self.fun = ROOT.TF1 ( funID() , self , hfit.xmin() , hfit.xmax() , hfit.npars() )
    #
    def norm     ( self ) : return False 
    def npars    ( self ) : return self._hfit.npars () 
    def pars     ( self ) : return self._hfit.pars  ()
    #
    def draw     ( self , *args ) : return sef.fun.Draw( *args ) 
    def Draw     ( self , *args ) : return sef.fun.Draw( *args )
    
    def fit      ( self , h , opts = 'S' , *args ) : return h.Fit( self.fun , opts , *args ) 
    def Fit      ( self , h , opts = 'S' , *args ) : return h.Fit( self.fun , opts , *args ) 
    #   
    ## the major method 
    def __call__ ( self , x , pars = [] ) :
        
        x0 = x if isinstance ( x ,  ( int , long , float ) ) else x[0]
        if pars :
            np = self._hfit.npars() 
            for i in range ( 0 , np ) :    
                self._hfit.setPar ( i , pars[i] )
                
        return self._hfit( x0 )
    
# =============================================================================
## @class H_nfit
#  simple function to fit/represent the histogram with normalized
#  functions, in particular:
#  - positive bernstein polynomial,
#  - positive B-spline expansion 
#  - positive monothoni B-spline expansion 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-09
class H_Nfit (object) :
    """
    Simple function to fit/represent the histogram with sum of bernstein positive polynominals
    """
    def __init__ ( self , hfit ) :
        self._hfit = hfit
        from Ostap.PyRoUts import funID
        self.fun   = ROOT.TF1 ( funID() , self , hfit.xmin() , hfit.xmax() , hfit.npars() + 1 )
        self.fun.SetParameter ( 0 , 1 ) 
        
    def norm     ( self ) : return True  
    def npars    ( self ) : return self._hfit.npars () + 1  ## NB: normalization!  
    def pars     ( self ) : return self._hfit.pars  ()
    
    def draw     ( self , *args ) : return sef.fun.Draw( *args ) 
    def Draw     ( self , *args ) : return sef.fun.Draw( *args )
    
    def fit      ( self , h , opts = 'S' , *args ) : return h.Fit( self.fun , opts , *args ) 
    def Fit      ( self , h , opts = 'S' , *args ) : return h.Fit( self.fun , opts , *args ) 

    ## the major method 
    def __call__ ( self , x , pars = [] ) :

        norm = 1.0
        x0 = x if isinstance ( x ,  ( int , long , float ) ) else x[0]
        
        if pars :

            norm = pars[0]
            
            np   = self._hfit.npars() 
            for i in range ( 0 , np ) :    
                self._hfit.setPar ( i , pars[i+1] )
                
        return norm * self._hfit( x0 )
    
# =============================================================================
## represent 1D-histo as polynomial sum 
def _h1_param_sum_ ( h1             ,
                     fun_obj        ,
                     fit_type       ,  
                     opts  = 'SQ0I' ) :
    """
    Represent histo as polynomial sum    
    """
    ## 
    b     = fun_obj  
    #
    bfit  = fit_type ( b )    
    bfit.fun.SetNpx  ( max ( 100 , 3 * h1.bins() ) )  
    
    bfit.histo     = h1
    
    if bfit.norm() : 
        mn,mx = h1.xminmax ()    
        mi    = h1.accumulate().value()*(mn-mx)/h1.bins() 
        bfit.fun.SetParameter ( 0 , mi )
        from math import pi 
        for i in range( 0, b.npars() ) :
            bfit.fun.SetParameter ( i + 1 , 0  )
            bfit.fun.SetParLimits ( i + 1 , -0.6 * pi  , 1.1 * pi ) 
            
    if 0 > opts.upper().find('S') : opts += 'S '

    fun = bfit.fun

    if bfit.norm() :
        mn,mx = h1.xminmax ()    
        mi    = h1.accumulate().value()*(mn-mx)/h1.bins() 
        fun.FixParameter    (0,mi)
        r = fun.Fit(h1,opts+'0Q')
        fun.ReleaseParameter(0)
        
    r = fun.Fit( h1 , opts )
        
    if 0 != r.Status() :
        logger.info ( 'Fit status is  %d [%s]' % ( r.Status() , type(b).__name__ ) )
        if bfit.norm() :
            fun.FixParameter ( 0 , mi ) 
            for i in range ( 0 , b.npars() ) :
                fun.SetParameter ( i + 1 , 0  )
            r = fun.Fit(h1,opts)
            fun.ReleaseParameter( 0)
        r = fun.Fit(h1,opts)
            
    if 0 != r.Status() :
        logger.warning ( 'Fit status is  %d [%s]' % ( r.Status() , type(b).__name__ ) )
        r = fun.Fit(h1,opts)        
        if bfit.norm() :
            mn,mx = h1.xminmax ()    
            mi    = h1.accumulate().value()*(mn-mx)/h1.bins() 
            fun.FixParameter ( 0 , mi )
            for i in range( 0 , b.npars() ) :
                fun.SetParameter ( i + 1 , 0  )
            r = fun.Fit(h1,opts)
            fun.ReleaseParameter( 0)
        r = fun.Fit ( h1 , opts )
        if 0 != r.Status() :
            logger.error ('Fit status is  %d [%s]' % ( r.Status() , type(b).__name__ ) )
            
                
    bfit.fitresult = r 
    return bfit.fun , bfit , b , bfit.fitresult 


# =============================================================================
## represent 1D-histo as Bernstein polynomial
def _h1_bernstein_ ( h1 , degree , interpolate = True , opts = 'SQ0' ) :
    """
    Represent histo as Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.bernstein ( 5 )  ## make a fit... 

    >>> tfun       = r[0]  ## get TH1 object
    >>> tfun.Draw()

    Underlying C++ object:
    >>> fun        = b[2]
    >>> print fun.pars() 

    ## fit result and status 
    >>> fit_result = r[3]
    >>> print fit_result.CovMatrix(1,1)
    """
    #
    mn,mx = h1.xminmax ()    
    func = cpp.Gaudi.Math.Bernstein ( degree , mn , mx ) 
    #
    ## make the approximation for bernstein coefficients
    #
    for i in range ( 0 , degree + 1 ) :
        x    = mn + ( mx - mn ) * float( i ) / degree
        v    = h1 ( x , interpolate = interpolate ).value()
        func.setPar ( i , v )
        
    func.setPar ( 0      , h1[ 1         ].value() )
    func.setPar ( degree , h1[ h1.bins() ].value() )

    return _h1_param_sum_ ( h1 , func , H_fit , opts )  


# =============================================================================
## represent 1D-histo as Chebyshev polynomial
def _h1_chebyshev_ ( h1 , degree , interpolate = True , opts = 'SQ0I' ) :
    """
    Represent histo as Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.bernstein ( 5 )  ## make a fit... 
    
    >>> tfun       = r[0]  ## get TH1 object
    >>> tfun.Draw()

    Underlying C++ object:
    >>> fun        = b[2]
    >>> print fun.pars() 

    ## fit result and status 
    >>> fit_result = r[3]
    >>> print fit_result.CovMatrix(1,1)
    """
    #
    mn,mx = h1.xminmax ()    
    func  = cpp.Gaudi.Math.ChebyshevSum ( degree , mn , mx ) 
    #
    ## make reasonable approximation
    import math 
    N  = func.npars()  
    for j in range ( 0 , N ) :

        cj = 0 
        for k in range(1,N+1)  :
            qk  = math.pi * j * ( k - 0.5 ) / N
            ck  = math.cos ( qk )
            tk  = math.pi *     ( k - 0.5 ) / N 
            tk  = math.cos ( tk )
            xk  = func.x   ( tk )
            fk  = h1 ( xk ) 
            cj += fk * ck
            
        if 0 == j : cj /=       N  
        else      : cj *= 2.0 / N 
        
        func.setPar ( j , cj ) 
        
    return _h1_param_sum_ ( h1 , func , H_fit , opts )  


# =============================================================================
## represent 1D-histo as Legendre polynomial
def _h1_legendre_ ( h1 , degree , interpolate = True , opts = 'SQ0' ) :
    """
    Represent histo as Legendre polynomial
    
    >>> h = ... # the historgam
    >>> r = h.legendre ( 5 )  ## make a fit...

    
    >>> tfun       = r[0]  ## get TH1 object
    >>> tfun.Draw()

    Underlying C++ object:
    >>> fun        = b[2]
    >>> print fun.pars() 

    ## fit result and status 
    >>> fit_result = r[3]
    >>> print fit_result.CovMatrix(1,1)
    
    """
    mn,mx = h1.xminmax ()    
    func  = cpp.Gaudi.Math.LegendreSum ( degree , mn , mx ) 
    #
    ## make some reasonable approximation
    #
    _b    = 1.0 / h1.bins() 
    _f    = cpp.Gaudi.Math.LegendreSum ( degree , mn , mx )
    for i in range ( 0 , _f.npars()  ) :
        _f  .setPar ( i ,  1 )
        _h  = h1 * _f 
        _c  = _h.accumulate().value()*(2*i+1) * _b 
        _f  .setPar ( i ,  0 ) 
        func.setPar ( i , _c )
        del _h

    del _f
    
    return _h1_param_sum_ ( h1 , func , H_fit , opts )  


# =============================================================================
## represent 1D-histo as plain vanilla polynomial
def _h1_polinomial_ ( h1 , degree , interpolate = True , opts = 'SQ0' ) :
    """
    Represent histo as plain vanilla polynomial
    
    >>> h = ... # the historgam
    >>> b = h.polinomial ( 5 )  ## make a fit... 

    >>> tfun       = r[0]  ## get TH1 object
    >>> tfun.Draw()

    Underlying C++ object:
    >>> fun        = b[2]
    >>> print fun.pars() 

    ## fit result and status 
    >>> fit_result = r[3]
    >>> print fit_result.CovMatrix(1,1)
    """
    mn,mx = h1.xminmax ()    
    func  = cpp.Gaudi.Math.Polynomial ( degree , mn , mx ) 
    #
    my = h1.accumulate().value()/h1.bins()
    func.setPar( 0, my )
    ##
    return _h1_param_sum_ ( h1 , func , H_fit , opts )  


# =============================================================================
## represent 1D-histo as B-spline
def _h1_bspline_ ( h1 , degree = 3 , knots = 3 , opts = 'SQ0' ) :
    """
    Represent histo as B-spline polynomial
    
    >>> h = ... # the historgam
    >>> b1 = h.bSpline ( degree = 3 , innerknots = 3  )
    >>> b2 = h.bSpline ( degree = 3 , innerknots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        func = cpp.Gaudi.Math.BSpline ( mn , mx , knots , degree )
    else :
        from LHCbMath.Types import doubles
        _knots = doubles ( mn , mx ) 
        for k in knots : _knots.push_back( k )
        func = cpp.Gaudi.Math.BSpline ( _knots , degree )
        
    ##
    return _h1_param_sum_ ( h1 , func , H_fit , opts )  


# =============================================================================
## represent 1D-histo as POSITIVE bernstein polynomial
def _h1_positive_ ( h1 , N , opts = 'SQ0' ) :
    """
    Represent histo as Positive Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.positive ( 5 )
    
    """
    mn,mx = h1.xminmax ()
    func  = cpp.Gaudi.Math.Positive ( N , mn , mx )
    # 
    return _h1_param_sum_ ( h1 , func , H_Nfit , opts ) 


# =============================================================================
## represent 1D-histo as MONOTHONIC bernstein polynomial
def _h1_monothonic_ ( h1 , N , increasing = True , opts = 'SQ0' ) :
    """
    Represent histo as Monothonic Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.monothonic ( 5 , increasing = True )
    
    """
    mn,mx = h1.xminmax ()
    func  = cpp.Gaudi.Math.Monothonic ( N , mn , mx , increasing )
    # 
    return _h1_param_sum_ ( h1 , func , H_Nfit , opts ) 


# =============================================================================
## represent 1D-histo as MONOTHONIC CONVEX/CONCAVE bernstein polynomial
def _h1_convex_ ( h1 , N , increasing = True , convex = True , opts = 'SQ0' ) :
    """
    Represent histo as Monothonic Convex/Concave  Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.convex ( 5 , increasing = True , convex = False )
    
    """
    mn,mx = h1.xminmax ()
    func  = cpp.Gaudi.Math.Convex ( N , mn , mx , increasing , convex )
    # 
    return _h1_param_sum_ ( h1 , func , H_Nfit , opts ) 



# =============================================================================
## represent 1D-histo as positive B-spline
def _h1_pspline_ ( h1 , degree = 3 , knots = 3 , opts = 'SQ0I' ) :
    """
    Represent histo as positive B-spline 
    
    >>> h = ... # the historgam
    >>> b1 = h.pSpline ( degree = 3 , knots = 3  )
    >>> b2 = h.pSpline ( degree = 3 , knots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        func = cpp.Gaudi.Math.PositiveSpline ( mn , mx , knots , degree  )
    else :
        from LHCbMath.Types import doubles
        _knots = doubles ( mn , mx ) 
        for k in knots : _knots.push_back( k )
        func = cpp.Gaudi.Math.PositiveSpline ( _knots , degree )
    #
    return _h1_param_sum_ ( h1 , func , H_Nfit , opts ) 

# =============================================================================
## represent 1D-histo as positive monothonic spline
def _h1_mspline_ ( h1 , degree = 3 , knots = 3 , increasing = True , opts = 'SQ0I' ) :
    """
    Represent histo as positive monothonic  spline 
    
    >>> h = ... # the historgam
    >>> b1 = h.mSpline ( degree = 3 , knots = 3  , increasing = True  )
    >>> b2 = h.mSpline ( degree = 3 , knots = [ 0.1 , 0.2, 0.8, 0.9 ] )
    >>> b3 = h.mSpline ( degree = 3 , knots = 3  , increasing = False )
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        func = cpp.Gaudi.Math.MonothonicSpline ( mn , mx , knots , degree , increasing )
    else :
        from LHCbMath.Types import doubles
        _knots = doubles ( mn , mx ) 
        for k in knots : _knots.push_back( k )
        func = cpp.Gaudi.Math.MonothonicSpline ( knots , degree , increasing )
    #
    return _h1_param_sum_ ( h1 , func , H_Nfit , opts ) 

# =============================================================================
## represent 1D-histo as monothonic convex/concave spline
def _h1_cspline_ ( h1 , degree = 3   , knots = 3 ,
                   increasing = True ,
                   convex     = True , 
                   opts       = 'SQ0I' ) :
    """
    Represent histo as positive monothonic convex/concave spline  
    
    >>> h = ... # the historgam
    >>> b1 = h.cSpline ( degree = 3 , knots = 3  , increasing = True , convex = False )
    >>> b2 = h.cSpline ( degree = 3 , knots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        func = cpp.Gaudi.Math.ConvexSpline ( mn , mx , knots , degree , increasing , convex  )
    else :
        from LHCbMath.Types import doubles
        _knots = doubles ( mn , mx ) 
        for k in knots : _knots.push_back( k )
        func   = cpp.Gaudi.Math.ConvexSpline ( _knots , order , increasing , convex )
        
    return _h1_param_sum_ ( h1 , func , H_Nfit , opts ) 


for t in ( ROOT.TH1D , ROOT.TH1F ) :
    t.bernstein  = _h1_bernstein_
    t.chebyshev  = _h1_chebyshev_
    t.legendre   = _h1_legendre_
    t.polynomial = _h1_polinomial_
    t.positive   = _h1_positive_
    t.monothonic = _h1_monothonic_
    t.convex     = _h1_convex_
    t.bSpline    = _h1_bspline_
    t.pSpline    = _h1_pspline_
    t.mSpline    = _h1_mspline_
    t.cSpline    = _h1_cspline_

    
## create function object 
def  _funobj0_ ( self ) :
    """
    """
    if hasattr ( self , '_bfit' ) : return self._bfit
    self._bfit = H_fit( self )
    return self._bfit

## create function object 
def  _funobjN_ ( self ) :
    """
    """
    if hasattr ( self , '_bfit' ) : return self._bfit
    self._bfit = H_Nfit( self )
    return self._bfit

## draw spline object
def _sp_draw_   ( self , opts = '' ) :
    """
    Draw spline object 
    """
    bf = self.funobj () 
    return bf.fun.Draw( opts ) 
    
cpp.Gaudi.Math.Bernstein        .funobj = _funobj0_
cpp.Gaudi.Math.ChebyshevSum     .funobj = _funobj0_
cpp.Gaudi.Math.LegendreSum      .funobj = _funobj0_
cpp.Gaudi.Math.Polynomial       .funobj = _funobj0_
cpp.Gaudi.Math.BSpline          .funobj = _funobj0_
cpp.Gaudi.Math.Positive         .funobj = _funobjN_
cpp.Gaudi.Math.PositiveSpline   .funobj = _funobjN_
cpp.Gaudi.Math.MonothonicSpline .funobj = _funobjN_
cpp.Gaudi.Math.ConvexSpline     .funobj = _funobjN_

# =============================================================================
## helper class to wrap 1D-histogram as function
#  Optionally  normalization, bias and scale are applied
#  Seful e.g. for using a histogram as function fitting 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
class H1Func(object) :
    """
    Helper class to wrap 1D-histogram as function

    >>> histo =
    >>> func  = H1Func ( histo )
    >>> tf1   = ROOT.TF1('f1', func , low , high , 3 )

    Three parameters: NORM, BIAS and SCALE
    f( x )  = NORM  * histo (  ( x - BIAS ) / scale ) 
    
    >>> histo2 = ...
    >>> histo2.Fit ( tf1 , 'S' )
    
    """
    def __init__ ( self , histo , func = lambda s : s.value() ) :
        self._histo = histo
        self._func  = func
        
    ## evaluate the function 
    def __call__ ( self , x , par = [ 1 , 0 , 1 ] ) :
        """
        Evaluate the function 
        """
        #
        x0 = x if isinstance ( x , (int,long,float) ) else x[0]
        #
        norm  = float ( par[0] )   ## NORM 
        bias  = float ( par[1] )   ## BIAS 
        scale = float ( par[2] )   ## SCALE 
        #
        x0    = ( x0 - bias ) / scale
        # 
        return norm * self._func ( self._histo ( x0 , interpolate = True ) )

    ## get corresponsing ROOT.TF1 object 
    def tf1  ( self ) :
        """
        Get corresponsing ROOT.TF1 object 
        """
        if not hasattr ( self , '_tf1' ) : 
            
            from Ostap.PyRoUts import funID
            mn = self._histo.xmin ()
            mx = self._histo.xmax ()
            self._tf1 =  ROOT.TF1 ( funID() , self , mn , mx , 3 )
            self._tf1.SetParNames (
                'Normalization' ,
                'Bias'          ,
                'Scale'
                )
            self._tf1.FixParameter ( 0 , 1 )
            self._tf1.FixParameter ( 1 , 0 )
            self._tf1.FixParameter ( 2 , 1 )

        return self._tf1

    def Draw ( self , *args ) : return self.draw ( *args ) 
    def draw ( self , *args ) :
        t = self.tf1()
        return t.Draw( *args )

# =============================================================================
## helper class to wrap 1D-histogram as spline 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-27
class H1Spline(object) :
    """
    Helper class to Wrap 1D-histogram as function/spline

    >>> histo =
    >>> func  = H1Spline ( histo )
    >>> tf1   = ROOT.TF1('f1', func , low , high , 1 )

    Parameters: NORM, BIAS and SCALE
    f( x)  = NORM  * spline ( ( x - BIAS ) / SCALE ) 
    
    >>> histo2 = ...
    >>> histo2.Fit ( tf1 , 'S' )
    
    
    """
    def __init__ ( self , histo , func = lambda s : s.value() , *args ) :
        """
        *args are transferred to Gaudi::Math::Spline
        """
        ## create the actual spline: 
        ## self._spline = histo.splineErr ( *args )
        self._spline = histo.spline ( *args )
        self._histo  = self._spline ## ATTENTION!!! 
        self._func   = func        
        
    ## evaluate the function 
    def __call__ ( self , x , par = [ 1 , 0 , 1 ] ) :
        """
        Evaluate the function 
        """
        #
        x0 = x if isinstance ( x , (int,long,float) ) else x[0]
        #
        norm  = float ( par[0] )  ## NORM 
        bias  = float ( par[1] )  ## BIAS 
        scale = float ( par[2] )  ## SCALE 
        #
        x0    = ( x0 - bias ) / scale
        #
        return norm * self._func ( VE( self._spline ( x0 ) , 0 ) ) 

    ## get corresponsing ROOT.TF1 object 
    def tf1  ( self ) :
        """
        Get corresponsing ROOT.TF1 object 
        """
        if not hasattr ( self , '_tf1' ) : 
            
            from Ostap.PyRoUts import funID
            #mn = self._spline.spline().xmin ()
            #mx = self._spline.spline().xmax ()
            mn = self._spline.xmin ()
            mx = self._spline.xmax ()
            self._tf1 =  ROOT.TF1 ( funID() , self , mn , mx , 3 )
            self._tf1.SetParNames (
                'Normalization' ,
                'Bias'          ,
                'Scale'
                )
            self._tf1.FixParameter ( 0 , 1 )
            self._tf1.FixParameter ( 1 , 0 )
            self._tf1.FixParameter ( 2 , 1 )

        return self._tf1

    def Draw ( self , *args ) : return self.draw ( *args ) 
    def draw ( self , *args ) :
        t = self.tf1()
        return t.Draw( *args )
    

# ==============================================================================
## helper class to wrap 2D-histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
class H2Func(object) :
    """
    Helper class to Wrap 2D-histogram as function 
    """
    def __init__ ( self , histo , func = lambda s : s.value() ) :
        self._histo = histo    
        self._func  = func
        
    ## evaluate the function 
    def __call__ ( self , x ) :
        """
        Evaluate the function 
        """
        x0 = x[0]
        y0 = x[1]
        return self._func ( self._histo ( x0 , y0 , interpolate = True ) )

# =============================================================================
## construct helper class 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_as_fun_ ( self , func = lambda s : s.value () ) :
    """
    construct the function from the histogram
    """
    return H1Func ( self , func )

# =============================================================================
## construct helper class 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-27
def _h1_as_spline_ ( self , func = lambda s : s.value () , *args ) :
    """
    construct the function/spline from the histogram 
    """
    return H1Spline ( self , func , *args )

# =============================================================================
## construct helper class 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_as_fun_ ( self , func = lambda s : s.value () ) :
    """
    construct the function from the histogram 
    """
    return H2Func ( self , func )
# =============================================================================
## construct function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_as_tf1_ ( self , func = lambda s : s.value () , spline = False , *args ) :
    """
    Construct the function from the 1D-histogram

    >>> histo = ...
    >>> fun1  = histo.asTF1 ( spline = False )
    >>> fun2  = histo.asTF1 ( spline = True  ) 
    """
    #
    if spline : fun = _h1_as_spline_ ( self , func , *args )
    else      : fun = _h1_as_fun_    ( self , func )
    #
    f1 = fun .tf1  ()
    nb = self.nbins()
    
    if f1.GetNpx() <  1.2 * nb :
        f1.SetNpx ( max ( 100 , 10 * nb ) ) 
    
    return f1 
    
# =============================================================================
## construct function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_as_tf2_ ( self , func = lambda s : s.value () ) :
    """
    Construct the function from the histogram

    >>> fun = h2.asFunc()
    
    """
    ax  = self.GetXaxis()
    ay  = self.GetYaxis()
    #
    fun = _h2_as_fun_ ( self , func )
    #
    from Ostap.PyRoUts import funID
    #
    f2  = ROOT.TF2  ( funID()       ,
                      fun           ,
                      ax.GetXmin () ,
                      ax.GetXmax () ,
                      ay.GetXmin () ,
                      ay.GetXmax () ) 
    
    f2.SetNpx  ( 10 * ax.GetNbins() )
    f2.SetNpy  ( 10 * ay.GetNbins() )
    
    f2._funobj = fun  
    f2._histo  = fun._histo
    f2._func   = fun._func
    
    return f2

    
ROOT.TH1F . asTF     = _h1_as_tf1_ 
ROOT.TH1D . asTF     = _h1_as_tf1_ 
ROOT.TH2F . asTF     = _h2_as_tf2_ 
ROOT.TH2D . asTF     = _h2_as_tf2_ 
ROOT.TH1F . asTF1    = _h1_as_tf1_ 
ROOT.TH1D . asTF1    = _h1_as_tf1_ 
ROOT.TH2F . asTF2    = _h2_as_tf2_ 
ROOT.TH2D . asTF2    = _h2_as_tf2_ 
ROOT.TH1F . asFunc   = _h1_as_fun_ 
ROOT.TH1D . asFunc   = _h1_as_fun_ 
ROOT.TH2F . asFunc   = _h2_as_fun_ 
ROOT.TH2D . asFunc   = _h2_as_fun_ 
ROOT.TH1F . asSpline = _h1_as_spline_ 
ROOT.TH1D . asSpline = _h1_as_spline_ 

# ============================================================================
def _h1_data_ ( h ) :
    """
    Get histogram/graph as vector of pairs (x,y)
    
    >>> histo = ...
    >>> data  = histo.data() 
    """
    return cpp.Gaudi.Math.Splines.getData ( h )

ROOT.TH1F         . data = _h1_data_
ROOT.TH1D         . data = _h1_data_
ROOT.TGraph       . data = _h1_data_
ROOT.TGraphErrors . data = _h1_data_



# =============================================================================
## add some spline&interpolation stuff
# =============================================================================
## create spline object for the histogram
#  @see Gaudi::Math::Spline 
#  @see GaudiMath::Spline 
#  @see GaudiMath::SplineBase
#  @see Genfun::GaudiMathImplementation::SplineBase ;
#  @see GaudiMath::Interpolation::Type
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-03-17 
def _1d_spline_ ( self                                      ,
                  type  = cpp.GaudiMath.Interpolation.Akima ,
                  null  = True                              ,
                  scale = 1                                 ,
                  shift = 0                                 ) :
    """
    Create spline object for the histogram:

    >>> histo = ...
    >>> spline = histo.spline ()

    >>> value  = spline ( 10 ) 
    """
    return cpp.Gaudi.Math.Spline ( self , type , null , scale , shift )
# =============================================================================
## create spline object for the histogram
#  @see Gaudi::Math::SplineError 
#  @see Gaudi::Math::Spline 
#  @see GaudiMath::Spline 
#  @see GaudiMath::SplineBase
#  @see Genfun::GaudiMathImplementation::SplineBase ;
#  @see GaudiMath::Interpolation::Type
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-03-17 
def _1d_spline_err_ ( self                                      ,
                      type  = cpp.GaudiMath.Interpolation.Akima ,
                      null  = True                              ,
                      scale = 1                                 ,
                      shift = 0                                 ) :
    """
    Create spline object for the histogram:

    >>> histo  = ...
    >>> spline = histo.splineErr ()

    >>> value  = spline ( 10 )
    """
    return cpp.Gaudi.Math.SplineErrors ( self , type , null , scale , shift )

_1d_spline_     . __doc__ += '\n' + cpp.Gaudi.Math.Spline       .__init__ .__doc__
_1d_spline_err_ . __doc__ += '\n' + cpp.Gaudi.Math.SplineErrors .__init__ .__doc__

for t in ( ROOT.TH1D , ROOT.TH1F , ROOT.TGraphErrors ) :
    t.spline    = _1d_spline_
    t.splineErr = _1d_spline_err_
    
ROOT.TGraph.spline    = _1d_spline_

# =============================================================================
# 2D interpolation
# =============================================================================
def _2d_interp_ ( self                                   ,
                  typex  = cpp.Gaudi.Math.Interp2D.Cubic , ## default is bicubic 
                  typey  = cpp.Gaudi.Math.Interp2D.Cubic , ## default is bicubic 
                  null   = True                          ,
                  scalex = 1 , 
                  scaley = 1 , 
                  shiftx = 0 , 
                  shifty = 0 ) :
    """
    Create interpolation object for 2D-histogram
    
    >>> histo_2d = ...

    >>> interp = histo_2d.interp()
    
    >>> value = interp ( 10 , 20 ) 
    """
    obj = cpp.Gaudi.Math.Interp2D ( self   ,
                                    typex  , typey  ,
                                    null   ,
                                    scalex , scaley , 
                                    shiftx , shifty )
    obj._histo = self
    
    return obj

_2d_interp_     . __doc__ += '\n' + cpp.Gaudi.Math.Interp2D .__init__ .__doc__

for t in ( ROOT.TH2D , ROOT.TH2F ) :
    t.interp    = _2d_interp_ 

# =============================================================================


# =============================================================================
## fit the histogram by sum of components
def _h_Fit_ ( self                              ,
              components                        ,
              draw = False                      ,
              interpolate = True                ,
              selector    = lambda i,x,y : True ) :
    """
    (Chi_2)-fit the histogram with the set of ``components''
    
    The ``components'' could be histograms, functions and other
    callable object :
    
    >>> h0 = ...
    >>> h .hFit ( h0 )
    
    >>> h0 = ...
    >>> h1 = ...
    >>> h .hFit ( [ h0 , h1 ] )
    
    """
    DATA =   VE.Vector
    CMPS = DATA.Vector

    if   isinstance ( components , ROOT.TH1D ) : components = [ components ]
    elif isinstance ( components , ROOT.TH1F ) : components = [ components ]
    elif not isinstance ( components , ( tuple , list ) ) :
        components = [ components ]

    data = DATA ()
    cmps = CMPS ()
    
    while len ( cmps ) < len ( components )  :
        cmps.push_back( DATA() )

    for i,x,y in self.iteritems () :

        if not selector ( i , x , y ) : continue
        
        dp = VE ( y )
        data.push_back ( dp )
        
        for j in range ( 0 , len ( components ) ) :

            cmp = components[j]
            if isinstance ( cmp , ( ROOT.TH1F , ROOT.TH1D )) :
                cp = cmp ( x , interpolate   )
            else :
                cp =  VE ( cmp ( x.value() ) )  ## CALLABLE !!! 
            
            cmps[ j ].push_back ( cp ) 
            

    _c2Fit = cpp.Gaudi.Math.Chi2Fit ( data , cmps )

    if draw :
        
        if not hasattr ( self , '_histos_' ) :
            self._histos_ = []
            
        self.Draw( 'e1' ) 
        for j in range ( 0 , len ( components ) ) :
            cmpj = components [j]
            if not isinstance ( cmpj , ( ROOT.TH1F , ROOT.TH1D ) ) : continue
            sc = _c2Fit[j].value ()
            nh = cmpj * sc 
            nh.Draw ( 'same' )
            self._histos_.append ( nh ) 

    return _c2Fit


ROOT.TH1F. hFit = _h_Fit_ 
ROOT.TH1D. hFit = _h_Fit_ 

# =============================================================================
## fit histo
#  @see TH1::Fit 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-28
def _f_fit_ ( func , histo , *args ) :
    """
    Fit histogram (Actially delegate to TH1::Fit method)
    
    >>> func  = ...
    >>> histo = ...
    >>> func.Fit ( histo , .... )
    
    """
    return histo.Fit( func , *args )

ROOT.TF1 . Fit      = _f_fit_ 
ROOT.TF1 . fitHisto = _f_fit_ 
ROOT.TF1 . fit      = _f_fit_ 
ROOT.TH1 . fit      = ROOT.TH1.Fit

# =============================================================================
# Decorate fit results 
# =============================================================================

# =============================================================================
## representation of TFitResult object 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _fit_repr_ ( self ) :
    """
    Representaion of TFitResult object
    """
    _r  = ''
    _r += "\n Status      = %s "    %   self.Status ()
    _r += "\n Chi2/nDoF   = %s/%s " % ( self.Chi2   () , self.Ndf() ) 
    _r += "\n Probability = %s "    %   self.Prob   () 
    _p = self.Parameters ()
    _e = self.Errors     ()
    for i in range( 0 , len(_p) ) :
        v = _p[i]
        e = _e[i]
        a = VE ( v ,e*e )
        _r  += " \n %s " % a 
    return _r

# =============================================================================
## get number of parameters 
def _fit_len_ ( r ) :
    """
    Get number of parameters 
    """
    return len ( r.Parameters() ) 

# =============================================================================
## iterator over fit-result object 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _fit_iter_ ( r ) :
    """
    Iterator over fit-result object 
    """
    l = len( r )
    i = 0
    while i < l :
        yield i
        i += 1

# =============================================================================
## get parameter number
#  @code
#  r    = h1.Fit( ... )
#  name = r.GetParNumber ( 1 ) 
#  @endcode
def _fit_parnum_ ( self , par ) : 
    """
    Get parameter number:
    >>> r    = h1.Fit( ... )
    >>> name = r.GetParNumber ( 1 ) 
    """ 
    if isinstance ( par , ( int , long ) ) :
        if 0<= par< len ( self ) : return int( par )   ## RETURN 
        else                     : return       -1     ## RETURN 
    #
    if isinstance   ( par , str )  :
        ll = len ( self )
        for i in range ( 0 , ll ) :
            if self.ParName(i) == par : return i       ## RETURN 
            
    ## nothing is found 
    return -1                                          ## RETURN 

# =============================================================================
## check parameter
#  @code
#  r = h1.Fit(....) ##
#  if i   in r :   ...  ## check parameter by index  
#  if 'm' in r :   ...  ## check parameter by name  
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-07-12
def _fit_contains_ ( self , par ) :
    """
    Check parameter
    >>> r = h1.Fit(....) ##
    >>> if i   in r :   ...  ## check parameter by index  
    >>> if 'm' in r :   ...  ## check parameter by name  
    """
    return  0 <= _fit_parnum_ ( self , par )

    
# =============================================================================
## getitem for fit-result-object            
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _fit_getitem_ ( self , par ) :
    """
    Getitem for fit-result-object            
    """
    ## convert parameter into integer 
    ipar = _fit_parnum_ ( par )
    if not 0<= ipar : raise IndexError("TFitResult:illegal index %s" % par)
    #
    _p = self.Parameter ( ipar )
    _e = self.Error     ( ipar )
    #
    return VE( _p , _e * _e )


# =============================================================================
## Get correlation coefficient for parameters 'i' and 'j'
def _fit_cor_ ( self , i , j ) :
    """
    Get correlation coefficient for parameters 'i' and 'j'

    >>> r = ...
    >>> print r.cor(1,2)
    """
    ipar = _fit_parnum_ ( self , i )
    jpar = _fit_parnum_ ( self , j )
    #
    if  0 > ipar : raise IndexError( "TFitResult:invalid index %s" % i )
    if  0 > jpar : raise IndexError( "TFitResult:invalid index %s" % j )
    #
    _cij = self.CovMatrix ( ipar , jpar )
    _ei  = self.Errors    ( ipar )
    _ej  = self.Errors    ( jpar )
    ##
    if 0 == _ei or 0 == _ej : return 0   ## RETURN 
    #
    return _cij / ( _ei * _ej ) 

# =============================================================================
## Get correlation matrix 
def _fit_corm_ ( self , root = False ) :
    """
    Get correlation matrix 

    >>> r = ...
    >>> print r.corMtrx ()
    """
    _l = len (self) 
    matrix = None
    
    from LHCbMath.Types import Gaudi as _G 
    _GM    = _G.Math
    if   1 == _l and hasattr ( _GM , 'SymMatrix1x1' ) : matrix = _GM.SymMatrix1x1 ()
    elif 2 == _l and hasattr ( _GM , 'SymMatrix2x2' ) : matrix = _GM.SymMatrix2x2 ()
    elif 3 == _l and hasattr ( _GM , 'SymMatrix3x3' ) : matrix = _GM.SymMatrix3x3 ()
    elif 4 == _l and hasattr ( _GM , 'SymMatrix4x4' ) : matrix = _GM.SymMatrix4x4 ()
    elif 5 == _l and hasattr ( _GM , 'SymMatrix5x5' ) : matrix = _GM.SymMatrix5x5 ()
    elif 6 == _l and hasattr ( _GM , 'SymMatrix6x6' ) : matrix = _GM.SymMatrix6x6 ()
    elif 7 == _l and hasattr ( _GM , 'SymMatrix7x7' ) : matrix = _GM.SymMatrix7x7 ()
    elif 8 == _l and hasattr ( _GM , 'SymMatrix8x8' ) : matrix = _GM.SymMatrix8x8 ()
    elif 9 == _l and hasattr ( _GM , 'SymMatrix9x9' ) : matrix = _GM.SymMatrix9x9 ()
    
    ## use ROOT matrices
    if not matrix :
        matrix = ROOT.TMatrix( _l , _l )

    ## fill matrix 
    for i in range (0,_l):
        for j in range (i, _l):
            _cij = self.CovMatrix( i , j ) 
            _eij = self.Error( i ) * self.Error( j )
            if 0 != _eij : _vij = _cij / _eij
            else         : _vij = 0
            matrix [ i , j ] = _vij 
            matrix [ j , i ] = _vij
            
    return matrix


ROOT.TFitResultPtr.__contains__ = _fit_contains_ 

ROOT.TFitResultPtr.__repr__     = _fit_repr_ 
ROOT.TFitResultPtr.__str__      = _fit_repr_ 
ROOT.TFitResultPtr.__iter__     = _fit_iter_ 
ROOT.TFitResultPtr.__getitem__  = _fit_getitem_ 
ROOT.TFitResultPtr.__call__     = _fit_getitem_ 
ROOT.TFitResultPtr.__len__      = _fit_len_ 
ROOT.TFitResultPtr.cor          = _fit_cor_ 
ROOT.TFitResultPtr.corMtrx      = _fit_corm_ 
ROOT.TFitResultPtr.GetParNumber = _fit_parnum_ 


# =============================================================================
## check existence parameter for the function
#  @code 
#  fun = ...         ## function
#    >>> if i   in fun : ... ## check if i   is valid parameter number 
#    >>> if 'm' in fun : ... ## check if 'm' is valid parameter name
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-07-13
def _tf1_contains_ ( func , par ) :
    """
    >>> fun = ...         ## function
    >>> if i   in fun : ... ## check if i   is valid parameter number 
    >>> if 'm' in fun : ... ## check if 'm' is valid parameter name  
    """
    ## check name 
    if   isinstance ( par , str            ) : return 0<=func.GetParNumber ( par ) 
    elif isinstance ( par , ( int , long ) ) : return 0<= par<func.GetNpar (     )
    #
    return False 

# =============================================================================
## Fix parameter for TF1
#  @code
#  fun =  ...     ## function
#  fun.fix(1,0)   ## fix parameter #1  at 0 
#  fun.fix(2)     ## fix parameter #2  at current value
#  fun.fix('m',1) ## fix parameter 'm' at 1 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-07-13
def _tf1_fix_ ( func , par , value = None ) :
    """
    Fix parameter for TF1
    >>> fun =  ...   ## function 
    >>> fun.fix(1,0) ## fix parameter #1 at 0 
    >>> fun.fix(2)   ## fix parameter #2 at current value 
    """
    if not par in func : raise IndexError("Invalid parameter index %s" % par )
    if     isinstance ( par , str  ) : par = func.GetParNumber( par )
    ##
    if not isinstance ( value , ( float , int , long ) )  :
        value = func.GetParameter(par)
    #
    func.FixParameter( par , value ) 

# =============================================================================
## Release parameter for TF1
#  @code
#  fun =  ...       ## function
#  fun.release(1)   ## release parameter #1 
#  fun.release('m') ## release parameter 'm'
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _tf1_release_ ( func , par ) :
    """
    Release parameter for TF1
    >>> fun =  ...       ## function
    >>> fun.release(1)   ## release parameter #1 
    >>> fun.release('m') ## release parameter 'm'
    """
    #
    if not par in func : raise IndexError("Invalid parameter index %s" % par )
    #
    if     isinstance ( par , str  ) : par = func.GetParNumber( par )
    func.ReleaseParameter( par ) 


# =============================================================================
## get the parameter from TF1
#  @code
#  fun =  ...   ## function
#  p = fun.par(1) 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _tf1_par_ ( func , par ) :
    """
    Get parameter from TF1
    >>> fun =  ...        ## function 
    >>> p2 = fun.par(2)   ## get parameter #2 
    >>> pm = fun.par('m') ## get parameter 'm'
    """
    if not par in func : raise IndexError("Invalid parameter index %s" % par )
    #
    if isinstance ( par , str  ) : par = func.GetParNumber( par )    
    v = func.GetParameter ( par )
    e = func.GetParError  ( par )
    #
    return VE ( v , e * e )

# =============================================================================
## set parameter of TF1
#  @code
#  fun =  ...   ## function
#  fun.setPar(1,1) 
#  fun.setPar('m',2) 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _tf1_setpar_ ( func , par , value ) :
    """
    Set parameter of TF1 
    >>> fun =  ...          ## function 
    >>> fun.setPar(1,1)     ## set parameter #1 to be 1 
    >>> fun.setPar('m',2)   ## set parameter 'm' to be 2
    """
    if not par in func : raise IndexError("Invalid parameter index %s" % par )
    #
    if isinstance ( par , str  ) : par = func.GetParNumber( par )
    #
    func.SetParameter ( par , float ( value ) )

# =============================================================================
## primitive iteration over parameters:
#  @code
#  fun =  ...        ## function
#  for p in fun: print fun(p)
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _tf1_iter_ ( func ) :
    """
    Primitive iteration over parameters 
    >>> fun =  ...        ## function 
    >>> for p in fun: print fun(p)
    """
    s = func.GetNpar()
    i = 0
    while i < s :
        yield i
        i += 1
        
# =============================================================================
## get parameter as attribute
#  @code
#  fun =  ...   ## function
#  pm  = fun.m  ## get parameter 'm'
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _tf1_getattr_ ( func , par ) :
    """
    Det parameter as attribute
    >>> fun =  ...   ## function
    >>> pm  = fun.m  ## get parameter 'm'
    """
    if not isinstance ( par , str ) : raise AttributeError('TF1:Invalid attribute %s' % par )
    if not par in func              : raise AttributeError('TF1:Invalid attribute %s' % par )
    return _tf1_par_  ( func , par )

    
ROOT.TF1.__contains__ = _tf1_contains_
ROOT.TF1.__len__      = lambda s : s.GetNpar() 
    
ROOT.TF1.par          = _tf1_par_
ROOT.TF1.param        = _tf1_par_
ROOT.TF1.parameter    = _tf1_par_

ROOT.TF1.setPar       = _tf1_setpar_
ROOT.TF1.__setitem__  = _tf1_setpar_

ROOT.TF1.fix          = _tf1_fix_
ROOT.TF1.rel          = _tf1_release_
ROOT.TF1.release      = _tf1_release_

ROOT.TF1.__iter__     = _tf1_iter_
ROOT.TF1.__getitem__  = _tf1_par_
ROOT.TF1.__getattr__  = _tf1_getattr_

    
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
