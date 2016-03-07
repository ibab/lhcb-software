#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#
#  Simple file to provide soem wrapper function for dealing with
#  Gaudi::Math::ValueWithError objects 
#  @see Gaudi::Math::ValueWithError
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-06-02
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
#
#
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-02"
__version__ = "$Revision$"
# =============================================================================
__all__     = (
    'exp'    , 'expm1'  ,
    'log'    , 'log10'  , 'log1p'  , 
    'sqrt'   , 'cbrt'   , 'pow'    ,   
    'sin'    , 'cos'    , 'tan'    , 
    'sinh'   , 'cosh'   , 'tanh'   ,
    'asin'   , 'acos'   , 'atan'   , 
    'asinh'  , 'acosh'  , 'atanh'  ,
    'erf'    , 'erfc'   , 'erfcx'  , 'probit' , 
    'gamma'  , 'tgamma' , 'lgamma' ,
    'exp2'   , 'log2'   ,
    'hypot'  , 'fma'  
    )
# =============================================================================
import math
from   LHCbMath.Types import cpp
VE     = cpp.Gaudi.Math.ValueWithError
_zero  = cpp.LHCb.Math.Zero('double')()
_ln2_i = 1/math.log(2.0)                 ## useful constant 
# =============================================================================

# =============================================================================
## define ``exp'' function 
def exp ( x ) :
    """ 'exp' function taking into account the uncertainties
    """
    fun = getattr ( x , '__exp__' , None )
    if fun : return fun()
    return math.exp ( x )

# =============================================================================
## define ``exp2'' function 
def exp2 ( x ) :
    """ 'exp2' function taking into account the uncertainties
    """
    fun = getattr ( x , '__exp2__' , None )
    if fun : return fun()
    return 2**x 

# =============================================================================
## define ``expm1'' function 
def expm1 ( x ) :
    """ 'expm1' function taking into account the uncertainties
    """
    fun = getattr ( x , '__expm1__' , None )
    if fun : return fun()
    return math.expm1 ( x )

# =============================================================================
## define ``log'' function 
def log ( x ) :
    """'log' function taking into account the uncertainties
    """
    fun = getattr ( x , '__log__' , None )
    if fun : return fun()
    return math.log ( x )

# =============================================================================
## define ``log2'' function 
def log2 ( x ) :
    """'log2' function taking into account the uncertainties
    """
    fun = getattr ( x , '__log2__' , None )
    if fun : return fun()
    ## 
    return _ln2_i * math.log ( x ) 

# =============================================================================
## define ``log10'' function 
def log10 ( x ) :
    """'log10' function taking into account the uncertainties
    """
    fun = getattr ( x , '__log10__' , None )
    if fun : return fun()
    return math.log10 ( x )

# =============================================================================
## define ``log1p'' function 
def log1p ( x ) :
    """'log1p' function taking into account the uncertainties
    """
    fun = getattr ( x , '__log1p__' , None )
    if fun : return fun()
    return math.log1p ( x )

# =============================================================================
## define ``sqrt'' function 
def sqrt ( x ) :
    """'sqrt' function taking into account the uncertainties
    """
    fun = getattr ( x , '__sqrt__' , None )
    if fun : return fun()
    return math.sqrt ( x )

# =============================================================================
## define ``cbrt'' function 
def cbrt ( x ) :
    """'cbrt' function taking into account the uncertainties
    """
    fun = getattr ( x , '__cbrt__' , None )
    if fun : return fun()
    return math.pow ( x , 1.0/3.0 )

# =============================================================================
## define ``pow'' function 
def pow ( x , y , *a ) :
    """'pow' function taking into account the uncertainties
    """
    if   isinstance ( x , VE ) or isinstance ( y , VE ) : return x**y 
    return math.pow ( x , y , *a ) 


# =============================================================================
## define ``sin'' function 
def sin ( x ) :
    """'Sine' function taking into account the uncertainties
    """
    fun = getattr ( x , '__sin__' , None )
    if fun : return fun()
    return math.sin ( x )

