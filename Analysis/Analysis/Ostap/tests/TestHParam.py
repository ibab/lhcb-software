#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestHParam.py
#
#  tests some histo parametrezations 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Tests some histo parametrezations 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random
from   Ostap.PyRoUts import *
from   Ostap.Utils   import timing 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__'  == __name__ : 
    logger = getLogger ( 'Ostap.TestHParam' )
else : 
    logger = getLogger ( __name__ )
# =============================================================================
logger.info ( 'Test for various parameterizations of histograms')
# =============================================================================

from Ostap.HParamDeco import legendre_sum, chebyshev_sum

#
## histos for exponential distribution 
#

h1 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 1 ) ; h1.Sumw2() 
h2 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 1 ) ; h2.Sumw2() 
h3 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 1 ) ; h3.Sumw2() 
h4 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 1 ) ; h4.Sumw2() 
h5 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 1 ) ; h5.Sumw2() 
h6 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 1 ) ; h6.Sumw2() 

#f1 = ROOT.TF1('f3','exp(-x/3.)'       ,0,10)
#f2 = ROOT.TF1('f4','exp(-(10-x)/3.)'  ,0,10)
#f3 = ROOT.TF1('f3','1-exp(-x/3.)'     ,0,10)
#f4 = ROOT.TF1('f4','1-exp(-(10-x)/3.)',0,10)

f1 = ROOT.TF1('f3','(x-1)**2'         ,0,1)
f2 = ROOT.TF1('f4','x**2'             ,0,1)
f3 = ROOT.TF1('f3','1-(x-1)**2'       ,0,1)
f4 = ROOT.TF1('f4','1-x**2'           ,0,1)
f5 = ROOT.TF1('f5','4*(x-0.5)**2'     ,0,1)
f6 = ROOT.TF1('f5','1-4*(x-0.5)**2'   ,0,1)

entries = 10000
## 
## random.seed(10) 
for i in xrange(0,entries) :
    h1.Fill ( f1.GetRandom() )
    h2.Fill ( f2.GetRandom() )
    h3.Fill ( f3.GetRandom() )
    h4.Fill ( f4.GetRandom() )
    h5.Fill ( f5.GetRandom() )
    h6.Fill ( f6.GetRandom() )

    
# h1 - decreasing convex
# h2 - increasing convex
# h3 - increasing concave
# h4 - decreasing concave 
# h5 - non-monothonic convex
# h6 - non-monothonic concave


with timing ( 'Bershtein' ) :
    rB1 = h1.bernstein  ( 3 )
    rB2 = h2.bernstein  ( 3 )
    rB3 = h3.bernstein  ( 3 )
    rB4 = h4.bernstein  ( 3 )
    rB5 = h5.bernstein  ( 3 )
    rB6 = h6.bernstein  ( 3 )
    
with timing ( 'Chebyshev' ) : 
    rC1 = h1.chebyshev  ( 3 )
    rC2 = h2.chebyshev  ( 3 )
    rC3 = h3.chebyshev  ( 3 )
    rC4 = h4.chebyshev  ( 3 )
    rC5 = h5.chebyshev  ( 3 )
    rC6 = h6.chebyshev  ( 3 )

with timing ( 'Legendre' ) :
    rL1 = h1.legendre   ( 3 )
    rL2 = h2.legendre   ( 3 )
    rL3 = h3.legendre   ( 3 )
    rL4 = h4.legendre   ( 3 )
    rL5 = h5.legendre   ( 3 )
    rL6 = h6.legendre   ( 3 )

with timing ( 'Polynomial' ) : 
    rP1 = h1.polynomial ( 3 )
    rP2 = h2.polynomial ( 3 )
    rP3 = h3.polynomial ( 3 )
    rP4 = h4.polynomial ( 3 )
    rP5 = h5.polynomial ( 3 )
    rP6 = h6.polynomial ( 3 )

with timing ( 'Fourier' ) :
    rF1 = h1.fourier    ( 3 )
    rF2 = h2.fourier    ( 3 )
    rF3 = h3.fourier    ( 3 )
    rF4 = h4.fourier    ( 3 )
    rF5 = h5.fourier    ( 3 )
    rF6 = h6.fourier    ( 3 )

