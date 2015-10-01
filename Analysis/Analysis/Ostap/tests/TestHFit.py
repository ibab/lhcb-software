#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestHFit.py
#
#  test fit of histograms 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Tests for fits of histograms 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-03-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT,random
from   Ostap.PyRoUts import *
from   Ostap.Utils   import rooSilent 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__' == __name__ :
    logger = getLogger ( 'Ostap.TestHFit' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
logger.info ( 'Test for histogtram fits')
# =============================================================================
## make simple test 
mass2    = ROOT.RooRealVar ( 'test_mass2' , 'Some test mass' , 0 , 10 )
x        = mass2 

## book very simple data set
varset2  = ROOT.RooArgSet  ( mass2 )

import Ostap.FitModels as     Models 

events = 50000

logger.debug('Make a test data using Gamma-Distribution')
m_gamma0 = Models.GammaDist_pdf( 'GD0' , x )
m_gamma0.k    .setVal( 2 )
m_gamma0.theta.setVal( 2 )

dataset2 = m_gamma0.pdf.generate ( varset2 , events ) 
 

#
## 
h1 = ROOT.TH1D ( hID() , '' ,  10 , 0  , 10 ) ; h1.Sumw2() 
h2 = ROOT.TH1D ( hID() , '' ,  20 , 0  , 10 ) ; h2.Sumw2() 
h3 = ROOT.TH1D ( hID() , '' , 100 , 10 , 10 ) ; h3.Sumw2() 

random.seed(10) 
bins  = [ 0 ] 
for i in range(0,  9 ) : bins.append ( random.uniform ( 0 , 10 ) )
bins += [  10 ]
bins.sort()
h4   = h1_axis ( bins ) ## non-uniform bins
bins  = [ 0 ] 
for i in range(0, 19 ) : bins.append ( random.uniform ( 0 , 10 ) )
bins += [  10 ]
bins.sort()
h5   = h1_axis ( bins ) ## non-uniform bins
#

dataset2.project ( h1 , x . GetName () )
dataset2.project ( h2 , x . GetName () )
dataset2.project ( h3 , x . GetName () )
dataset2.project ( h4 , x . GetName () )
dataset2.project ( h5 , x . GetName () )

h1s = h1.rescale_bins ( 1 )
h2s = h2.rescale_bins ( 1 )
h3s = h3.rescale_bins ( 1 )
h4s = h4.rescale_bins ( 1 )
h5s = h5.rescale_bins ( 1 )

# =============================================================================
logger.info('Test  Gamma-Distribution')
# =============================================================================
m_gamma = Models.GammaDist_pdf( 'GD1' , x )

with rooSilent() : 
    result,f  = m_gamma.fitTo ( dataset2 )  
    result,f  = m_gamma.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tGamma:       k    = %s " % result( m_gamma.k.GetName()     )[0]
    print  "\tGamma:       theta= %s " % result( m_gamma.theta.GetName() )[0]   


# =============================================================================
logger.info("Test  2-expo-distribution")
# =============================================================================
m_2expo = Models.TwoExpos_pdf( '2exp' , x )
with rooSilent() : 
    result,f  = m_2expo.fitTo ( dataset2 )  
    result,f  = m_2expo.fitTo ( dataset2 )  

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\t2expo:       alpha= %s " % result( m_2expo . alpha     .GetName()  )[0]   
    print  "\t2expo:       delta= %s " % result( m_2expo . delta     .GetName()  )[0]   
    print  "\t2expo:          x0= %s " % result( m_2expo . x0        .GetName()  )[0]   


# =============================================================================
logger.info("Test  Poly(2)*Expo2 -Distribution")
# =============================================================================
m_p2e2 = Models.TwoExpoPoly_pdf('P2e2'  , x , power = 1 )
m_p2e2.x0.fix(0)
with rooSilent() : 
    result,f  = m_p2e2.fitTo ( dataset2 )  
    result,f  = m_p2e2.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tPoly2e2    alpha=  %s " % result( m_p2e2. alpha      .GetName()  )[0]   
    print  "\tPoly2e2    delta=  %s " % result( m_p2e2. delta     .GetName()  )[0]   
    for phi in m_p2e2.phis : 
        print  "\tPoly2e2:      phi=  %s " % phi.ve() 


models   = ( m_gamma , m_2expo, m_p2e2   )
histos   = ( h1  , h2  , h3  , h4  , h5  )
histos_s = ( h1s , h2s , h3s , h4s , h5s )

for h in histos :
    for m in models :
        r,f = m.fitTo ( h , silent = True )
        if 0 != r.status() or 3 != result.covQual() :
            logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( r.status() , r.covQual()  ) )
            print r

for h in histos_s :
    for m in models :
        r,f = m.fitTo ( h , density = True , silent = True )
        if 0 != r.status() or 3 != result.covQual() :
            logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( r.status() , r.covQual()  ) )
            print r
            


# =============================================================================
# The END 
# =============================================================================
