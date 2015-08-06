#!/usr/bin/env python
# # -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Test some additional polinomial functionality,
#  in particular basic transformation 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-02-10
#  
#                    $Revision: 183195 $
#  Last modification $Date: 2015-01-24 14:25:18 +0100 (Sat, 24 Jan 2015) $
#  by                $Author: ibelyaev $
# =============================================================================
""" Test additional polynomial functionality,in particluar basic transformation 
"""
# =============================================================================
__version__ = "$Revision: 183195 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-02-10"
# =============================================================================
import ROOT, random, math 
from LHCbMath.Types import cpp
from LHCbMath.deriv import Integral
# =============================================================================    
ostap = False 
try :
    ## just as a sugar, not really needed for the test  
    from   Ostap.PyRoUts import *
    import Ostap.Models
    ostap = True
except:
    ostap = False
    pass
# =============================================================================    

ostap = False 

xmin      =   -1
xmax      =    4
xminmax   = xmin,xmax

_integrate_exp_ = cpp.Gaudi.Math.integrate
_exponent_      = 3.7

def integrate_exp ( poly , low , high ) :
    return _integrate_exp_ ( poly , _exponent_ , low , high ) 

scale     = 2**20 
SE        = cpp.StatEntity 
counters  = {}
integrals = {}

for T in ( cpp.Gaudi.Math.Polynomial   ,
           cpp.Gaudi.Math.Bernstein    ,
           cpp.Gaudi.Math.LegendreSum  ,
           cpp.Gaudi.Math.ChebyshevSum ) :
    
    print 'TEST', T.__name__
    
    o = T( 4, *xminmax )
    for i in range ( 0 , o.npars() ) :
        x = random.uniform (-5,5) 
        o.setPar( i, x )
    
    p = cpp.Gaudi.Math.Polynomial   ( o )
    c = cpp.Gaudi.Math.ChebyshevSum ( o ) 
    l = cpp.Gaudi.Math.LegendreSum  ( o )
    b = cpp.Gaudi.Math.Bernstein    ( o )
    
    if hasattr ( o ,  'draw' ) and ostap : 
        from Ostap.Canvas import getCanvas
        cc = getCanvas()
        o.draw ()
        p.draw ('same')
        c.draw ('same')
        l.draw ('same')
        b.draw ('same')
        cc >> '%s' % T.__name__
        
        o._tf1.SetLineColor(1)
        p._tf1.SetLineColor(2)
        c._tf1.SetLineColor(3)
        l._tf1.SetLineColor(4)
        b._tf1.SetLineColor(8)

    n1 = type(o).__name__
    n1 = n1[ n1.rfind('::') + 2 :]
    
    for t in ( p , c , l , b ) :

        n2 = type(t).__name__ 
        n2 = n2[ n2.rfind('::') +2 : ]
        
        counters  [ n1 , n2 ] = SE() 
        integrals [ n1 , n2 ] = SE() 

    integrals [ n1 , 'Numeric' ] = SE() 

    def i_fun ( x ) :
        return o ( x ) * math.exp ( _exponent_ * x )
    
    for i in range( 0, 10000 ) :
        
        x  = random.uniform ( xmin , xmax )
        x2 = random.uniform ( xmin , xmax )
        
        vo = o ( x )        
        io = integrate_exp ( o , x , x2 )

        # numerical integral 
        IN = Integral     ( i_fun , x ) 
        ni = IN           ( x2        )
        
        integrals [ n1 , 'Numeric' ] += (ni-io) * scale 

        for t in ( p , c , l , b ) :
            
            n2 = type(t).__name__ 
            n2 = n2[ n2.rfind('::') +2 : ]
            
            vt = t(x)
            d  = vt - vo
            counters [ n1 , n2 ] += d * scale 
            
            it = integrate_exp ( t , x , x2 )
            
            integrals [ n1 , n2 ] += (it-io) * scale 
            


def summary ( cnts ) :
    
    keys = cnts.keys()
    keys.sort() 
    for k in keys :
        
        cnt = cnts[k]
        
        m      = cnt.mean() / scale
        r      = cnt.rms () / scale
        mn, mx = cnt.minmax()
        mn    /= scale
        mx    /= scale
        m      = m.toString ( '( %0.4g +- %0.4g)' )
        print " %12s -> %-12s : %-20s %.3g min/max=%.3g/%.3g" % ( k[0], k[1] , m , r , mn , mx )
        
        
print 'VALUES '
summary ( counters   )
print 'INTEGRALS'
summary ( integrals  )

# =============================================================================
if '__main__' == __name__ :

    print __doc__
    print 'Author :  %s ' % __author__
    print 'Version:  %s ' % __version__
    print 'Date   :  %s ' % __date__
    

# =============================================================================
# The END 
# =============================================================================
