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
import ROOT, random 
from LHCbMath.Types import cpp
# =============================================================================    
try :
    ## just as a sugar, not really needed for the test  
    from   Ostap.PyRoUts import *
    import Ostap.Models
    ostap = True
except:
    ostap = False
    pass
# =============================================================================    

xmin      =   -1
xmax      =    4
xminmax   = xmin,xmax

_integrate_exp_ = cpp.Gaudi.Math.integrate

def integrate_exp ( poly , low , high ) :
    return _integrate_exp_ ( poly , 1.0 , low , high ) 
    
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


    for i in range( 0, 1000 ) :
        
        x = random.uniform ( xmin , xmax )
        
        vo = o ( x )
        vp = p ( x )
        vc = c ( x )
        vl = l ( x )
        vb = b ( x )
        if abs ( vp  - vo ) > 5.e-14 or \
           abs ( vc  - vo ) > 5.e-14 or \
           abs ( vl  - vo ) > 5.e-14 or \
           abs ( vb  - vo ) > 5.e-14 :
            print T.__name__, x , vo , vp-vo , vc-vo , vl-vo , vb-vo

    ilow  = xmin + 0.3 * ( xmax - xmin )
    ihigh = xmin + 0.7 * ( xmax - xmin )
    
    io = integrate_exp ( o , ilow , ihigh ) 
    ip = integrate_exp ( p , ilow , ihigh ) 
    ic = integrate_exp ( c , ilow , ihigh ) 
    il = integrate_exp ( l , ilow , ihigh ) 
    ib = integrate_exp ( b , ilow , ihigh ) 
    
    if abs ( ip  - io ) > 5.e-13 or \
       abs ( ic  - io ) > 5.e-13 or \
       abs ( il  - io ) > 5.e-13 or \
       abs ( ib  - io ) > 5.e-13 :
        print 'INTEGRALS:', T.__name__, io , ip-io , ic-io , il-io , ib-io
    
# =============================================================================
if '__main__' == __name__ :

    print __doc__
    print 'Author :  %s ' % __author__
    print 'Version:  %s ' % __version__
    print 'Date   :  %s ' % __date__
    

# =============================================================================
# The END 
# =============================================================================
