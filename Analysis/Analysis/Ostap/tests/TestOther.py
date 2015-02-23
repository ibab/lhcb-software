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
import ROOT
from   Ostap.PyRoUts import *
from   Ostap.Utils   import rooSilent 
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
mass2    = ROOT.RooRealVar ( 'test_mass2' , 'Some test mass' , 0 , 10 )
x        = mass2 

## book very simple data set
varset2  = ROOT.RooArgSet  ( mass2 )


import Ostap.FitModels as     Models 

events = 5000

logger.debug('Make a test data using Gamma-Distribution')
m_gamma0 = Models.GammaDist_pdf( 'GD0' , x )
m_gamma0.k    .setVal( 2 )
m_gamma0.theta.setVal( 1 )

dataset2 = m_gamma0.pdf.generate ( varset2 , events ) 
 

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
logger.info('Test  GenGamma-Distribution')
# =============================================================================
m_gengamma = Models.GenGammaDist_pdf( 'GGD' , x )
m_gengamma.p    .fix(1) 
m_gengamma.low  .fix(0) 
m_gengamma.theta.fix(1) 
m_gengamma.k    .fix(2) 

with rooSilent() : 
    result,f  = m_gengamma.fitTo ( dataset2 )  
    m_gengamma.p    .release()  
    m_gengamma.theta.release()  
    m_gengamma.k    .release()  
    result,f  = m_gengamma.fitTo ( dataset2 )  
    

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tGenGamma:    k    = %s " % result( m_gengamma.k.GetName()     )[0]
    print  "\tGenGamma:    theta= %s " % result( m_gengamma.theta.GetName() )[0]   
    print  "\tGenGamma:    p    = %s " % result( m_gengamma.p.GetName()     )[0]
    print  "\tGenGamma:    low  = %s " % result( m_gengamma.low.GetName()   )[0]   

# =============================================================================
logger.info('Test  Amoroso-Distribution')
# =============================================================================
m_amoroso = Models.Amoroso_pdf( 'Amo' , x  , a = 0 )
m_amoroso.theta.setVal (1)
m_amoroso.alpha.setVal (2)
m_amoroso.beta .setVal (1)


with rooSilent() : 
    result,f  = m_amoroso.fitTo ( dataset2 )  
    result,f  = m_amoroso.fitTo ( dataset2 )  
    

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tAmoroso:     theta= %s " % result( m_amoroso.theta.GetName()  )[0]   
    print  "\tAmoroso:     alpha= %s " % result( m_amoroso.alpha.GetName()  )[0]   
    print  "\tAmoroso:     beta = %s " % result( m_amoroso.beta .GetName()  )[0]   
    print  "\tAmoroso:     a    = %s " % result( m_amoroso.a    .GetName()  )[0]   


# =============================================================================
logger.info('Test  LogGamma-Distribution')
# =============================================================================
m_loggamma = Models.LogGamma_pdf( 'LogG' , x  , alpha = 5 , nu = -2 , lam = 5 )

dataset3 = m_loggamma.pdf.generate ( varset2 , events ) 

m_loggamma.alpha.release() 
m_loggamma.lam  .release()
m_loggamma.nu   .release() 

with rooSilent() : 
    result,f  = m_loggamma.fitTo ( dataset3 )  
    result,f  = m_loggamma.fitTo ( dataset3 )   

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tLogGamma:    nu=    %s " % result( m_loggamma. nu     .GetName()  )[0]   
    print  "\tLogGamma:    lam=   %s " % result( m_loggamma. lam    .GetName()  )[0]   
    print  "\tLogGamma:    alpha= %s " % result( m_loggamma. alpha  .GetName()  )[0]   


# =============================================================================
logger.info('Test  log(Gamma)-Distribution')
# =============================================================================
m_lngamma = Models.LogGammaDist_pdf( 'logG' , x  )

dataset5 = m_lngamma.pdf.generate( varset2 , events )

with rooSilent() : 
    result,f  = m_lngamma.fitTo ( dataset5 )  
    result,f  = m_lngamma.fitTo ( dataset5 )  
    

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tLn(Gamma):   k=     %s " % result( m_lngamma. k     .GetName()  )[0]   
    print  "\tLn(Gamma):   theta= %s " % result( m_lngamma. theta .GetName()  )[0]   


# =============================================================================
logger.info('Test  log10(Gamma)-Distribution')
# =============================================================================
m_log10gamma = Models.Log10GammaDist_pdf( 'log10G' , x   )

dataset6 = m_log10gamma.pdf.generate( varset2 , events )

with rooSilent() : 
    result,f  = m_log10gamma.fitTo ( dataset6 )  
    result,f  = m_log10gamma.fitTo ( dataset6 )  
    

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tLog10(Gamma):k=     %s " % result( m_log10gamma. k     .GetName()  )[0]   
    print  "\tLog10(Gamma):theta= %s " % result( m_log10gamma. theta .GetName()  )[0]   


# =============================================================================
logger.info("Test  Landau-Distribution")
# =============================================================================
m_landau = Models.Landau_pdf( 'Landau'  , x , delta = 1.08 )
#
m_landau.scale.release () 
m_landau.scale.setVal  (0.466)
dataset4 = m_landau.pdf.generate( varset2 , 1000 )

