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
"""Tests for various background polynomials models  
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-02-07"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT
from   Ostap.PyRoUts import *
from   Ostap.Utils   import rooSilent 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__' == __name__ :
    logger = getLogger ( 'Ostap.TestPoly' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
logger.info ( 'Test for some polynomials models from Analysis/Ostap')
# =============================================================================
## make simple test 
mass2    = ROOT.RooRealVar ( 'test_mass3' , 'Some test mass' , 2 , 10 )
x        = mass2 

## book very simple data set
varset2  = ROOT.RooArgSet  ( mass2 )

import Ostap.FitModels as     Models 

events = 10000

logger.debug('Make a test data using Gamma-Distribution')
m_gamma0 = Models.GammaDist_pdf( 'GD0' , x )
m_gamma0.k    .setVal( 2 )
m_gamma0.theta.setVal( 1 )

dataset2 = m_gamma0.pdf.generate ( varset2 , events ) 
 
models = []

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

models.append ( m_gamma ) 

# =============================================================================
logger.info("Test  Poly(4)-Distribution")
# =============================================================================
m_p4 = Models.PolyPos_pdf('P4'  , x , power = 4 )

with rooSilent() : 
    result,f  = m_p4.fitTo ( dataset2 )  
    result,f  = m_p4.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_p4.phis : 
        print  "\tPoly6:       phi= %s " % phi.ve() 
        
models.append ( m_p4 ) 

# =============================================================================
logger.info("Test  monothonic Poly(4)-Distribution")
# =============================================================================
m_d4 = Models.Monothonic_pdf('D4'  , x , power = 4 , increasing = False  )

with rooSilent() : 
    result,f  = m_d4.fitTo ( dataset2 )  
    result,f  = m_d4.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_d4.phis : 
        print  "\tPoly4:       phi= %s " % phi.ve() 

models.append ( m_d4 ) 

# =============================================================================
logger.info("Test  convex Poly(4)-Distribution")
# =============================================================================
m_c4 = Models.Convex_pdf('C4'  , x , power = 4 , increasing = False , convex = True  )

with rooSilent() : 
    result,f  = m_c4.fitTo ( dataset2 )  
    result,f  = m_c4.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_c4.phis : 
        print  "\tPoly4:       phi= %s " % phi.ve() 
        
models.append ( m_c4 ) 

# =============================================================================
logger.info("Test  Poly(4)*Expo -Distribution")
# =============================================================================
m_p4e = Models.Bkg_pdf('P4e'  , x , power = 4 )
m_p4e.tau.fix(-1.25)  
with rooSilent() : 
    result,f  = m_p4e.fitTo ( dataset2 )  
    m_p4e.tau.release() 
    result,f  = m_p4e.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tPoly4e       tau=  %s " % result( m_p4e. tau      .GetName()  )[0]   
    for phi in m_p4e.phis : 
        print  "\tPoly4e:      phi=  %s " % phi.ve() 

models.append ( m_p4e ) 

# =============================================================================
logger.info ("Test positive spline: order 3 with 2 inner knots ")
# =============================================================================
s3   = cpp.Gaudi.Math.PositiveSpline( x.xmin() , x.xmax() , 2 , 3 )
m_s3 = Models.PSpline_pdf ( 'S3' , x , s3 )

with rooSilent() : 
    result,f  = m_s3.fitTo ( dataset2 )  
    result,f  = m_s3.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_s3.phis : 
        print  "\tSpline3:     phi=  %s " % phi.ve() 

models.append ( m_s3 ) 

# =============================================================================
logger.info ("Test positive decreasing: order 3 with 2 inner knots ")
# =============================================================================
m3   = cpp.Gaudi.Math.MonothonicSpline( x.xmin() , x.xmax() , 2 , 3 , False )
m_m3 = Models.MSpline_pdf ( 'S3' , x , m3 )

with rooSilent() : 
    result,f  = m_m3.fitTo ( dataset2 )  
    result,f  = m_m3.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_m3.phis : 
        print  "\tSpline3:     phi=  %s " % phi.ve() 

models.append ( m_m3 ) 

# =============================================================================
logger.info ("Test positive decreasing convex: order 3 with 2 inner knots ")
# =============================================================================
c3   = cpp.Gaudi.Math.ConvexSpline( x.xmin() , x.xmax() , 2 , 3 , False , True )
m_c3 = Models.CSpline_pdf ( 'C3' , x , c3 )

with rooSilent() : 
    result,f  = m_c3.fitTo ( dataset2 )  
    result,f  = m_c3.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_c3.phis : 
        print  "\tSpline3:     phi=  %s " % phi.ve() 

models.append ( m_c3 ) 

#
## check that everything is serializable
# 
import Ostap.ZipShelve   as DBASE
with DBASE.tmpdb() as db : 
    db['mass,vars'] = mass2, varset2
    db['dataset'  ] = dataset2
    db['models'   ] = models
    db['result'   ] = result
    db['frame'    ] = f
    db.ls() 

# =============================================================================
# The END 
# =============================================================================