with timing ( 'Cosine' ) :
    rC1 = h1.cosine     ( 3 )
    rC2 = h2.cosine     ( 3 )
    rC3 = h3.cosine     ( 3 )
    rC4 = h4.cosine     ( 3 ) 
    rC5 = h5.cosine     ( 3 )
    rC6 = h6.cosine     ( 3 ) 

## bad :-( 
with timing ( 'Positive' ) :
    rp1 = h1.positive   ( 3 )
    rp2 = h2.positive   ( 3 )
    rp3 = h3.positive   ( 3 )
    rp4 = h4.positive   ( 3 )

## good! :-) 
with timing ( 'Positive-PDF' ) :
    rp1p = h1.pdf_positive  ( 3 , silent = True )
    rp2p = h2.pdf_positive  ( 3 , silent = True )
    rp3p = h3.pdf_positive  ( 3 , silent = True )
    rp4p = h4.pdf_positive  ( 3 , silent = True )

with timing ( 'Monothonic' ) :
    rm1 = h1.monothonic     ( 3 , increasing = False )
    rm2 = h2.monothonic     ( 3 , increasing = True  )
    rm3 = h3.monothonic     ( 3 , increasing = True  )
    rm4 = h4.monothonic     ( 3 , increasing = False )
    
with timing ( 'Monothonic-PDF' ) :
    rm1 = h1.pdf_decreasing ( 3 , silent = True )
    rm2 = h2.pdf_increasing ( 3 , silent = True )
    rm3 = h3.pdf_increasing ( 3 , silent = True )
    rm4 = h4.pdf_decreasing ( 3 , silent = True )

with timing ( 'Convex' ) :
    rZ1 = h1.convex ( 3 , increasing = False , convex = True  )
    rZ2 = h2.convex ( 3 , increasing = True  , convex = True  )
    rZ3 = h3.convex ( 3 , increasing = True  , convex = False )
    rZ4 = h4.convex ( 3 , increasing = False , convex = False )

with timing ( 'Convex-PDF' ) :
    rZ1 = h1.pdf_convex_decreasing  ( 3 , silent = True , density = False )
    rZ2 = h2.pdf_convex_increasing  ( 3 , silent = True , density = False )
    rZ3 = h3.pdf_concave_increasing ( 3 , silent = True , density = False )
    rZ4 = h4.pdf_concave_decreasing ( 3 , silent = True , density = False )
    
with timing ( 'Convex/ConcavePoly' ) :
    rZ1 = h1.convexpoly  ( 3 )
    rZ2 = h2.convexpoly  ( 3 )
    rZ3 = h3.concavepoly ( 3 )
    rZ4 = h4.concavepoly ( 3 )
    rZ5 = h5.convexpoly  ( 3 )
    rZ6 = h6.concavepoly ( 3 )

with timing ( 'Convex/ConcavePoly-PDF' ) :
    rZ1 = h1.pdf_convexpoly  ( 2 , silent = True , density = False )
    rZ2 = h2.pdf_convexpoly  ( 3 , silent = True , density = False )
    rZ3 = h3.pdf_concavepoly ( 3 , silent = True , density = False )
    rZ4 = h4.pdf_concavepoly ( 3 , silent = True , density = False )
    rZ5 = h5.pdf_convexpoly  ( 3 , silent = True , density = False )
    rZ6 = h6.pdf_concavepoly ( 3 , silent = True , density = False )

with timing ( 'Spline' ) :
    rS1 = h1.bSpline  ( 2 , 1  )
    rS2 = h2.bSpline  ( 2 , 1  )
    rS3 = h3.bSpline  ( 2 , 1  )
    rS4 = h4.bSpline  ( 2 , 1  )
    rS5 = h5.bSpline  ( 2 , 1  )
    rS6 = h6.bSpline  ( 2 , 1  )
    
## bad :-( 
with timing ( 'pSpline' ) :
    rb1 = h1.pSpline  ( 2 , 1  )
    rb2 = h2.pSpline  ( 2 , 1  )
    rb3 = h3.pSpline  ( 2 , 1  )
    rb4 = h4.pSpline  ( 2 , 1  )
    rb5 = h5.pSpline  ( 2 , 1  )
    rb6 = h6.pSpline  ( 2 , 1  )

