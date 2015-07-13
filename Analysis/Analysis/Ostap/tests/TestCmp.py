#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestCmp.py
#
#  tests for specific histo comparisons 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Tests for various background fit models  
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random
from   Ostap.PyRoUts import *
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__'  == __name__ : 
    logger = getLogger ( 'Ostap.TestCmp' )
else : 
    logger = getLogger ( __name__ )
# =============================================================================
logger.info ( 'Test for specific historgam comparisons')
# =============================================================================


#
## histos for gaussian distributions
# 

h1g   = ROOT.TH1D ( hID() , '' ,  40 , -5 , 5 ) ; h1g.Sumw2() 
h2g   = ROOT.TH1D ( hID() , '' ,  40 , -5 , 5 ) ; h2g.Sumw2() 
h3g   = ROOT.TH1D ( hID() , '' ,  20 , -5 , 5 ) ; h3g.Sumw2() 

bins  = [ -5 ]
## 
random.seed(10) 
for i in range(0, 15 ) : bins.append ( random.uniform ( -5 , 5 ) )
bins += [  5 ]
bins.sort()

h4g   = h1_axis ( bins ) 

#
## histos for uniform distributions
# 
h1u = h1g.Clone( hID() ) 
h2u = h2g.Clone( hID() )  
h3u = h3g.Clone( hID() )  
h4u = h4g.Clone( hID() ) 

#
## histos for exponential distributions
# 
h1e = h1g.Clone( hID() ) 
h2e = h2g.Clone( hID() )  
h3e = h3g.Clone( hID() )  
h4e = h4g.Clone( hID() ) 


## get the value 
v = VE(1,1.75**2)
## v = VE(0,1)

random.seed(10) 
for i in range ( 0, 50000 ) :

    g1 = v.gauss()
    g2 = v.gauss()
    g3 = g2          ## the same as g2 
    g4 = v.gauss()
    
    h1g.Fill ( g1 ) 
    h2g.Fill ( g2 ) 
    h3g.Fill ( g3 ) 
    h4g.Fill ( g4 ) 
    
    u1 = random.uniform ( -5 , 5 )
    u2 = random.uniform ( -5 , 5 )
    u3 = u2          #3 the same as u2 
    u4 = random.uniform ( -5 , 5 )
    
    h1u.Fill ( u1 ) 
    h2u.Fill ( u2 ) 
    h3u.Fill ( u3 ) 
    h4u.Fill ( u4 ) 

    e1 = -1 * random.expovariate( -0.5 )  -5 
    e2 = -1 * random.expovariate( -0.5 )  -5 
    e3 = e2  ## the same as e2 
    e4 = -1 * random.expovariate( -0.5 )  -5 
    if not -5 < e1 < 5 : continue 
    if not -5 < e2 < 5 : continue 
    if not -5 < e3 < 5 : continue 
    if not -5 < e4 < 5 : continue 

    h1e.Fill ( e1 ) 
    h2e.Fill ( e2 ) 
    h3e.Fill ( e3 ) 
    h4e.Fill ( e4 ) 
    
def compare ( h1 , h2 ) :

    r1  = h1.cmp_fit       ( h2 , opts = 'WL' )
    if r1 : logger.info    ( 'h1 vs h2 : fit probability is %g%% ' % ( r1.Prob()*100 ) )
    else  : logger.warning ( 'h1 vs h2 : fit problems ')
    
    r2  = h2.cmp_fit       ( h1 , opts = 'WL' )
    if r2 : logger.info    ( 'h2 vs h1 : fit probability is %g%% ' % ( r2.Prob()*100 ) )
    else  : logger.warning ( 'h2 vs h1 : fit problems ')
    
    ct  = h1.cmp_cos      ( h2 , spline = True , rescale = True ) 
    logger.info           ( 'h1 vs h2 : cos(theta)      is %s ' % ct  )
    
    dd1 = h1.cmp_dist     ( h2 , spline = True , rescale = True ) 
    logger.info           ( 'h1 vs h2 : distance        is %s ' % dd1 )
    
    dd2 = h1.cmp_dist2    ( h2 , spline = True , rescale = True ) 
    logger.info           ( 'h1 vs h2 : distance2       is %s ' % dd2 )
    
    h1.cmp_prnt ( h2 , 'h1 vs h2 ' ) 

    ## @see http://www.itl.nist.gov/div898/handbook/eda/section3/eda35g.htm
    ## ks1 = h1.KolmogorovTest ( h2 )
    ## logger.info           ( 'h1 vs h2 : Kolmogorov test    %s ' % ks1 )
    
    ## ks2 = h2.KolmogorovTest ( h1 )
    ## logger.info           ( 'h2 vs h1 : Kolmogorov test    %s ' % ks2 )
    


## @todo find better solution
## import warnings
## warnings.simplefilter("ignore")

logger.info( 'Compare gaussians    (1) and (2)' )
compare ( h1g , h2g )
logger.info( 'Compare gaussians    (1) and (3)' )
compare ( h1g , h3g )
logger.info( 'Compare gaussians    (1) and (4)' )
compare ( h1g , h4g )
logger.info( 'Compare gaussians    (2) and (3) : should be the same!' )
compare ( h2g , h3g )
logger.info( 'Compare gaussians    (2) and (4)' )
compare ( h2g , h4g )
logger.info( 'Compare gaussians    (3) and (4)' ) 
compare ( h3g , h4g )

logger.info( 'Compare uniforms     (1) and (2)' )
compare ( h1u , h2u )
logger.info( 'Compare uniforms     (1) and (3)' )
compare ( h1u , h3u )
logger.info( 'Compare uniforms     (1) and (4)' )
compare ( h1u , h4u )
logger.info( 'Compare uniforms     (2) and (3) : should be the same!' )
compare ( h2u , h3u )
logger.info( 'Compare uniforms     (2) and (4)' )
compare ( h2u , h4u )
logger.info( 'Compare uniforms     (3) and (4)' ) 
compare ( h3u , h4u )

logger.info( 'Compare exponentials (1) and (2)' )
compare ( h1e , h2e )
logger.info( 'Compare exponentials (1) and (3)' )
compare ( h1e , h3e )
logger.info( 'Compare exponentials (1) and (4)' )
compare ( h1e , h4e )
logger.info( 'Compare exponentials (2) and (3) : should be the same!' )
compare ( h2e , h3e )
logger.info( 'Compare exponentials (2) and (4)' )
compare ( h2e , h4e )
logger.info( 'Compare exponentials (3) and (4)' ) 
compare ( h3e , h4e )

_ig = 0 
for ig in ( h1g , h2g , h3g , h4g ) :
    _ig += 1
    _iu  = 0 
    for iu in ( h1u , h2u , h3u , h4u ) :
        _iu += 1 
        logger.info( 'Compare gaussian  (%d) and uniform     (%d)' % ( _ig , _iu ) )
        compare ( ig , iu )

_ig = 0 
for ig in ( h1g , h2g , h3g , h4g ) :
    _ig += 1
    _ie  = 0 
    for ie in ( h1e , h2e , h3e , h4e ) :
        _ie += 1 
        logger.info( 'Compare gaussian  (%d) and exponential (%d)' % ( _ig , _iu ) )
        compare ( ig , ie )
        
_iu = 0 
for iu in ( h1u , h2u , h3u , h4u ) :
    _iu += 1
    _ie  = 0 
    for ie in ( h1e , h2e , h3e , h4e ) :
        _ie += 1 
        logger.info( 'Compare uniform (%d) and exponential (%d)' % ( _iu , _iu ) )
        compare ( iu , ie )


# =============================================================================
# The END 
# =============================================================================