#
with rooSilent() : 
    result,f  = m_landau.fitTo ( dataset4 )  
    m_landau.delta.release () 
    result,f  = m_landau.fitTo ( dataset4 )  

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tLandau:      scale= %s " % result( m_landau. scale     .GetName()  )[0]   
    print  "\tLandau:      delta= %s " % result( m_landau. delta     .GetName()  )[0]   


# =============================================================================
logger.info("Test  Beta'-Distribution")
# =============================================================================
m_betaprime = Models.BetaPrime_pdf( 'BetaP', x  , delta = 0 , alpha = 2 )

m_betaprime.beta  .setVal ( 300 )
m_betaprime.scale .setVal ( 300 )

with rooSilent() : 
    result,f  = m_betaprime.fitTo ( dataset2 )  
    result,f  = m_betaprime.fitTo ( dataset2 )  
    

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tBeta':       alpha= %s " % result( m_betaprime. alpha     .GetName()  )[0]   
    print  "\tBeta':       beta=  %s " % result( m_betaprime. beta      .GetName()  )[0]   
    print  "\tBeta':       scale= %s " % result( m_betaprime. scale     .GetName()  )[0]   
    print  "\tBeta':       delta= %s " % result( m_betaprime. delta     .GetName()  )[0]   



# =============================================================================
logger.info("Test  SinhAsinh-Distribution")
# =============================================================================
m_shash = Models.SinhAsinh_pdf( 'SASH' , x  )

m_shash.mu      .setVal (  0.79 )
m_shash.sigma   .setVal (  0.88 ) 
m_shash.epsilon .setVal ( -0.76 ) 
m_shash.delta   .setVal (  0.92 ) 

with rooSilent() : 
    result,f  = m_shash.fitTo ( dataset2 )  
    result,f  = m_shash.fitTo ( dataset2 )  
    m_shash.delta.release()
    result,f  = m_shash.fitTo ( dataset2 )  
    m_shash.epsilon.release()
    result,f  = m_shash.fitTo ( dataset2 )  
        
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tSinhAsinh:   mu   = %s " % result( m_shash. mu        .GetName()  )[0]   
    print  "\tSinhAsinh:   sigma= %s " % result( m_shash. sigma     .GetName()  )[0]   
    print  "\tSinhAsinh:   eps  = %s " % result( m_shash. epsilon   .GetName()  )[0]   
    print  "\tSinhAsinh:   delta= %s " % result( m_shash. delta     .GetName()  )[0]   

# =============================================================================
logger.info("Test  Argus-Distribution")
# =============================================================================
m_argus = Models.Argus_pdf( 'ARGUS' , x )
m_argus . shape.release()
m_argus . high .release()
m_argus . low  .release()
m_argus . high.fix    (  0 ) 
m_argus . low .fix    ( 10 )

with rooSilent() : 
    result,f  = m_argus.fitTo ( dataset2 )  
    result,f  = m_argus.fitTo ( dataset2 )  

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tArgus:       shape= %s " % result( m_argus . shape     .GetName()  )[0]   
    print  "\tArgus:       high = %s " % result( m_argus . high      .GetName()  )[0]   
    print  "\tArgus:       low  = %s " % result( m_argus . low       .GetName()  )[0]   


# =============================================================================
logger.info("Test  Adjusted Argus-Distribution")
# =============================================================================
m_argusa = Models.Argus_pdf( 'AA' , x )
m_argusa . shape.release()
m_argusa . high .release()
m_argusa . low  .release()
m_argusa . high.fix    ( 0 ) 
m_argusa . low .fix    ( 5 )
m_argusa . adjust ( 0.001 )

with rooSilent() : 
    result,f  = m_argusa.fitTo ( dataset2 )
    m_argusa . low      .release   ()
    m_argusa . p0_value .release ()
    result,f  = m_argusa.fitTo ( dataset2 )  

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    print  "\tArgusA:      shape= %s " % result( m_argusa . shape     .GetName()  )[0]   
    print  "\tArgusA:      high = %s " % result( m_argusa . high      .GetName()  )[0]   
    print  "\tArgusA:      low  = %s " % result( m_argusa . low       .GetName()  )[0]   
    print  "\tArgusA:      p0   = %s " % result( m_argusa . p0_value  .GetName()  )[0]   



# =============================================================================
logger.info("Test  Poly(6)-Distribution")
# =============================================================================
m_p6 = Models.PolyPos_pdf('P6'  , x , power = 6 )

with rooSilent() : 
    result,f  = m_p6.fitTo ( dataset2 )  
    result,f  = m_p6.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_p6.phis : 
        print  "\tPoly6:       phi= %s " % phi.ve() 
        


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


# =============================================================================
logger.info("Test  Poly(4)*PS -Distribution")
# =============================================================================
ps = cpp.Gaudi.Math.PhaseSpaceNL ( 0, 20 , 2  , 4 )  
m_ps = Models.PSPol_pdf('PS'  , x , ps , power = 4 )

with rooSilent() : 
    result,f  = m_ps.fitTo ( dataset2 )  
    result,f  = m_ps.fitTo ( dataset2 )  
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result
else :
    for phi in m_ps.phis : 
        print  "\tPoly3ps:     phi=  %s " % phi.ve() 


# =============================================================================
logger.info ("Test positive spline: order 3 with 5 inner knots ")
# =============================================================================
s3   = cpp.Gaudi.Math.PositiveSpline( x.xmin() , x.xmax() , 5 , 3 )
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


# =============================================================================
# The END 
# =============================================================================
