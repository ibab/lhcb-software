#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file TestCmp.py
#
#  tests for specific histo comparisons 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-11
# 
#                    $Revision: 172094 $
#  Last modification $Date: 2014-04-29 16:02:06 +0200 (Tue, 29 Apr 2014) $
#                 by $Author: albarano $
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
from   PyPAW.PyRoUts import *
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__'  == __name__ : 
    logger = getLogger ( 'PyPAW.TestCmp' )
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


## get the value 
v = VE(1,2**2)
## v = VE(0,1)

## fill
random.seed(10) 
for i in range ( 0, 5000 ) :

    g1 = v.gauss()
    g2 = v.gauss()
    g3 = v.gauss()
    g4 = v.gauss()
    
    h1g.Fill ( g1 ) 
    h2g.Fill ( g2 ) ## g1 )
    h3g.Fill ( g3 ) ## g1 )
    h4g.Fill ( g4 ) ## g1 )

    u1 = random.uniform ( -5 , 5 )
    u2 = random.uniform ( -5 , 5 )
    u3 = random.uniform ( -5 , 5 )
    u4 = random.uniform ( -5 , 5 )
    
    h1u.Fill ( u1 ) 
    h2u.Fill ( u2 ) ## u1 ) 
    h3u.Fill ( u3 ) ## u1 ) 
    h4u.Fill ( u4 ) ## u1 ) 
    

## @todo find better solution
import warnings
warnings.simplefilter("ignore")

logger.info( 'Compare gaussians (1) and (2)' )
# 
h1 = h1g
h2 = h2g
# 
r  = h1.cmp_fit   ( h2 , opts = 'WL' )
if r : logger.info    ( '(g1) vs (g2) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g1) vs (g2) : fit problems ')
ct = h1.cmp_cos   ( h2 , spline = True ) 
logger.info    ( '(g1) vs (g2) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist  ( h2 , spline = True ) 
logger.info    ( '(g1) vs (g2) : dist       is %s ' % dd )
h1.cmp_prnt ( h2 , '(g1) vs (g2) ' ) 

logger.info( 'Compare gaussians (1) and (3)' )
#
h1 = h1g
h2 = h2g
# 
r  = h1.cmp_fit  ( h2 , rescale = True , opts = 'WL' )
if r : logger.info    ( '(g1) vs (g3) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g1) vs (g3) : fit problems ')
ct = h1.cmp_cos  ( h2 , rescale = True , spline = True ) 
logger.info    ( '(g1) vs (g3) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist ( h2 , rescale = True , spline = True ) 
logger.info    ( '(g1) vs (g3) : dist       is %s ' % dd )
#
h1.cmp_prnt ( h2 , '(g1) vs (g3) ' ) 

logger.info( 'Compare gaussians (1) and (4)' )
#
h1 = h1g
h2 = h4g
# 
r  = h1.cmp_fit  ( h2 , rescale = True , opts = 'WL' )
if r : logger.info    ( '(g1) vs (g4) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g1) vs (g4) : fit problems ')
ct = h1.cmp_cos  ( h2 , rescale = True , spline = True ) 
logger.info      ( '(g1) vs (g4) : cos(theta) is %s' % ct )
dd = h1.cmp_dist ( h2 , rescale = True , spline = True ) 
logger.info    ( '(g1) vs (g4) : dist       is %s ' % dd )
# 
h1.cmp_prnt ( h2 , '(g1) vs (g4) ' ) 


logger.info( 'Compare gaussians (3) and (4)' )
#
h1 = h3g
h2 = h4g
# 
r  = h1.cmp_fit ( h2, rescale = True , opts = '' )
if r : logger.info    ( '(g3) vs (g4) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g3) vs (g4) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(g3) vs (g4) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info    ( '(g3) vs (g4) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(g3) vs (g4) ' ) 


logger.info( 'Compare uniform (1) and (2)' )
#
h1 = h1u
h2 = h2u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(u1) vs (u2) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(u1) vs (u2) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(u1) vs (u2) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(u1) vs (u2) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(u1) vs (u2) ' ) 

logger.info( 'Compare uniform (1) and (3)' )
#
h1 = h1u
h2 = h3u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(u1) vs (u3) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(u1) vs (u3) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(u1) vs (u3) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(u1) vs (u3) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(u1) vs (u3) ' ) 


logger.info( 'Compare uniform (1) and (4)' )
#
h1 = h1u
h2 = h4u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(u1) vs (u4) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(u1) vs (u4) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(u1) vs (u4) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(u1) vs (u4) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(u1) vs (u4) ' ) 
                
logger.info( 'Compare uniform (3) and (4)' )
#
h1 = h3u
h2 = h4u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(u3) vs (u4) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(u3) vs (u4) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(u3) vs (u4) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(u3) vs (u4) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(u3) vs (u4) ' ) 

logger.info( 'Compare gaussian (1) and uniform (1)' )
#
h1 = h1g
h2 = h1u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(g1) vs (u1) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g1) vs (u1) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(g1) vs (u1) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(g1) vs (u1) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(g1) vs (u1) ' ) 

logger.info( 'Compare gaussian (1) and uniform (3)' )
#
h1 = h1g
h2 = h3u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(g1) vs (u3) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g1) vs (u3) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(g1) vs (u3) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(g1) vs (u3) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(g1) vs (u3) ' ) 


logger.info( 'Compare gaussian (4) and uniform (4)' )
#
h1 = h4g
h2 = h4u
# 
r  = h1.cmp_fit ( h2, rescale = True  , opts = '' )
if r : logger.info    ( '(g4) vs (u4) : fit probability is %g%% ' % ( r.Prob()*100 ) )
else : logger.warning ( '(g4) vs (u4) : fit problems ')
ct = h1.cmp_cos ( h2 , rescale = True , spline = True ) 
logger.info     ( '(g4) vs (u4) : cos(theta) is %s ' % ct )
dd = h1.cmp_dist( h2 , rescale = True , spline = True ) 
logger.info     ( '(g4) vs (u4) : dist       is %s ' % dd )

h1.cmp_prnt ( h2 , '(g4) vs (u4) ' ) 


# =============================================================================
# The END 
# =============================================================================
