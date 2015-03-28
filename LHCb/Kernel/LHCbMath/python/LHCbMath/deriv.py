#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file  LHCbMath/deriv.py 
#  Simple adaptive numerical differentiation (for pyroot/PyRoUts/Ostap)
#  R. De Levie, "An improved numerical approximation for the first derivative"
#  @see http://www.ias.ac.in/chemsci/Pdf-Sep2009/935.pdf
#
#  .. and also very simple wrapper to numerical integation using scipy
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Simple adaptive numerical differentiation (for pyroot/PyRoUts/Ostap/...)

>>> func = lambda x : x*x
>>> print derivative ( func , 1 )

... and also very simple wrapper to numerical integation using scipy

>>> func = lambda x : x*x
>>> print integral ( func , 0 , 1 )

there are also object form:

>>> func = ...
>>> deriv = Derivative ( func )
>>> integ = Integral   ( func , 0 )
>>> print deriv ( 0.1 ) , integ ( 0.1 )  

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-06"
__all__     = (
    "derivative" , ## numerical differentiation (as function)
    "integral"   , ## numerical integration     (as function,  using scipy)
    "Derivative" , ## numerical differentiation (as object) 
    "Integral"   , ## numerical integration     (as as object, using scipy)
    ) 
# =============================================================================
from sys import float_info
_eps_   = float_info.epsilon
if not 0.75 < _eps_ * 2**52 < 1.25 :
    import warnings
    warnings.warn ('"epsilon" in not in the expected range!Math could be suboptimal')
# =============================================================================
from LHCbMath.Types import cpp 
VE = cpp.Gaudi.Math.ValueWithError
_mULPs_       = 1000 
_is_zero_     = cpp.LHCb .Math.Zero('double')()
_next_double_ = cpp.Gaudi.Math.next_double
def _delta_ ( x , ulps = _mULPs_ ) :
    n1 = _next_double_ ( x ,  ulps )
    n2 = _next_double_ ( x , -ulps )
    return max ( abs ( n1 - x ) , abs ( n2 - x ) ) 

# =============================================================================
## calculate 1st (and optionally 3rd)  derivative with the given step
#  f'      is calculated as O(h**2)
#  f^(III) is calculated as O(h**2)
def _h2_ ( func , x , h , der = False ) :
    """
    calculate 1st (and optionally 3rd) derivative  with O(h*h) precision
    """
    fm1 = func ( x - 1 * h )
    fp1 = func ( x + 1 * h )

    d1   = (         -fm1 +  fp1        ) / (2 * h    )
    if not der : return d1 
    
    fm2 = func ( x - 2 * h )    
    fp2 = func ( x + 2 * h )
    d3   = ( -fm2 + 2*fm1 -2*fp1 + fp2  ) / (2 * h*h*h)

    return d1,d3

# =============================================================================
## calculate 1st (and optionally 5th) derivative with the given step
#  f'     is calculated as O(h**4)
#  f^(V)  is calculated as O(h**2)
def _h4_ ( func , x , h , der = False ) :
    """
    calculate 1st (and optionally 5th) derivative  with O(h*h) precision
    """    
    fm2 = func ( x - 2 * h )    
    fm1 = func ( x - 1 * h )
    fp1 = func ( x + 1 * h )
    fp2 = func ( x + 2 * h )
    
    d1  = (         fm2 -8*fm1 +8*fp1 -  fp2       )/(12 * h   )
    if not der : return d1 
    
    fm3 = func ( x - 3 * h )    
    fp3 = func ( x + 3 * h )
    d5  = ( -fm3 +4*fm2 -5*fm1 +5*fp1 -4*fp2 + fp3 )/( 2 * h**5)
    
    return d1,d5

