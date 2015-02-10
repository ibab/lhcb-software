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
        
        if pars :
            np = self._hfit.npars() 
            for i in range ( 0 , np ) :    
                self._hfit.setPar ( i , pars[i] )
                
        return self._hfit( x[0] )

BernsteinFIT = H_fit
BSplineFIT   = H_fit

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
        self.fun   = ROOT.TF1 ( funID() , self , hfit.xmin() , hfit.xmax() , hfit.npars() + 1  )
        self.fun.SetParameter ( 0 , 1 )
        
    def npars    ( self ) : return self._hfit.npars () + 1  ## NB: normalization!  
    def pars     ( self ) : return self._hfit.pars  ()
    
    def draw     ( self , *args ) : return sef.fun.Draw( *args ) 
    def Draw     ( self , *args ) : return sef.fun.Draw( *args )
    
    def fit      ( self , h , opts = 'S' , *args ) : return h.Fit( self.fun , opts , *args ) 
    def Fit      ( self , h , opts = 'S' , *args ) : return h.Fit( self.fun , opts , *args ) 

    ## the major method 
    def __call__ ( self , x , pars = [] ) :

        norm = 1.0
        
        if pars :

            norm = pars[0]
            
            np   = self._hfit.npars() 
            for i in range ( 0 , np ) :    
                self._hfit.setPar ( i , pars[i+1] )
                
        return norm * self._hfit( x[0] )
    
PositiveFIT         = H_Nfit
PositiveSplineFIT   = H_Nfit
MonothonicSplineFIT = H_Nfit

# =============================================================================
## represent 1D-histo as Bernstein polynomial
def _h1_bernstein_ ( h1 , N , interpolate = True , opts = 'SQ0I' ) :
    """
    Represent histo as Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.bernstein ( 5 )
    
    """
    mn,mx = h1.xminmax ()
    #
    ## N = min ( N ,  len ( h1 ) - 1 ) 
    b  = cpp.Gaudi.Math.Bernstein ( N , mn , mx )
    #
    ## make the approximation for bernstein coefficients 
    for i in range ( 0 , N + 1 ) :

        if   i == 0   :
        
            x  = mn 
            fb =  1 
            v  = h1 [ 1 ].value()

        elif i == N   :
            
            x  = mx
            lb = h1.bins() 
            v  = h1 [ lb ].value()
            
        else :
            
            x = mn + ( mx - mn ) * float( i ) / float ( N )
            v = h1 ( x , interpolate = interpolate ).value()
            
        b.setPar ( i , v )


    bfit  = BernsteinFIT( b )    
    bfit.fun.SetNpx ( max ( 100 , 3 * h1.bins() ) )  
    
    bfit.histo     = h1
    bfit.fitresult = bfit.fun.Fit(h1,opts) 
    
    return bfit.fun,bfit,b,bfit.fitresult 