## good :-) 
with timing ( 'pSpline-PDF' ) :
    rb1p = h1.pdf_pSpline  ( spline = ( 2 , 1 ) , silent = True , density = False )
    rb2p = h2.pdf_pSpline  ( spline = ( 2 , 1 ) , silent = True , density = False )
    rb3p = h3.pdf_pSpline  ( spline = ( 2 , 1 ) , silent = True , density = False )
    rb4p = h4.pdf_pSpline  ( spline = ( 2 , 1 ) , silent = True , density = False )
    rb5p = h5.pdf_pSpline  ( spline = ( 2 , 1 ) , silent = True , density = False )
    rb6p = h6.pdf_pSpline  ( spline = ( 2 , 1 ) , silent = True , density = False )

## bad :-( 
with timing ( 'mSpline' ) :
    rM1 = h1.mSpline  ( 2 , 1  , increasing = False ) 
    rM2 = h2.mSpline  ( 2 , 1  , increasing = True  ) 
    rM3 = h3.mSpline  ( 2 , 1  , increasing = True  ) 
    rM4 = h4.mSpline  ( 2 , 1  , increasing = False )

## good :-) 
with timing ( 'mSpline-PDF' ) :
    rM1p = h1.pdf_mSpline  ( (2,1,False) , silent = True  , density = True ) 
    rM2p = h2.pdf_mSpline  ( (2,1,True ) , silent = True  , density = True ) 
    rM3p = h3.pdf_mSpline  ( (2,1,True ) , silent = True  , density = True )
    rM4p = h4.pdf_mSpline  ( (2,1,False) , silent = True  , density = True ) 


## bad :-(
with timing ( 'cSpline' ) :
    rc1 = h1.cSpline  ( 2 , 1  , increasing = False , convex = True  )
    rc2 = h2.cSpline  ( 2 , 1  , increasing = True  , convex = True  )
    rc3 = h3.cSpline  ( 2 , 1  , increasing = True  , convex = False )
    rc4 = h4.cSpline  ( 2 , 1  , increasing = False , convex = False )

## good :-)
with timing ( 'cSpline-PDF' ) :
    rc1p = h1.pdf_cSpline  ( ( 2 , 1  , False , True  ) , silent = True , density=False )
    rc2p = h2.pdf_cSpline  ( ( 2 , 1  , True  , True  ) , silent = True , density=False )
    rc3p = h3.pdf_cSpline  ( ( 2 , 1  , True  , False ) , silent = True , density=False )
    rc4p = h4.pdf_cSpline  ( ( 2 , 1  , False , False ) , silent = True , density=False )

with timing ( 'convexSpline' ) :
    rc1 = h1.convexSpline  ( 2 , 1  )
    rc2 = h2.convexSpline  ( 2 , 1  )
    rc3 = h3.concaveSpline ( 2 , 1  )
    rc4 = h2.concaveSpline ( 2 , 1  )
    rc5 = h2.convexSpline  ( 2 , 1  )
    rc6 = h2.concaveSpline ( 2 , 1  )

with timing ( 'convex/concaveSpline-PDF' ) :
    rc1p = h1.pdf_convexSpline   ( ( 2 , 1 ) , silent = True , density=False )
    rc2p = h2.pdf_convexSpline   ( ( 2 , 1 ) , silent = True , density=False )
    rc3p = h3.pdf_concaveSpline  ( ( 2 , 1 ) , silent = True , density=False )
    rc4p = h4.pdf_concaveSpline  ( ( 2 , 1 ) , silent = True , density=False )
    rc5p = h5.pdf_convexSpline   ( ( 2 , 1 ) , silent = True , density=False )
    rc6p = h6.pdf_concaveSpline  ( ( 2 , 1 ) , silent = True , density=False )


#
## another splines 
#

templ = ROOT.TH1F( hID() , 'histogram(expo)' , 12 , 0 , 10 )  
h1c   = h1.rebinNumbers ( templ )
h2c   = h2.rebinNumbers ( templ )
h3c   = h3.rebinNumbers ( templ )
h4c   = h4.rebinNumbers ( templ )


sp1 = h1c.asSpline()
sp2 = h2c.asSpline()
sp3 = h3c.asSpline()
sp4 = h4c.asSpline()


# =============================================================================
# The END 
# =============================================================================