# =============================================================================
## calculate 1st (and optionally 7th) derivative with the given step
#  f'      is calculated as O(h**6)
#  f^(VII) is calculated as O(h**2)
def _h6_ ( func , x , h , der = False ) :
    """
    calculate 1st (and optionally 7th) derivative  with O(h*h) precision
    """        
    fm3 = func ( x - 3 * h )    
    fm2 = func ( x - 2 * h )    
    fm1 = func ( x - 1 * h )
    fp1 = func ( x + 1 * h )
    fp2 = func ( x + 2 * h )
    fp3 = func ( x + 3 * h )
    
    d1  = (         -fm3 + 9*fm2 -45*fm1 +45*fp1 - 9*fp2 +  fp3       )/(60 * h   )

    if not der : return d1
    
    fm4 = func ( x - 4 * h )    
    fp4 = func ( x + 4 * h )
    d7  = ( -fm4 + 6*fm3 -14*fm2 +14*fm1 -14*fp1 +14*fp2 -6*fp3 + fp4 )/( 2 * h**7)
    
    return d1,d7

# =============================================================================
## calculate 1st (and optionally 9th) derivative with the given step
#  f'     is calculated as O(h**8)
#  f^(IX) is calculated as O(h**2)
def _h8_ ( func , x , h , der = False ) :
    """
    calculate 1st (and optionally 9th) derivative  with O(h*h) precision
    """            
    fm4 = func ( x - 4 * h )    
    fm3 = func ( x - 3 * h )    
    fm2 = func ( x - 2 * h )    
    fm1 = func ( x - 1 * h )
    fp1 = func ( x + 1 * h )
    fp2 = func ( x + 2 * h )
    fp3 = func ( x + 3 * h )
    fp4 = func ( x + 4 * h )
    
    d1  = (       3*fm4 -32*fm3 +168*fm2 -672*fm1 +672*fp1 -168*fp2 +32*fp3 -3*fp4       )/(840 * h   )

    if not der : return d1 
    
    fm5 = func ( x - 5 * h )    
    fp5 = func ( x + 5 * h )
    d9  = ( -fm5 +8*fm4 -27*fm3 + 48*fm2 - 42*fm1 + 42*fp1 - 48*fp2 +27*fp3 -8*fp4 + fp5 )/(  2 * h**9)
    
    return d1,d9

# =============================================================================
## calculate 1st (and optionally 11th) derivative with the given step
#  f'     is calculated as O(h**10)
#  f^(XI) is calculated as O(h**2)
def _h10_ ( func , x , h , der = False ) :
    """
    calculate 1st (and optionally 11th) derivative  with O(h*h) precision
    """
    
    fm5 = func ( x - 5 * h )    
    fm4 = func ( x - 4 * h )    
    fm3 = func ( x - 3 * h )    
    fm2 = func ( x - 2 * h )    
    fm1 = func ( x - 1 * h )
    fp1 = func ( x + 1 * h )
    fp2 = func ( x + 2 * h )
    fp3 = func ( x + 3 * h )
    fp4 = func ( x + 4 * h )
    fp5 = func ( x + 5 * h )
    
    d1  = (       -2*fm5 +25*fm4 -150*fm3 +600*fm2 -2100*fm1 +2100*fp1 -600*fp2 +150*fp3 -25*fp4 + 2*fp5       )/(2520 * h    )

    if not der : return d1
    
    fm6 = func ( x - 6 * h )    
    fp6 = func ( x + 6 * h )
    d11 = ( -fm6 +10*fm5 -44*fm4 +110*fm3 -165*fm2 + 132*fm1 - 132*fp1 +165*fp2 -110*fp3 +44*fp4 -10*fp5 + fp6 )/(   2 * h**11)
    
    return d1,d11

# =============================================================================
# The actual setup for
# =============================================================================
_funcs_ = (
    _h2_   ,
    _h2_   ,
    _h4_   ,
    _h6_   ,
    _h8_   ,
    _h10_ 
    )
_numbers_ = (
    ( 0.5*10**(-10./ 3) ,
      0.5*10**(-10./ 3) ,
      0.5*10**(-10./ 5) ,
      0.5*10**(-10./ 7) ,
      0.5*10**(-10./ 9) ,
      0.5*10**(-10./11) ) , 
    (    6 , 4.5324e-17 , 5.1422e-6 , 6.0554e-6 ) , ## I=1, J= 3  3-point rule 
    (   30 , 6.0903e-17 , 8.5495e-4 , 7.4009e-4 ) , ## I=2, J= 5  5-point rule 
    (  140 , 6.9349e-17 , 7.7091e-3 , 5.8046e-4 ) , ## I=3, J= 7  7-point rule 
    (  630 , 7.4832e-17 , 2.6237e-2 , 1.8227e-2 ) , ## I=4, J= 9  9-point rule 
    ( 2772 , 7.8754e-17 , 5.7292e-2 , 3.7753e-2 ) , ## I=5, J=11 11-point rule 
    )