# =============================================================================
## represent 1D-histo as B-spline
def _h1_bspline_ ( h1 , order = 3 , knots = 3 , opts = 'SQ0I' ) :
    """
    Represent histo as B-spline polynomial
    
    >>> h = ... # the historgam
    >>> b1 = h.bSpline ( order = 3 , innerknots = 3  )
    >>> b2 = h.bSpline ( order = 3 , innerknots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        b = cpp.Gaudi.Math.BSpline ( mn , mx , knots , order )
    else :
        _knots = cpp.std.vector('double') ()
        _knots.push_back ( mn ) 
        _knots.push_back ( mx ) 
        for k in knots : _knots.push_back( k )
        b = cpp.Gaudi.Math.BSpline ( _knots , order )

    bfit  = BSplineFIT( b )
    bfit.fun.SetNpx ( max ( 100 , 3 * h1.bins() ) )  
    
    bfit.histo     = h1 
    bfit.fitresult = bfit.fun.Fit(h1,opts) 
    
    return bfit.fun,bfit,b,bfit.fitresult 


# =============================================================================
## represent 1D-histo as POSITIVE spline polynomial
def _h1_positive_ ( h1 , N , opts = 'SQ0I' ) :
    """
    Represent histo as Positive Bernstein polynomial
    
    >>> h = ... # the historgam
    >>> b = h.positive ( 5 )
    
    """
    mn,mx = h1.xminmax ()
    b = cpp.Gaudi.Math.Positive ( N , mn , mx )
    #

    bfit  = PositiveFIT( b )

    bfit.fun.SetNpx ( max ( 100 , 3 * h1.bins() ) )  
    bfit.fun.SetParameter ( 0 , h1.accumulate().value() / h1.bins() )
    
    bfit.histo     = h1
    bfit.fitresult = bfit.fun.Fit(h1,opts) 
    
    return fun,bfit,b,bfit.fitresult 

# =============================================================================
## represent 1D-histo as positive B-spline
def _h1_pspline_ ( h1 , order = 3 , knots = 3 , opts = 'SQ0I' ) :
    """
    Represent histo as positive B-spline 
    
    >>> h = ... # the historgam
    >>> b1 = h.pSpline ( order = 3 , knots = 3  )
    >>> b2 = h.pSpline ( order = 3 , knots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        b = cpp.Gaudi.Math.PositiveSpline ( mn , mx , knots , order )
    else :
        _knots = cpp.std.vector('double') ()
        _knots.push_back ( mn ) 
        _knots.push_back ( mx ) 
        for k in knots : _knots.push_back( k )
        b = cpp.Gaudi.Math.BSpline ( _knots , order )


    bfit  = PositiveSplineFIT( b )
    
    bfit.fun.SetParameter ( 0 , h1.accumulate().value() / h1.bins() )
    bfit.fun.SetNpx ( max ( 100 , 3 * h1.bins() ) )  
    
    bfit.histo     = h1
    bfit.fitresult = bfit.fun.Fit(h1,opts) 
    
    return bfit.fun,bfit,b,bfit.fitresult 


# =============================================================================
## represent 1D-histo as positive increasing spline
def _h1_ispline_ ( h1 , order = 3 , knots = 3 , opts = 'SQ0I' ) :
    """
    Represent histo as positive increasing spline 
    
    >>> h = ... # the historgam
    >>> b1 = h.iSpline ( order = 3 , knots = 3  )
    >>> b2 = h.iSpline ( order = 3 , knots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        b = cpp.Gaudi.Math.MonothonicSpline ( mn , mx , knots , order , True )
    else :
        _knots = cpp.std.vector('double') ()
        _knots.push_back ( mn ) 
        _knots.push_back ( mx ) 
        for k in knots : _knots.push_back( k )
        b = cpp.Gaudi.Math.IncreasingSpline ( _knots , order , True )

    bfit  = MonothonicSplineFIT( b )
    bfit.fun.SetParameter ( 0   , h1[h1.bins()]   )
    bfit.fun.SetNpx ( max ( 100 , 3 * h1.bins() ) )  
    
    bfit.histo     = h1
    bfit.fitresult = bfit.fun.Fit(h1,opts) 
    
    return bfit.fun,bfit,b,bfit.fitresult 


# =============================================================================
## represent 1D-histo as positive decreasing spline
def _h1_dspline_ ( h1 , order = 3 , knots = 3 , opts = 'SQ0I' ) :
    """
    Represent histo as positive decreasing spline 
    
    >>> h = ... # the historgam
    >>> b1 = h.dSpline ( order = 3 , knots = 3  )
    >>> b2 = h.dSpline ( order = 3 , knots = [ 0.1 , 0.2, 0.8, 0.9 ]  )
    
    """
    mn,mx = h1.xminmax ()
    #
    from Ostap.PyRoUts import funID
    if isinstance ( knots , ( int , long ) ) :
        b = cpp.Gaudi.Math.MonothonicSpline ( mn , mx , knots , order , False )
    else :
        _knots = cpp.std.vector('double') ()
        _knots.push_back ( mn ) 
        _knots.push_back ( mx ) 
        for k in knots : _knots.push_back( k )
        b = cpp.Gaudi.Math.MonothonicSpline ( _knots , order , False )

    bfit  = MonothonicSplineFIT( b )
    bfit.fun.SetParameter ( 0 , h1[1]             )
    bfit.fun.SetNpx ( max ( 100 , 3 * h1.bins() ) )  
    
    bfit.histo     = h1
    bfit.fitresult = bfit.fun.Fit(h1,opts) 
    
    return bfit.fun,bfit,b,bfit.fitresult 


for t in ( ROOT.TH1D , ROOT.TH1F ) :
    t.bernstein  = _h1_bernstein_
    t.positive   = _h1_positive_
    t.bSpline    = _h1_bspline_
    t.pSpline    = _h1_pspline_
    t.iSpline    = _h1_ispline_
    t.dSpline    = _h1_dspline_

    
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
cpp.Gaudi.Math.BSpline          .funobj = _funobj0_
cpp.Gaudi.Math.Positive         .funobj = _funobjN_
cpp.Gaudi.Math.PositiveSpline   .funobj = _funobjN_
cpp.Gaudi.Math.MonothonicSpline .funobj = _funobjN_

# =============================================================================
## helper class to wrap 1D-histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
class H1Func(object) :
    """
    Helper class to Wrap 1D-histogram as function 
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
        norm  = par[0]
        bias  = par[1]
        scale = par[2]
        #
        x0    = ( x0 - bias ) / scale
        # 
        return norm * self._func ( self._histo ( x0 , interpolate = True ) )


# =============================================================================
## helper class to wrap 1D-histogram as spline 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-27
class H1Spline(object) :
    """
    Helper class to Wrap 1D-histogram as function/spline 
    """
    def __init__ ( self , histo , func = lambda s : s.value() , *args ) :
        self._spline = histo.splineErr ( *args )
        self._histo  = self._spline ## ATTENTION!!! 
        self._func   = func
        
    ## evaluate the function 
    def __call__ ( self , x , par = [ 1 ] ) :
        """
        Evaluate the function 
        """
        #
        x0 = x if isinstance ( x , (int,long,float) ) else x[0]
        #
        norm  = par[0]
        #
        return norm * self._func ( self._spline ( x0 ) ) 

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
    construct the function fomr the histogram 
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
    construct the function fomr the histogram 
    """
    return H2Func ( self , func )
# =============================================================================
## construct function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_as_tf1_ ( self , func = lambda s : s.value () , spline = False , *args ) :
    """
    Construct the function from the 1D-histogram

    >>> fun = h1.asFunc()
    
    """
    ax  = self.GetXaxis()
    #
    from Ostap.PyRoUts import funID
    #
    if spline :
        fun = _h1_as_spline_ ( self , func , *args )
        f1  = ROOT.TF1  ( funID()       ,
                          fun           ,
                          ax.GetXmin () ,
                          ax.GetXmax () , 1 )
        
        f1.SetParNames ( 'Normalization' )
        #
    else      :
        #
        fun = _h1_as_fun_    ( self , func )
        f1  = ROOT.TF1  ( funID()       ,
                          fun           ,
                          ax.GetXmin () ,
                          ax.GetXmax () , 3 )
        f1.SetParNames (
            'Normalization' ,
            'Bias'          ,
            'Scale' 
            )
        #
        f1.FixParameter(1,0)
        f1.FixParameter(2,1)
        #

    f1.FixParameter(0,1) 
    f1.SetNpx  ( 10 * ax.GetNbins() )
    f1._funobj = fun  
    f1._histo  = fun._histo
    f1._func   = fun._func
    
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

    >>> value = spline ( 10 ) 
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
## iterator over fit-result object 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _fit_iter_ ( r ) :
    """
    Iterator over fit-result object 
    """
    _i = 0
    _p = r.Parameters ()
    _e = r.Errors     ()
    _l = len(_p)
    while _i < _l :
        a = VE ( _p[_i] , _e[_i]**2 )
        yield a
        _i += 1
        
# =============================================================================
## getitem for fit-result-object            
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _fit_get_item_ ( self , i ) :
    """
    Getitem for fit-result-object            
    """
    _p = self.Parameters ()
    _e = self.Errors     ()
    _l = len(_p)
    if 0<= i < _l :
        return VE ( _p[i]  , _e[i]**2 )
    raise IndexError 

ROOT.TFitResultPtr.__repr__     = _fit_repr_ 
ROOT.TFitResultPtr.__str__      = _fit_repr_ 
ROOT.TFitResultPtr.__iter__     = _fit_iter_ 
ROOT.TFitResultPtr.__getitem__  = _fit_get_item_ 
ROOT.TFitResultPtr.__call__     = _fit_get_item_ 
ROOT.TFitResultPtr.__len__      = lambda s : len( s.Parameters() ) 


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
