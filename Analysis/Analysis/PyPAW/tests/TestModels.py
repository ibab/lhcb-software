#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file TestModels.py
#
#  tests for various signal models 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision: 172094 $
#  Last modification $Date: 2014-04-29 16:02:06 +0200 (Tue, 29 Apr 2014) $
#                 by $Author: albarano $
# =============================================================================
"""
Tests for various fit models  
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT
from   PyPAW.PyRoUts import *
from   PyPAW.Utils   import rooSilent 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__ : 
    logger = getLogger( 'PyPAW.TestModels' )
else : 
    logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Test for Fit models from Analysis/PyPAW')
## ============================================================================
## make simple test mass 
mass     = ROOT.RooRealVar ( 'test_mass' , 'Some test mass' , 3.0 , 3.2 )

## book very simple data set
varset0  = ROOT.RooArgSet  ( mass )
dataset0 = ROOT.RooDataSet ( dsID() , 'Test Data set-0' , varset0 )  

## fill it 
m = VE(3.100,0.015**2)
for i in xrange(0,5000) : 
    mass.setVal  ( m.gauss() )
    dataset0.add ( varset0   )

print dataset0

import PyPAW.FitModels as Models 


# =============================================================================
## gauss PDF
# =============================================================================
logger.info ('Test Gauss_pdf' ) 
signal_gauss = Models.Gauss_pdf ( 'Gauss'        ,
                                  mass.getMin () ,
                                  mass.getMax () ,
                                  mass = mass    )

signal_gauss . mean  . setVal ( m.value () )
signal_gauss . sigma . setVal ( m.error () )

## fit with fixed mass and sigma
with rooSilent() : 
    result, frame = signal_gauss . fitTo ( dataset0 )
    
## print 'Fit with everything fixed:' , result 
signal_gauss.mean .release()
signal_gauss.sigma.release()

with rooSilent() : 
    result, frame = signal_gauss . fitTo ( dataset0 )

## print 'Fit with everything released:' , result
print 'Mean & Sigma are: ', result('mean_Gauss')[0] , result('sigma_Gauss')[0]


model_gauss = Models.Fit1D( signal     = signal_gauss ,
                            background = Models.Bkg_pdf ('BkgGauss', mass = mass , power = 0 ) )
model_gauss.background.tau.fix(0)

with rooSilent() : 
    result, frame = model_gauss . fitTo ( dataset0 )
    result, frame = model_gauss . fitTo ( dataset0 )
    
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual()  ) )
    print result 
else :     
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]



# =============================================================================
## CrystalBall PDF
# =============================================================================
logger.info ('Test CrystalBall_pdf' )
model_cb = Models.Fit1D (
    signal = Models.CrystalBall_pdf ( 'CB' , mass.getMin() , mass.getMax()  ,
                                      mass  = mass               ,
                                      sigma = signal_gauss.sigma ,  
                                      mean  = signal_gauss.mean  ) ,
    background = model_gauss.background  )

model_cb.signal.alpha.fix(2) 
model_cb.signal.n    .fix(5) 

with rooSilent() : 
    result, frame = model_cb. fitTo ( dataset0 )
    model_cb.signal.alpha.release()
    result, frame = model_cb. fitTo ( dataset0 )
    result, frame = model_cb. fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]

# =============================================================================
## right side CrystalBall PDF
# =============================================================================
logger.info ('Test CrystalBallRS_pdf' )
model_cbrs = Models.Fit1D (
    signal = Models.CrystalBallRS_pdf ( 'CBRS' , mass.getMin() , mass.getMax()  ,
                                        mass  = mass               ,
                                        sigma = signal_gauss.sigma ,  
                                        mean  = signal_gauss.mean  ) ,
    background = model_gauss.background  )

model_cbrs.signal.alpha.fix(2) 
model_cbrs.signal.n    .fix(5) 

with rooSilent() : 
    result, frame = model_cbrs. fitTo ( dataset0 )
    model_cbrs.signal.alpha.release()
    result, frame = model_cbrs. fitTo ( dataset0 )
    result, frame = model_cbrs. fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]

# =============================================================================
## double sided CrystalBall PDF
# =============================================================================
logger.info ('Test CrystalBallDS_pdf' )
model_cbds = Models.Fit1D (
    signal = Models.CB2_pdf ( 'CBDS' , mass.getMin() , mass.getMax()  ,
                              mass  = mass               ,
                              sigma = signal_gauss.sigma ,  
                              mean  = signal_gauss.mean  ) ,
    background = model_gauss.background  )

model_cbds.signal.aL.fix(2) 
model_cbds.signal.nL.fix(5) 
model_cbds.signal.aR.fix(2.5) 
model_cbds.signal.nR.fix(10) 

with rooSilent() : 
    result, frame = model_cbds. fitTo ( dataset0 )
    model_cbds.signal.aL.release()
    model_cbds.signal.aR.release()
    result, frame = model_cbds. fitTo ( dataset0 )
    model_cbds.signal.aL.fix(5) 
    model_cbds.signal.aL.fix(5)    
    result, frame = model_cbds. fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]


# =============================================================================
## Needham PDF
# =============================================================================
logger.info ('Test Needham_pdf' )
model_matt = Models.Fit1D (
    signal = Models.Needham_pdf ( 'Matt' , mass.getMin()     , mass.getMax()  ,
                                  mass  = mass               ,
                                  sigma = signal_gauss.sigma ,  
                                  mean  = signal_gauss.mean  ) ,
    background = model_gauss.background  )

with rooSilent() : 
    result, frame = model_matt. fitTo ( dataset0 )
    model_matt.signal.mean .release()
    model_matt.signal.sigma.release()
    result, frame = model_matt. fitTo ( dataset0 )
    result, frame = model_matt. fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]




# ==========================================================================
## Apolonios
# ==========================================================================
logger.info ('Test Apolonios_pdf' ) 
model_apolonios = Models.Fit1D (
    signal = Models.Apolonios_pdf ( 'APO' , mass.getMin() , mass.getMax()  ,
                                    mass  = mass ,
                                    mean  = signal_gauss.mean    ,
                                    sigma = signal_gauss.sigma ,
                                    fixB     =  1 ,
                                    fixN     = 10 ,
                                    fixAlpha =  3 ) ,
    background = model_gauss.background  )

with rooSilent() : 
    result, frame = model_apolonios. fitTo ( dataset0 )
    result, frame = model_apolonios. fitTo ( dataset0 )
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]


# =============================================================================
## Bifurkated gauss PDF
# =============================================================================
logger.info ('Test BifurcatedGauss_pdf' ) 
signal_bifurcated = Models.BifurcatedGauss_pdf ( 'BFGauss'      ,
                                                 mass.getMin () ,
                                                 mass.getMax () ,
                                                 mean  = signal_gauss.mean  ,
                                                 sigma = signal_gauss.sigma ,
                                                 mass  = mass    )

signal_bifurcated . asym  . setVal ( 0          )

model_bifurcated = Models.Fit1D( signal     = signal_bifurcated       ,
                                 background = model_gauss.background  )

with rooSilent() : 
    result, frame = model_bifurcated . fitTo ( dataset0 )
    result, frame = model_bifurcated . fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else :     
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean   & Sigma      are: ', result ( 'mean_Gauss')[0] , result( 'sigma_Gauss' )[0]


# =============================================================================
## GenGaussV1
# ============================================================================= 
logger.info ('Test GenGaussV1_pdf' ) 
model_gauss_gv1 = Models.Fit1D (
    signal = Models.GenGaussV1_pdf ( 'Gv1' , mass.getMin() , mass.getMax()  ,
                                     mass = mass , mean = signal_gauss.mean ) ,
    background = model_gauss.background  )

model_gauss_gv1.signal.beta .fix(2)
model_gauss_gv1.signal.mean .fix( m.value() ) 

with rooSilent() : 
    result, frame = model_gauss_gv1. fitTo ( dataset0 )
    model_gauss_gv1.signal.alpha.release()
    result, frame = model_gauss_gv1. fitTo ( dataset0 )
    model_gauss_gv1.signal.mean .release() 
    result, frame = model_gauss_gv1. fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else :     
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean                is: ' , result ( 'mean_Gauss')[0] 

#
## GenGaussV2
#
logger.info ('Test GenGaussV2_pdf' ) 
model_gauss_gv2 = Models.Fit1D (
    signal = Models.GenGaussV2_pdf ( 'Gv2' , mass.getMin() , mass.getMax()  ,
                                     mass = mass , mean = signal_gauss.mean ) ,
    background = model_gauss.background  )

model_gauss_gv2.signal.kappa.fix(0)
model_gauss_gv2.b.fix(0)

with rooSilent() : 
    result, frame = model_gauss_gv2. fitTo ( dataset0 )
    model_gauss_gv2.signal.mean.release() 
    result, frame = model_gauss_gv2. fitTo ( dataset0 )
    model_gauss_gv2.signal.kappa.release() 
    result, frame = model_gauss_gv2. fitTo ( dataset0 )
    result, frame = model_gauss_gv2. fitTo ( dataset0 )
    model_gauss_gv2.b.release() 
    result, frame = model_gauss_gv2. fitTo ( dataset0 )

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else :
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean                is: ' , result ( 'mean_Gauss')[0] 


# =============================================================================
## SkewGauss
# =============================================================================
logger.info ('Test SkewGauss_pdf' ) 
model_gauss_skew = Models.Fit1D (
    signal = Models.SkewGauss_pdf ( 'GSk' , mass.getMin() , mass.getMax()  ,
                                    mass = mass , mean = signal_gauss.mean ) ,
    background = model_gauss.background  )

model_gauss_skew.signal.alpha.fix(0)

with rooSilent() : 
    result, frame = model_gauss_skew. fitTo ( dataset0 )
    result, frame = model_gauss_skew. fitTo ( dataset0 )
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean                is: ' , result ( 'mean_Gauss')[0] 


# =============================================================================
## Bukin
# =============================================================================
logger.info ('Test Bukin_pdf' ) 
model_bukin = Models.Fit1D (
    signal = Models.Bukin_pdf ( 'Bukin' , mass.getMin() , mass.getMax()  ,
                                mass  = mass ,
                                mean  = signal_gauss.mean  , 
                                sigma = signal_gauss.sigma ) ,
    background = model_gauss.background  )


model_bukin.signal.mean .fix  ( m.value() )
model_bukin.signal.sigma.fix  ( m.error() )
model_bukin.signal.sigma.fix  ( m.error() )
model_bukin.signal.rhol .fix  ( 0 )
model_bukin.signal.rhor .fix  ( 0 )
model_bukin.signal.xi   .fix  ( 0 )
with rooSilent() : 
    result, frame = model_bukin. fitTo ( dataset0 )
    model_bukin.signal.xi  .release()     
    result, frame = model_bukin. fitTo ( dataset0 )
    model_bukin.signal.rhol.release()     
    result, frame = model_bukin. fitTo ( dataset0 )
    model_bukin.signal.rhor.release()     
    result, frame = model_bukin. fitTo ( dataset0 )
    model_bukin.signal.mean .release() 
    model_bukin.signal.sigma.release() 
    result, frame = model_bukin. fitTo ( dataset0 )
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean                is: ' , result ( 'mean_Gauss')[0] 



# =============================================================================
## StudentT
# =============================================================================
logger.info ('Test StudentT_pdf' ) 
model_student = Models.Fit1D (
    signal = Models.StudentT_pdf ( 'ST' , mass.getMin() , mass.getMax()  ,
                                   mass = mass , mean = signal_gauss.mean ) ,
    background = model_gauss.background  )


with rooSilent() : 
    result, frame = model_student. fitTo ( dataset0 )
    result, frame = model_student. fitTo ( dataset0 )
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else : 
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'           )[0]
    print 'Mean                is: ' , result ( 'mean_Gauss')[0] 



# =============================================================================
## Breit-Wigner
# =============================================================================
logger.info ('Test BreitWigner_pdf' )
bw = cpp.Gaudi.Math.BreitWigner( m.value() ,
                                 m.error() ,
                                 0.150     ,
                                 0.150     , 1 )
model_bw = Models.Fit1D (
    signal = Models.BreitWigner_pdf ( 'BW' , mass.getMin() , mass.getMax()  ,
                                      bw                         ,     
                                      mass  = mass               ,
                                      mean  = signal_gauss.mean  ) , 
    background = model_gauss.background  )

model_bw.signal.mean.fix ( m.value() )
with rooSilent() : 
    result, frame = model_bw. fitTo ( dataset0 )
    model_bw.signal.mean .release()
    model_bw.signal.gamma.release()
    result, frame = model_bw. fitTo ( dataset0 )
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else :
    print 'Signal & Background are: ', result ( 'S'         )[0] , result( 'B'        )[0]
    print 'Mean   & Gamma      are: ', result ( 'mean_Gauss')[0] , result( 'gamma_BW' )[0]


# =============================================================================
## Voigt
# =============================================================================
logger.info ('Test Voigt_pdf' )
model_vgt = Models.Fit1D (
    signal = Models.Voigt_pdf ( 'V' , mass.getMin() , mass.getMax()  ,
                                mass  = mass                ,
                                mean  = signal_gauss.mean   , 
                                sigma = signal_gauss.sigma  ) , 
    background = model_gauss.background  )

model_vgt.signal.mean .fix ( m.value() )
model_vgt.signal.sigma.fix ( m.error() )
with rooSilent() : 
    result, frame = model_vgt. fitTo ( dataset0 )
    model_vgt.signal.mean.release()
    result, frame = model_vgt. fitTo ( dataset0 )
    result, frame = model_vgt. fitTo ( dataset0 )
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d ' % ( result.status() , result.covQual () ) )
    print result 
else :
    print 'Signal & Background are: ', result ( 'S'         )[0] , result ( 'B'       )[0]
    print 'Mean   & Gamma      are: ', result ( 'mean_Gauss')[0] , result ( 'gamma_V' )[0]
    

