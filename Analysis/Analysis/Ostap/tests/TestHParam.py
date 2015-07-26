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

h1 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 10 ) ; h1.Sumw2() 
h2 = ROOT.TH1F( hID() , 'histogram(expo)' , 100, 0 , 10 ) ; h2.Sumw2() 

entries = 10000
## 
## random.seed(10) 
for i in xrange(0,entries) :
    v1 = random.expovariate( 1/3.0 )
    v2 = 10 - v1
    h1.Fill ( v1 )
    h2.Fill ( v2 )

h3 = 1.2*h1.minmax()[1].value() - h1
h4 = 1.2*h1.minmax()[1].value() - h2

#
# h1 - decreasing convex
# h2 - increasing convex
# h3 - increasing concave
# h4 - decreasing concave 

""" 
with timing ( 'Bershtein' ) :
    rB1 = h1.bernstein  ( 3 )
    rB2 = h2.bernstein  ( 3 )
    rB3 = h3.bernstein  ( 3 )
    rB4 = h4.bernstein  ( 3 )
    
with timing ( 'Chebyshev' ) : 
    rC1 = h1.chebyshev  ( 3 )
    rC2 = h2.chebyshev  ( 3 )
    rC3 = h3.chebyshev  ( 3 )
    rC4 = h4.chebyshev  ( 3 )

with timing ( 'Legendre' ) :
    rL1 = h1.legendre   ( 3 )
    rL2 = h2.legendre   ( 3 )
    rL3 = h3.legendre   ( 3 )
    rL4 = h4.legendre   ( 3 )

with timing ( 'Polynomial' ) : 
    rP1 = h1.polynomial ( 3 )
    rP2 = h2.polynomial ( 3 )
    rP3 = h3.polynomial ( 3 )
    rP4 = h4.polynomial ( 3 )

"""

with timing ( 'Fourier' ) :
    rF1 = h1.fourier   ( 3 )
    rF2 = h2.fourier   ( 3 )
    rF3 = h3.fourier   ( 3 )
    rF4 = h4.fourier   ( 3 )

## bad :-( 
with timing ( 'Positive' ) :
    rp1 = h1.positive  ( 3 )
    rp2 = h2.positive  ( 3 )
    rp3 = h3.positive  ( 3 )
    rp4 = h4.positive  ( 3 )

## good! :-) 
with timing ( 'Positive-PDF' ) :
    rp1p = h1.pdf_positive  ( 3 , silent = True )
    rp2p = h2.pdf_positive  ( 3 , silent = True )
    rp3p = h3.pdf_positive  ( 3 , silent = True )
    rp4p = h4.pdf_positive  ( 3 , silent = True )

print 'REGULAR',rp1
print 'PDF'    ,rp1p
"""
with timing ( 'Monothonic' ) :
    rm1 = h1.monothonic ( 3 , increasing = False )
    rm2 = h2.monothonic ( 3 , increasing = True  )
    rm3 = h3.monothonic ( 3 , increasing = True  )
    rm4 = h4.monothonic ( 3 , increasing = False )

with timing ( 'Concave' ) :
    rZ1 = h1.convex ( 3 , increasing = False , convex = True  )
    rZ2 = h2.convex ( 3 , increasing = True  , convex = True  )
    rZ3 = h3.convex ( 3 , increasing = True  , convex = False )
    rZ4 = h4.convex ( 3 , increasing = False , convex = False )

with timing ( 'Spline' ) :
    rS1 = h1.bSpline  ( 2 , 1  )
    rS2 = h2.bSpline  ( 2 , 1  )
    rS3 = h3.bSpline  ( 2 , 1  )
    rS4 = h4.bSpline  ( 2 , 1  )

## bad :-( 
with timing ( 'pSpline' ) :
    rb1 = h1.pSpline  ( 2 , 1  )
    rb2 = h2.pSpline  ( 2 , 1  )
    rb3 = h3.pSpline  ( 2 , 1  )
    rb4 = h4.pSpline  ( 2 , 1  )

with timing ( 'mSpline' ) :
    rM1 = h1.mSpline  ( 2 , 1  , increasing = False ) 
    rM2 = h2.mSpline  ( 2 , 1  , increasing = True  ) 
    rM3 = h3.mSpline  ( 2 , 1  , increasing = True  ) 
    rM4 = h4.mSpline  ( 2 , 1  , increasing = False )


with timing ( 'cSpline' ) :
    rc1 = h1.cSpline  ( 2 , 1  , increasing = False , convex = True  )
    rc2 = h2.cSpline  ( 2 , 1  , increasing = True  , convex = True  )
    rc3 = h3.cSpline  ( 2 , 1  , increasing = True  , convex = False )
    rc4 = h4.cSpline  ( 2 , 1  , increasing = False , convex = False )

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

"""


import Ostap.FitModels as Models

tmp    = ROOT.RooRealVar('tmp','',*h1.xminmax() )
degree = 3 
pdf    = Models.PolyPos_pdf (  'NAME' , tmp , degree )



# =============================================================================
# The END 
# =============================================================================