# =============================================================================
## Calculate the first derivative for the function
#  R. De Levie, "An improved numerical approximation for the first derivative"
#  @see http://www.ias.ac.in/chemsci/Pdf-Sep2009/935.pdf
#  @code
#  >>> func =  lambda x : x*x
#  >>> print derivative ( func , x = 1 ) 
#  @endcode
#  @param func (INPUT) the function itself
#  @param x    (INPUT) the argument
#  @param h    (INPUT) the guess for the step used in numeric differentiation
#  @param I    (INPUT) the rule to be used ("N-point rule" = 2*I+1)
#  @param err  (INPUT) calcualte the uncertainty?
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
def derivative ( func , x , h = 0  , I = 2 , err = False ) : 
    """
    Calculate the first derivative for the function
    #  @code
    #  >>> func =  lambda x : x*x
    #  >>> print derivative ( func , x = 1 ) 
    """

    ## get the function value at the given point 
    f0 = func(x)

    ## adjust the rule 
    I  = min ( max ( I , 1 ) , 5 )
    J  = 2 * I + 1

    
    _dfun_ = _funcs_[I]
    delta  = _delta_ ( x )
    
    ## if the intial step is too small, choose another one 
    if abs ( h ) <  _numbers_[I][3] or abs ( h ) < delta :  
        if _is_zero_( x )     : h    =             _numbers_[0][I]
        else                  : h    = abs ( x ) * _numbers_[I][3] 
        
    ## 1) find the estimate for first and "J"th the derivative with the given step 
    d1,dJ = _dfun_( func , x , h , True )
        
    ## find the optimal step 
    if _is_zero_  ( dJ ) or  ( _is_zero_ ( f0 ) and _is_zero_ ( x * d1 ) ) :
        if  _is_zero_ ( x )   : hopt =             _numbers_[0][I] 
        else                  : hopt = abs ( x ) * _numbers_[I][3]
    else : 
        hopt = _numbers_[I][2]*(  ( abs ( f0 ) + abs ( x * d1 ) ) / abs ( dJ ) )**( 1.0 / J )

    ## finally get the derivative 
    if not err  :  return _dfun_ ( func , x , hopt , False )

    ## estimate the uncrtainty, if needed  
    d1,dJ =  _dfun_ ( func , x , hopt , True )
    e     = _numbers_[I][1]/(J-1)/_numbers_[I][2]
    e2    = e * e * ( J * _eps_ + abs ( f0 ) + abs( x * d1 ) )**(2-2./J) * abs( dJ )**(2./J)
    return VE ( d1 , 4 * e2 )

# =============================================================================
## @class Derivative
#  Calculate the first derivative for the function
#  R. De Levie, "An improved numerical approximation for the first derivative"
#  @see http://www.ias.ac.in/chemsci/Pdf-Sep2009/935.pdf
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
class Derivative(object) :
    """
    Calculate the first derivative for the function
    R. De Levie, ``An improved numerical approximation for the first derivative''
    see http://www.ias.ac.in/chemsci/Pdf-Sep2009/935.pdf
    """
    def __init__ ( self , func , h = 0 , I = 2 , err = False ) :
        """
        Calculate the first derivative for the function
        R. De Levie, ``An improved numerical approximation for the first derivative''
        see http://www.ias.ac.in/chemsci/Pdf-Sep2009/935.pdf
        
        >>> func = math.sin
        >>> deri = Derivative ( func )
        
        """
        self._func = func
        self._h    = h    
        self._I    = I
        self._err  = err

    ## evaluate the derivative 
    def __call__ ( self , x ) :
        """
        Calculate the first derivative for the function
        R. De Levie, ``An improved numerical approximation for the first derivative''
        see http://www.ias.ac.in/chemsci/Pdf-Sep2009/935.pdf
        
        >>> func  = math.sin
        >>> deriv = Derivative ( func )
        
        >>> print deriv(0.1) 
        """
        return derivative ( self._func ,
                            x          ,
                            self._h    ,
                            self._I    ,
                            self._err  )

