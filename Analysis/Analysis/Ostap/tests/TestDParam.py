#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestModels.py
#
#  tests for various signal models 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Tests for various background fit models  
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random 
from   Ostap.PyRoUts import *
from   Ostap.Utils   import rooSilent, timing 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__' == __name__ :
    logger = getLogger ( 'Ostap.TestOther' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
logger.info ( 'Test for signal it models from Analysis/Ostap')
# =============================================================================
## make simple test 
mass4    = ROOT.RooRealVar ( 'test_mass4' , 'Some test mass' , 0 , 10 )
x        = mass4 

## book very simple data set
varset4  = ROOT.RooArgSet  ( mass4 )


dataset4 = ROOT.RooDataSet ( dsID() , 'Test Data set-4' , varset4 ) 
dataset5 = ROOT.RooDataSet ( dsID() , 'Test Data set-5' , varset4 ) 

events = 40000

## random.seed(10) 
for i in xrange ( 0 , events ) :
    
    v1 = random.expovariate( 1/3.0 )
    if not 0 < v1 < 10 : continue 

    x.setVal ( v1 )
    dataset4.add( varset4 )
    
    v2 = 10 - v1 
    x.setVal ( v2 )
    dataset5.add( varset4 )



import Ostap.FitModels as Models

positive   = Models.PolyPos_pdf    ( 'PP'  , mass4 , power = 3 )
increasing = Models.Monothonic_pdf ( 'PI'  , mass4 , power = 3 , increasing = True  )
decreasing = Models.Monothonic_pdf ( 'PD'  , mass4 , power = 3 , increasing = False )
inc_convex = Models.Convex_pdf     ( 'PIX' , mass4 , power = 3 , increasing = True  , convex = True  )
dec_convex = Models.Convex_pdf     ( 'PDX' , mass4 , power = 3 , increasing = False , convex = True  )
convex     = Models.ConvexOnly_pdf ( 'PX'  , mass4 , power = 3 , convex = True  )
concave    = Models.ConvexOnly_pdf ( 'PX'  , mass4 , power = 3, convex = False )


with timing ('Positive   5') , rooSilent() :
    r5,f = positive.fitTo ( dataset5 )
with timing ('Increasing 5') , rooSilent()  :
    i5, f = increasing.fitTo ( dataset5 )
with timing ('Convex     5') , rooSilent() :
    x4, f = inc_convex.fitTo ( dataset5 )
with timing ('ConvexOnly 5') , rooSilent() :
    c4, f = convex.fitTo ( dataset5 )


## logger.info ( 'Positive   pars: %s' % positive  .pdf.function().pars() )
## logger.info ( 'Increasing pars: %s' % increasing.pdf.function().pars() )
## logger.info ( 'Convex     pars: %s' % inc_convex.pdf.function().pars() )


for i in ( positive , increasing , inc_convex , convex ) :
    pars = i.pdf.function().bernstein().pars()
    pars = list ( pars )
    pars.reverse()  
    logger.info ( 'Common pars: %s' % list ( pars )  )

with timing ('Positive   4') , rooSilent()  :
    r4,f = positive.fitTo ( dataset4 )
with timing ('Decreasing 4') , rooSilent() :
    d4, f = decreasing.fitTo ( dataset4 )
with timing ('Convex     4') , rooSilent()  :
    x4, f = dec_convex.fitTo ( dataset4 )
with timing ('ConvexOnly 4') , rooSilent()  :
    c4, f = convex.fitTo ( dataset4 )

## logger.info ( 'Positive   pars: %s' % positive  .pdf.function().pars() )
## logger.info ( 'Decreasing pars: %s' % decreasing.pdf.function().pars() )
## logger.info ( 'Convex     pars: %s' % dec_convex.pdf.function().pars() )

for i in ( positive , decreasing , dec_convex , convex ) :
    pars = i.pdf.function().bernstein().pars()
    pars = list ( pars ) 
    logger.info ( 'Common pars: %s' % list ( pars )  )

pos_sp_ = cpp.Gaudi.Math.PositiveSpline   ( x.getMin() , x.getMax() , 1 , 3 )
inc_sp_ = cpp.Gaudi.Math.MonothonicSpline ( x.getMin() , x.getMax() , 1 , 3 , True  )
dec_sp_ = cpp.Gaudi.Math.MonothonicSpline ( x.getMin() , x.getMax() , 1 , 3 , False )
ix_sp_  = cpp.Gaudi.Math.ConvexSpline     ( x.getMin() , x.getMax() , 1 , 3 , True  , True )
dx_sp_  = cpp.Gaudi.Math.ConvexSpline     ( x.getMin() , x.getMax() , 1 , 3 , False , True )


pos_sp  = Models.PSpline_pdf ( 'SP'  , mass4 , pos_sp_ )
inc_sp  = Models.MSpline_pdf ( 'SI'  , mass4 , inc_sp_ )
dec_sp  = Models.MSpline_pdf ( 'SD'  , mass4 , dec_sp_ )
ix_sp   = Models.CSpline_pdf ( 'SIX' , mass4 ,  ix_sp_ )
dx_sp   = Models.CSpline_pdf ( 'SDX' , mass4 ,  dx_sp_ )


with timing ('Spline Positive   5') , rooSilent() :
    r5,f = pos_sp.fitTo ( dataset5 )
with timing ('Spline Increasing 5') , rooSilent()  :
    i4, f = inc_sp.fitTo ( dataset5 )
with timing ('Spline Convex     5') , rooSilent() :
    x4, f = ix_sp.fitTo ( dataset5 )

## logger.info ( 'Positive   pars: %s' % pos_sp.pdf.function().pars() )
## logger.info ( 'Increasing pars: %s' % inc_sp.pdf.function().pars() )
## logger.info ( 'Convex     pars: %s' %  ix_sp.pdf.function().pars() )


for i in (pos_sp , inc_sp , ix_sp ) :
    pars = i.pdf.function().bspline().pars()
    pars = list ( pars )
    pars.reverse()  
    logger.info ( 'Common pars: %s' % pars )
    
with timing ('Spline Positive   4') , rooSilent()  :
    r4,f = pos_sp.fitTo ( dataset4 )
with timing ('Spline Decreasing 4') , rooSilent() :
    d4, f = dec_sp.fitTo ( dataset4 )
with timing ('Spline Convex     4') , rooSilent()  :
    x4, f = dx_sp.fitTo ( dataset4 )

## logger.info ( 'Positive   pars: %s' % pos_sp.pdf.function().pars() )
## logger.info ( 'Decreasing pars: %s' % dec_sp.pdf.function().pars() )
## logger.info ( 'Convex     pars: %s' %  dx_sp.pdf.function().pars() )

for i in (pos_sp , dec_sp , dx_sp ) :
    pars = i.pdf.function().bspline().pars()
    pars = list ( pars ) 
    logger.info ( 'Common pars: %s' % pars )

# =============================================================================
# The END 
# =============================================================================
