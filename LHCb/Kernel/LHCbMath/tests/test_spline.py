#!/usr/bin/env python
# # -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Test some spline functionality 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-02-10
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
""" Test some spline functionality 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-02-10"
# =============================================================================
from LHCbMath.Types import cpp
import random,math 
VD = cpp.vector('double')

knots = VD()
knots.push_back ( 0    )
knots.push_back ( 0.05 )
knots.push_back ( 0.1  )
knots.push_back ( 0.15 )
knots.push_back ( 0.3  )
knots.push_back ( 0.6  )
knots.push_back ( 1.0  )

myspline = cpp.Gaudi.Math.PositiveSpline   ( knots , 3 ) 
myspline = cpp.Gaudi.Math.MonothonicSpline ( knots , 3 , True ) 

for i in range(0,myspline.npars() ) :
    myspline.setPar ( i , random.uniform(-math.pi,math.pi) )
    
spline   = myspline.bspline()

print 'SPLINE-0, ' , spline.pars()

c0 = spline 
c1 = c0.derivative()
c2 = c1.derivative()
c3 = c2.derivative()
c4 = c3.derivative()

print 'SPLINE-0, ' , c0.pars()
print 'SPLINE-1, ' , c1.pars()
print 'SPLINE-2, ' , c2.pars()
print 'SPLINE-3, ' , c3.pars()
print 'SPLINE-4, ' , c3.pars()

from LHCbMath.deriv import Derivative 
c1_n = Derivative ( c0 )
c2_n = Derivative ( c1 )
c3_n = Derivative ( c2 )

from LHCbMath.deriv import Integral
i1   = c0.indefinite_integral() 
i1_n = Integral   ( c0 , 0 )

import random

for i in range(0,10 ) :
    x = ( 0.01 + float(i) ) /11 

    print x, c0(x), 'FIRST  :', c0.derivative(x)/c1_n(x) , c1(x)/c1_n(x) , 'SECOND :', c2(x)/c2_n(x)
    
    print 'INTEGRALS', c0.integral(0,x) , i1(x) , i1_n ( x )    


 
cN = cpp.Gaudi.Math.BSpline ( c0 )
for i in range ( 0 , cN.npars() ) : cN.setPar( i , 0 )

cs = []
for i in range ( 0 , cN.npars() ) :

    cc = cpp.Gaudi.Math.BSpline( cN )
    cc.setPar  ( i , 1 )
    cs.append ( cc )

for i in range(0,10 ) :
    
    x = ( 0.01 + float(i) ) / 11 

    s = 0
    for ic in cs :
        s += ic( x ) 
    print 'SUM:' , s 




# =============================================================================
# The END 
# =============================================================================