# =============================================================================
## define ``cos'' function 
def cos ( x ) :
    """'Cosine' function taking into account the uncertainties
    """
    fun = getattr ( x , '__cos__' , None )
    if fun : return fun()
    return math.cos ( x )

# =============================================================================
## define ``tan'' function 
def tan ( x ) :
    """'tangent' function taking into account the uncertainties
    """
    fun = getattr ( x , '__tan__' , None )
    if fun : return fun()
    return math.tan ( x )

# =============================================================================
## define ``sinh'' function 
def sinh ( x ) :
    """'Sinh' function taking into account the uncertainties
    """
    fun = getattr ( x , '__sinh__' , None )
    if fun : return fun()
    return math.sinh ( x )

# =============================================================================
## define ``cosh'' function 
def cosh ( x ) :
    """'Cosh' function taking into account the uncertainties
    """
    fun = getattr ( x , '__cosh__' , None )
    if fun : return fun()
    return math.cosh ( x )

# =============================================================================
## define ``tanh'' function 
def tanh ( x ) :
    """'tanh' function taking into account the uncertainties
    """
    fun = getattr ( x , '__tanh__' , None )
    if fun : return fun()
    return math.tanh ( x )

# =============================================================================
## define ``erf'' function 
#  @see https://en.wikipedia.org/wiki/Error_function
def erf ( x ) :
    """ Error function taking into account the uncertainties
    - see https://en.wikipedia.org/wiki/Error_function
    """
    fun = getattr ( x , '__erf__' , None )
    if fun : return fun()
    return math.erf ( x )

# =============================================================================
## define ``erfc'' function 
#  @see https://en.wikipedia.org/wiki/Error_function
def erfc ( x ) :
    """ Complemenatry function taking into account the uncertainties
    - see https://en.wikipedia.org/wiki/Error_function
    """
    fun = getattr ( x , '__erfc__' , None )
    if fun : return fun()
    return math.erfc ( x )

# =============================================================================
## define ``asin'' function 
def asin ( x ) :
    """'asin' function taking into account the uncertainties
    """
    fun = getattr ( x , '__asin__' , None )
    if fun : return fun()
    return math.asin ( x )

# =============================================================================
## define ``acos'' function 
def acos ( x ) :
    """'acos' function taking into account the uncertainties
    """
    fun = getattr ( x , '__acos__' , None )
    if fun : return fun()
    return math.acos ( x )

# =============================================================================
## define ``atan'' function 
def atan ( x ) :
    """'atan' function taking into account the uncertainties
    """
    fun = getattr ( x , '__atan__' , None )
    if fun : return fun()
    return math.atan ( x )

# =============================================================================
## define ``asinh'' function 
def asinh ( x ) :
    """'asinh' function taking into account the uncertainties
    """
    fun = getattr ( x , '__asinh__' , None )
    if fun : return fun()
    return math.asinh ( x )

# =============================================================================
## define ``acosh'' function 
def acosh ( x ) :
    """'acosh' function taking into account the uncertainties
    """
    fun = getattr ( x , '__acosh__' , None )
    if fun : return fun()
    return math.acosh ( x )

# =============================================================================
## define ``atanh'' function 
def atanh ( x ) :
    """'atanh' function taking into account the uncertainties
    """
    fun = getattr ( x , '__atanh__' , None )
    if fun : return fun()
    return math.atanh ( x )


# =============================================================================
## define ``tgamma'' function 
def tgamma ( x ) :
    """'tgamma' function taking into account the uncertainties
    """
    fun = getattr ( x , '__tgamma__' , None )
    if fun : return fun()
    return math.gamma ( x )

## define ``gamma'' function 
gamma = tgamma