# =============================================================================
## Calculate the integral (from x0 to x) for the 1D-function 
#  @code 
#  >>> func = ...
#  >>> a = integral(func,0,1)
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
def integral ( func , x0 , x , err = False , *args ) :
    """
    Calculate the integral for the 1D-function using scipy
    
    >>> func = ...
    >>> i = integral(func,0,1)
    """
    from scipy import integrate
    result = integrate.quad ( func , x0 , x , args = args )
    return VE( result[0] , result[1] * result[1] ) if err else result[0] 

# =============================================================================
## @class Integral
#  Calculate the integral (from x0 to x) for the 1D-function 
#  @code 
#  >>> func = ...
#  >>> func_0 = Integral(func,0)
#  >>> func_0 ( 10 )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
class Integral(object) :
    """
    Calculate the integral for the 1D-function using scipy
    """
    ## Calculate the integral for the 1D-function using scipy
    def __init__ ( self , func , x0 = 0 , err = False , *args ) :
        """
        Calculate the integral for the 1D-function using scipy
        
        >>> func = ...
        >>> func_0 = Integral(func,0)
        """
        self._func   = func 
        self._x0     = x0
        self._err    = err
        self._args   = args
        
    ## Calculate the integral for the 1D-function using scipy
    def __call__ ( self , x , *args ) :
        """
        Calculate the integral for the 1D-function using scipy
        
        >>> func = ...
        >>> func_0 = Integral(func,0)
        >>> func_0 ( 10 ) 
        """
        from scipy import integrate
        args   = args if args else self._args 
        result = integrate.quad ( self._func , self._x0 , x , args = args )
        return VE ( result[0] , result[1] * result[1] ) if self._err else result[0] 

# =============================================================================
## @class Moment
#  Calculate the N-th moment for the distribution 
#  @code
#   xmin,xmax = 0,math.pi 
#   mean  = Moment(1,xmin,xmax)  ## specify min/max
#   value = mean  ( math.sin )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
class Moment(object) :
    """
    Calculate the N-th moment for the distribution
    
    >>> xmin,xmax = 0,math.pi 
    >>> mean  = Moment(1,xmin,xmax)  ## specify min/max
    >>> value = mean  ( math.sin )
    
    """
    ## constructor
    def __init__ ( self , N , xmin , xmax , err = False , x0 = 0 , *args ) :
        """
        Contructor 
        """
        if not isinstance ( N , ( int , long ) ) :
            raise TypeError('Moment: illegal order')
        
        self._N    = N 
        self._xmin = xmin
        self._xmax = xmax
        self._x0   = x0  
        self._err  = err
        self._args = args
        
    ## calculate un-normalized k-moment  
    def _moment0_ ( self , func , *args ) :
        from scipy import integrate
        args = args if args else self._args 
        return integrate.quad ( func , self._xmin , self._xmax , args = args )
        
    ## calculate un-normalized k-moment  
    def _momentK_ ( self , k , func , *args ) :
        from scipy import integrate
        x0     = self._x0 
        func_N = lambda x,*a : func( x , *a ) * ( ( x - x0 ) ** k  )
        ##
        args = args if args else self._args 
        return integrate.quad ( func_N , self._xmin , self._xmax , args = args )
    
    ## calculate the moment 
    def __call__ ( self , func , *args ) :
        ## 
        n0  = self._moment0_ (            func , *args ) 
        nN  = self._momentK_ ( self._N  , func , *args ) 
        ##
        if not self._err : return nN[0]/n0[0]
        ##
        n_0    = VE ( n0[0] , n0[1] * n0[1] )
        n_N    = VE ( nN[0] , nN[1] * nN[1] )
        ##
        return n_N/n_0

# =============================================================================
## get the mean-value
#  Calculate the mean-value for the distribution 
#  @code
#   xmin,xmax = 0,math.pi 
#   mean  = Mean(xmin,xmax)  ## specify min/max
#   value = mean  ( math.sin )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
class Mean(Moment) :
    """
    Calculate the N-th moment for the distribution
    
    >>> xmin,xmax = 0,math.pi 
    >>> mean  = Mean ( xmin , xmax )  ## specify min/max
    >>> value = mean ( math.sin    )
    
    """
    def __init__ ( self , xmin , xmax , err = False , x0  = 0 ) :
        Moment.__init__ ( self , 1 , xmin , xmax , err , x0 )

