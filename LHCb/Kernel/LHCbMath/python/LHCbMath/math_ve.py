#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#
#  Simple file to provide soem wrapper function for dealing with
#  Gaudi::Math::ValueWithError objects 
#  @see Gaudi::Math::ValueWithError
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-06-02
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author: ibelyaev $
#
#
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-02"
__version__ = "$Revision:$"
# =============================================================================
__all__     = (
    'exp'  , 'log'  , 'log10' ,
    'sqrt' , 'cbrt' , 'pow'   ,   
    'sin'  , 'cos'  , 'tan'   , 
    'sinh' , 'cosh' , 'tanh'  
    )
# =============================================================================
import math
from   LHCbMath.Types import cpp
VE    = cpp.Gaudi.Math.ValueWithError
_zero = cpp.LHCb.Math.Zero('double')() 
# =============================================================================


# =============================================================================
## define ``exp'' function 
def exp ( x ) :
    """
    'exp' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__exp__() 
    return math.exp ( x )

# =============================================================================
## define ``log'' function 
def log ( x ) :
    """
    'log' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__log__() 
    return math.log ( x )

# =============================================================================
## define ``log10'' function 
def log10 ( x ) :
    """
    'log10' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__log10__() 
    return math.log10 ( x )

# =============================================================================
## define ``sqrt'' function 
def sqrt ( x ) :
    """
    'sqrt' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__sqrt__() 
    return math.sqrt ( x )

# =============================================================================
## define ``cbrt'' function 
def cbrt ( x ) :
    """
    'cbrt' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__cbrt__() 
    return math.pow ( x , 1.0/3.0 )

# =============================================================================
## define ``pow'' function 
def pow ( x , y , *a ) :
    """
    'pow' function taking into account the uncertainties
    """
    if   isinstance ( x , VE ) or isinstance ( y , VE ) : return x**y 
    return math.pow ( x , y , *a ) 


# =============================================================================
## define ``sin'' function 
def sin ( x ) :
    """
    'Sine' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__sin__() 
    return math.sin ( x )

# =============================================================================
## define ``cos'' function 
def cos ( x ) :
    """
    'cos' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__cos__() 
    return math.cos ( x )

# =============================================================================
## define ``tan'' function 
def tan ( x ) :
    """
    'tan' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__tan__()
    return math.tan ( x )

# =============================================================================
## define ``sinh'' function 
def sinh ( x ) :
    """
    'Sinh' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__sinh__() 
    return math.sin ( x )

# =============================================================================
## define ``cosh'' function 
def cosh ( x ) :
    """
    'cosh' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__cosh__() 
    return math.cos ( x )

# =============================================================================
## define ``tanh'' function 
def tanh ( x ) :
    """
    'tanh' function taking into account the uncertainties
    """
    if isinstance ( x , VE ) : return x.__tanh__()
    return math.tan ( x )


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
    funcs = [ exp  , log  , log10 ,
              sqrt , cbrt ,
              sin  , cos  , tan  ,
              sinh , cosh , tanh ]
    
    for v in vars :
        print 'Var = ' , v 
        for f in funcs :
            print "\t%s\t%s =\t%s " % ( f.__name__ , v ,  f(v) ) 
            
             
# =============================================================================
# The  END
# =============================================================================
