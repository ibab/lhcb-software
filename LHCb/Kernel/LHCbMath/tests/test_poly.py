#!/usr/bin/env python
# # -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Test some polinomial functionality 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-02-10
#  
#                    $Revision: 183195 $
#  Last modification $Date: 2015-01-24 14:25:18 +0100 (Sat, 24 Jan 2015) $
#  by                $Author: ibelyaev $
# =============================================================================
""" Test some polinomial functionality 
"""
# =============================================================================
__version__ = "$Revision: 183195 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-02-10"
# =============================================================================
from LHCbMath.Types import cpp, doubles
from LHCbMath.deriv import Derivative,Integral 
import random,math 

_equal_  = cpp.LHCb.Math.Equal_To('double')()
def equal ( x , y ) :
    if _equal_ ( x, y ) : return True
    if max ( abs(x) , abs(y) ) > 1.e-8  :
        if abs( y ) > abs( x ) :  return abs(1-x/y)<1.e-10 
        else                   :  return abs(1-y/x)<1.e-10 
        
    return abs(x-y) < 1.e-10

def delta ( x , y ) :

    if   not _equal_ ( x , 0 ) : return abs(1.0*y/x-1.0)
    elif not _equal_ ( y , 0 ) : return abs(1.0*x/y-1.0)

    return abs(x-y) 
    
ps = cpp.Gaudi.Math.Polynomial   ( 5 ,   -5 , 10 )
ls = cpp.Gaudi.Math.LegendreSum  ( 5 ,   -1 , 9  )
cs = cpp.Gaudi.Math.ChebyshevSum ( 5 ,   -2 , 8  )
bs = cpp.Gaudi.Math.Bernstein    ( 5 ,   -3 , 7  )

for i in range(0,ps.npars() ) :
    
    ps.setPar ( i , random.uniform ( -10 , 10 ) ) 
    ls.setPar ( i , random.uniform ( -10 , 10 ) ) 
    cs.setPar ( i , random.uniform ( -10 , 10 ) ) 
    bs.setPar ( i , random.uniform ( -10 , 10 ) ) 


x0 = 0

## numerical integrals 
ps_i = Integral( ps , x0 )
ls_i = Integral( ls , x0 )
cs_i = Integral( cs , x0 )
bs_i = Integral( bs , x0 )

psi_ = ps.indefinite_integral ()
lsi_ = ls.indefinite_integral ()
csi_ = cs.indefinite_integral ()
bsi_ = bs.indefinite_integral ()

## integrals 
psi = lambda x : psi_(x)-psi_(x0)
lsi = lambda x : lsi_(x)-lsi_(x0)
csi = lambda x : csi_(x)-csi_(x0)
bsi = lambda x : bsi_(x)-bsi_(x0)


psd = ps.derivative ()
lsd = ls.derivative ()
csd = cs.derivative ()
bsd = bs.derivative ()

ps_d = Derivative ( ps )
ls_d = Derivative ( ls )
cs_d = Derivative ( cs )
bs_d = Derivative ( bs )


for t in [
    ( ps , psi , ps_i , psd , ps_d ) ,
    ( ls , lsi , ls_i , lsd , ls_d ) ,
    ( cs , csi , cs_i , csd , cs_d ) ,
    ( bs , bsi , bs_i , bsd , bs_d ) ,
    ] :
    
    for i in range(0,10) :

        fun = t[0]

        x = random.uniform (fun.xmin(),fun.xmax())

        
        f  = fun  (x)
        i1 = t[1] (x)               ## indefinite_integral  as object       
        i2 = t[2] (x)               ## numerical integration using SciPy 
        i3 = fun.integral(x0,x)
        d1 = t[3] (x)               ## derivative as object   
        d2 = t[4] (x)               ## numerical differentiation   
        d3 = fun.derivative(x) 

        _t = type(fun)
        
        if not equal ( i1 , i2 ) :
            print 'Integrals   1/2 are not equal: %s %g %g %g' % ( _t.__name__ , delta(i1,i2) , i1,i2 )
        if not equal ( i1 , i3 ) :
            print 'Integrals   1/3 are not equal: %s %g %g %g' % ( _t.__name__ , delta(i1,i3) , i1,i3 )
        if not equal ( d1 , d2 ) :
            print 'Derivatives 1/2 are not equal: %s %g %g %g' % ( _t.__name__ , delta(d1,d2) , d1, d2 )
        if not equal ( d1 , d3 ) :
            print 'Derivatives 1/3 are not equal: %s %g %g %g' % ( _t.__name__ , delta(d1,d3) , d1, d3 )
            
        

#
## Test integration with exponential 
#
BP      = cpp.Gaudi.Math.Bernstein
BB      = cpp.Gaudi.Math.Bernstein.Basic
integra = cpp.Gaudi.Math.integrate

xmin =  0
xmax =  1

tau  = +2.0 
bb   = BB ( 7 , 16 )
bp   = BP ( bb    , xmin , xmax )

def i_fun ( x ) :
    return bp ( x ) * math.exp( tau * x )

II = Integral( i_fun    , xlow = xmin )
a1 = integra ( bb , tau )
a2 = integra ( bp , tau , xmin , xmax )
a3 = integra ( bp , tau )
a4 = II(xmax)  

print 'R!' , a1  , a2 , a3 , a4 

    

# =============================================================================
if '__main__' == __name__ :

    print __doc__
    print 'Author :  %s ' % __author__
    print 'Version:  %s ' % __version__
    print 'Date   :  %s ' % __date__
    

# =============================================================================
# The END 
# =============================================================================