# =============================================================================
## get the variance
#  Calculate the mean-value for the distribution 
#  @code
#   xmin,xmax = 0,math.pi 
#   variance  = Variance ( xmin,xmax )  ## specify min/max
#   value     = variance ( math.sin  )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-06-06
class Variance(Moment) :
    def __init__ ( self , xmin , xmax , err = False , x0  = 0 ) :
        Moment.__init__ ( self , 2 , xmin , xmax , err , x0 )
    ## calculate the variance 
    def __call__ ( self , func , *args ) :
        ## 
        n0 = self._moment0_ (     func , *args ) 
        n1 = self._momentK_ ( 1 , func , *args ) 
        n2 = self._momentK_ ( 2 , func , *args ) 
        ##
        if not self._err :
            n_0 = n0[0]
            n_1 = n1[0]
            n_2 = n2[0]
        else : 
            n_0 = VE ( n0[0] , n0[1] * n0[1] )
            n_1 = VE ( n1[0] , n1[1] * n1[1] )
            n_2 = VE ( n2[0] , n2[1] * n2[1] )
        ##
        return (n_2*n_0 - n_1*n_1)/(n_0*n_0)
    
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
    import math

    functions = [
        ( lambda x : math.cos(100.*x)  , lambda x : -100*math.sin(100.*x)                , 0   ) , 
        ( lambda x : x**3              , lambda x : 3.0*x*x                              , 1   ) , 
        ( lambda x : math.exp(x)       , lambda x : math.exp(x)                          , 5   ) ,
        ( lambda x : x**8              , lambda x : 8.0*x**7                             , 2.2 ) ,
        ( lambda x : math.tanh(10.*x)  , lambda x : 10*(1.-math.tanh(10.*x)**2)          , 0.1 ) ,
        ( lambda x : math.tan (10.*x)  , lambda x : 10*(1.+math.tan (10.*x)**2)          , 2.1 ) ,
        ( lambda x : 1./math.sin(2.*x) , lambda x : -2./math.sin(2.*x)**2*math.cos(2.*x) , 0.2 ) , 
        ( lambda x : 1.11*x            , lambda x : 1.11                                 , 0.4 ) , 
        ( lambda x : 1000./x**2        , lambda x : -2000./x**3                          , 0.8 ) , 
        ( lambda x : 1.11111           , lambda x : 0.0                                  , 0.6 ) , 
        ( lambda x : x**50             , lambda x : 50.*x**49                            , 1.3 ) , 
        ]


    for o in functions :

        fun = o[0] ## function 
        der = o[1] ## derivative 
        x   = o[2] ## argument 
        
        print 80*'*'
        
        for i in range(1,6 ) :
            
            res = derivative ( fun , x , 1.e-20 , i  , err = True )
            f1  = res 
            d   = der(x)  ## the exact value for derivative 
            if _is_zero_ ( d ) : 
                print 'Rule=%2d' % ( 2*i+1 ) , f1 , d , (f1.value()-d)    
            else      :
                print 'Rule=%2d' % ( 2*i+1 ) , f1 , d , (f1.value()-d)/d  
                    
    print 80*'*'

    ## the function 
    func    = math.sin
    ## analysitical derivative 
    deriv_a = math.cos
    ## numerical first derivative     
    deriv_1 = Derivative ( func    , I = 5 )  
    ## numerical second derivative     
    deriv_2 = Derivative ( deriv_1 , I = 5 )  

    import random

    for i in range ( 0 , 20 ) : 

        x = random.uniform( 0 , 0.5*math.pi )
        
        print "x=%10.5g f=%10.5g delta(f')= %+10.4g delta(f\")= %+10.4g "  %  ( x       ,
                                                                                func(x) ,
                                                                                1-deriv_1(x)/deriv_a(x),
                                                                                1+deriv_2(x)/func(x) )

    print 80*'*'
            
# =============================================================================
# The END 
# =============================================================================