# =============================================================================
## define ``lgamma'' function 
def lgamma ( x ) :
    """'lgamma' function taking into account the uncertainties
    """
    fun = getattr ( x , '__lgamma__' , None )
    if fun : return fun()
    return math.lgamma ( x )


_erfcx_ = cpp.Gaudi.Math.erfcx 
# =============================================================================
## define ``erfc'' function 
#  @see https://en.wikipedia.org/wiki/Error_function
def erfcx ( x ) :
    """ Scaled complementary error function taking into account the uncertainties
    - see https://en.wikipedia.org/wiki/Error_function
    """
    fun = getattr ( x , '__erfcx__' , None )
    if fun : return fun()
    return _erfcx_ ( x )

_probit_ = cpp.Gaudi.Math.probit  
# =============================================================================
## define ``probit'' function 
#  @see https://en.wikipedia.org/wiki/Probit
def probit ( x ) :
    """ Probit function taking into account the uncertainties
    - see https://en.wikipedia.org/wiki/Probit
    """
    fun = getattr ( x , '__probit__' , None )
    if fun : return fun()
    return _probit_ ( x )

_fma_ = cpp.Gaudi.Math.fma
# =============================================================================
## evaluate fma(x,y,z) = x*y+x 
#  @param y    (INPUT) the parameter 
#  @param x    (INPUT) the parameter 
#  @param z    (INPUT) the parameter 
#  @param cxy  (INPUT) the correlation coefficient   -1<=c_xy<=1 
#  @param cxz  (INPUT) the correlation coefficient   -1<=c_xz<=1 
#  @param cyz  (INPUT) the correlation coefficient   -1<=c_yz<=1 
#  @return  fma(x,y,z)
#  @warning invalid and small covariances are ignored
def fma ( x , y , z , cxy = 0 , cxz = 0 , cyz = 0 ) : 
    """ evaluate fma(x,y,z)=x*y+z witn uncertainties 
    """
    _x = VE ( x )
    _y = VE ( y )
    _z = VE ( z )
    return _fma_ ( _x , _y , _z , cxy , cxz , cyz )


_hypot_ = cpp.Gaudi.Math.hypot
# =============================================================================
## evaluate hypot(x,y) = sqrt(x*x+y*y)
#   \f$ \sqrt( x^2 + y^2 ) \f$
#  @param x (INPUT) the first parameter
#  @param y (INPUT) the second parameter
#  @param c (INPUT) the correlation coefficient  (-1<=c<=1)
#  @return the value of <code>hypot</code> function
#  @warning invalid and small covariances are ignored
def hypot ( x , y , c = 0 ) : 
    """ evaluate hypot(x,y)=sqrt{x*x+y*y} witn uncertainties 
    """
    _x = VE ( x )
    _y = VE ( y )
    return _hypot_ ( _x , _y , c )


# =============================================================================
if '__main__' == __name__ :

    ## make printout of the own documentations
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__
    print ' Version : %s ' %   __version__  
    print ' Symbols : %s ' % list (  __all__ ) 
    print '*'*120
    
    vars  = [ VE ( 0.001 , 0.0001**2 ) , VE(1,0) , VE(1,0.1**2) , VE(10,0.01**2) ]
    funcs = [ exp   , expm1  ,
              log   , log10  , log1p  , 
              sqrt  , cbrt   ,
              sin   , cos    , tan    ,
              sinh  , cosh   , tanh   ,
              asin  , acos   , atan   ,
              asinh , acosh  , atanh  ,
              erf   , erfc   , erfcx  , probit , 
              gamma , tgamma , lgamma ]

    ## use helper object:
    from LHCbMath.deriv import EvalVE 
    funcs += [ EvalVE ( math.sin , math.cos ) ,
               EvalVE ( math.sin )            ]
    
    for v in vars :
        print 'Var = ' , v 
        for f in funcs :
            print "\t%s\t%s =\t%s " % ( f.__name__ , v ,  f(v) ) 
            
             
# =============================================================================
# The  END
# =============================================================================
