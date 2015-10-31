#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file Test2DModels2.py
#
#  tests for various 2D-fit modelsa
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Tests for various 2D-fit modelsa
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random 
from   Ostap.PyRoUts import *
from   Ostap.Utils   import rooSilent 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__ or '__builtin__' == __name__ : 
    logger = getLogger( 'Ostap.Test2DModels2' )
else : 
    logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Test for 2D nonfactorizable fit models from Analysis/Ostap')
# =============================================================================
## make simple test mass 
m_x     = ROOT.RooRealVar ( 'mass_x' , 'Some test mass(X)' , 3 , 3.2 )
m_y     = ROOT.RooRealVar ( 'mass_y' , 'Some test mass(Y)' , 3 , 3.2 )

## book very simple data set
varset  = ROOT.RooArgSet  ( m_x , m_y )
dataset = ROOT.RooDataSet ( dsID() , 'Test Data set-1' , varset )  

m = VE(3.100,0.015**2)

N_ss =  5000
N_sb =  2500
N_bs =  2500
N_bb =  5000

random.seed(0)

## fill it : 5000 events  Gauss * Gauss 
for i in xrange(0,N_ss) : 
    m_x.setVal  ( m.gauss() )
    m_y.setVal  ( m.gauss() )
    dataset.add ( varset  )

## fill it : 2500 events  Gauss * const  
for i in xrange(0,N_sb) : 
    m_x.setVal  ( m.gauss() )
    m_y.setVal  ( random.uniform ( m_y.getMin() , m_y.getMax() ) ) 
    dataset.add ( varset  )

## fill it : 2500 events  const * Gauss
for i in xrange(0,N_bs) : 
    m_x.setVal  ( random.uniform ( m_x.getMin() , m_x.getMax() ) ) 
    m_y.setVal  ( m.gauss() )
    dataset.add ( varset  )

## fill it : 5000 events  const * const
for i in xrange(0,N_bb) :

    m_x.setVal ( random.uniform ( m_x.getMin() , m_x.getMax() ) ) 
    m_y.setVal ( random.uniform ( m_y.getMin() , m_y.getMax() ) ) 
    dataset.add ( varset  )


print dataset
# =============================================================================


import Ostap.FitModels as     Models 
models = [] 

# =============================================================================
## gauss as signal, const as background 
# =============================================================================
logger.info ('Simplest (factorized) fit model:  ( Gauss + const ) x ( Gauss + const ) ' )
model   = Models.Fit2D (
    signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
    signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y )
    )

model.signal1.sigma.fix ( m.error () )
model.signal2.sigma.fix ( m.error () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.bkg1   .tau  .fix ( 0 )
model.bkg2   .tau  .fix ( 0 )
model.bkgA   .tau  .fix ( 0 )
model.bkgB   .tau  .fix ( 0 )

## fit with fixed mass and sigma
with rooSilent() : 
    result, frame = model. fitTo ( dataset )
    model.signal1.sigma.release () 
    model.signal2.sigma.release ()
    model.signal1.mean .release () 
    model.signal2.mean .release () 
    result, frame = model. fitTo ( dataset )


if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d '
                   % ( result.status() , result.covQual()  ) )
    print result 

print '\nSxS=%s \nSxB=%s \nBxS=%s \nBxB=%s ' % ( result ( model.SS_name ) [0] ,
                                                 result ( model.SB_name ) [0] ,
                                                 result ( model.BS_name ) [0] ,
                                                 result ( model.BB_name ) [0] ) 

models.append ( model ) 
# =============================================================================
## gauss as signal, const as background 
# =============================================================================
logger.info ('Simplest (factorized) fit model:  ( Gauss + P2 ) x ( Gauss + P2 ) ' )
model   = Models.Fit2D (
    suffix   = '_2' , 
    signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
    signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y ) ,
    power1   = 2 , 
    power2   = 2 ,
    powerA   = 2 ,
    powerB   = 2 , 
    )

model.signal1.sigma.fix ( m.error () )
model.signal2.sigma.fix ( m.error () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.bkg1   .tau  .fix ( 0 )
model.bkg2   .tau  .fix ( 0 )
model.bkgA   .tau  .fix ( 0 )
model.bkgB   .tau  .fix ( 0 )

## fit with fixed mass and sigma
with rooSilent() : 
    result, frame = model. fitTo ( dataset )
    # model.signal1.sigma.release () 
    # model.signal2.sigma.release ()
    # model.signal1.mean .release () 
    # model.signal2.mean .release () 
    result, frame = model. fitTo ( dataset )


if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d '
                   % ( result.status() , result.covQual()  ) )
    print result 

print '\nSxS=%s \nSxB=%s \nBxS=%s \nBxB=%s ' % ( result ( model.SS_name ) [0] ,
                                                 result ( model.SB_name ) [0] ,
                                                 result ( model.BS_name ) [0] ,
                                                 result ( model.BB_name ) [0] ) 


models.append ( model ) 
# =============================================================================
## gauss as signal, const as background 
# =============================================================================
logger.info ('Simplest non-factorized fit model:  ( Gauss + P1 ) (x) ( Gauss + P1 ) + BB' )
model   = Models.Fit2D (
    suffix   = '_3' , 
    signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
    signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y ) ,
    power1   = 1 , 
    power2   = 1 ,
    bkg2D    = Models.PolyPos2D_pdf ( 'P2D' , m_x , m_y , nx = 2 , ny = 2 ) 
    )

model.signal1.sigma.fix ( m.error () )
model.signal2.sigma.fix ( m.error () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.bkg1   .tau  .fix ( 0 )
model.bkg2   .tau  .fix ( 0 )

## fit with fixed mass and sigma
with rooSilent() : 
    result, frame = model. fitTo ( dataset )
    # model.signal1.sigma.release () 
    # model.signal2.sigma.release ()
    # model.signal1.mean .release () 
    # model.signal2.mean .release () 
    result, frame = model. fitTo ( dataset )
    
    
if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d '
                   % ( result.status() , result.covQual()  ) )
    print result 

print '\nSxS=%s \nSxB=%s \nBxS=%s \nBxB=%s ' % ( result ( model.SS_name ) [0] ,
                                                 result ( model.SB_name ) [0] ,
                                                 result ( model.BS_name ) [0] ,
                                                 result ( model.BB_name ) [0] ) 


models.append ( model ) 
# =============================================================================
## gauss as signal, const as background 
# =============================================================================
logger.info ('Simplest non-factorized symmetric fit model:  ( Gauss + P1 ) (x) ( Gauss + P1 ) + BBsym' )
model   = Models.Fit2D (
    suffix   = '_4' , 
    signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
    signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y ) ,
    power1   = 1 , 
    power2   = 1 ,
    bkg2D    = Models.PolyPos2Dsym_pdf ( 'P2Ds' , m_x , m_y , n = 2 ) 
    )

model.signal1.sigma.fix ( m.error () )
model.signal2.sigma.fix ( m.error () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.bkg1   .tau  .fix ( 0 )
model.bkg2   .tau  .fix ( 0 )

## fit with fixed mass and sigma
with rooSilent() : 
    result, frame = model. fitTo ( dataset )
    # model.signal1.sigma.release () 
    # model.signal2.sigma.release ()
    # model.signal1.mean .release () 
    # model.signal2.mean .release () 
    result, frame = model. fitTo ( dataset )
    

if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d '
                   % ( result.status() , result.covQual()  ) )
    print result 

print '\nSxS=%s \nSxB=%s \nBxS=%s \nBxB=%s ' % ( result ( model.SS_name ) [0] ,
                                                 result ( model.SB_name ) [0] ,
                                                 result ( model.BS_name ) [0] ,
                                                 result ( model.BB_name ) [0] ) 


models.append ( model ) 
# =============================================================================
## gauss as signal, const as background 
# =============================================================================
logger.info ('Symmetrized non-factorized symmetric fit model:  ( Gauss + P1 ) (x) ( Gauss + P1 ) + BBsym' )
sb      = ROOT.RooRealVar('sb','SB',0,10000)
model   = Models.Fit2D (
    suffix   = '_5' , 
    signal_1 = Models.Gauss_pdf ( 'Gx' , m_x.getMin () , m_x.getMax () , mass = m_x ) ,
    signal_2 = Models.Gauss_pdf ( 'Gy' , m_y.getMin () , m_y.getMax () , mass = m_y ) ,
    power1   = 1 , 
    power2   = 1 ,
    bkg2D    = Models.PolyPos2Dsym_pdf ( 'P2Ds' , m_x , m_y , n = 2 ) ,
    sb = sb ,
    bs = sb 
    )

model.signal1.sigma.fix ( m.error () )
model.signal2.sigma.fix ( m.error () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.signal1.mean .fix ( m.value () )
model.signal2.mean .fix ( m.value () )
model.bkg1   .tau  .fix ( 0 )
model.bkg2   .tau  .fix ( 0 )

## fit with fixed mass and sigma
with rooSilent() : 
    result, frame = model. fitTo ( dataset )
    # model.signal1.sigma.release () 
    # model.signal2.sigma.release ()
    # model.signal1.mean .release () 
    # model.signal2.mean .release () 
    result, frame = model. fitTo ( dataset )


if 0 != result.status() or 3 != result.covQual() :
    logger.warning('Fit is not perfect MIGRAD=%d QUAL=%d '
                   % ( result.status() , result.covQual()  ) )
    print result 

print '\nSxS=%s \nSxB=%s \nBxS=%s \nBxB=%s ' % ( result ( model.SS_name ) [0] ,
                                                 result ( model.SB_name ) [0] ,
                                                 result ( model.BS_name ) [0] ,
                                                 result ( model.BB_name ) [0] ) 
print result 


models.append ( model ) 



#
## check that everything is serializable
# 
import Ostap.ZipShelve   as DBASE
with DBASE.tmpdb() as db : 
    db['x,y,vars'] = m_x, m_y, varset
    db['dataset' ] = dataset
    db['models'  ] = models
    db['result'  ] = result
    db['frame'   ] = frame
    db.ls() 

# =============================================================================
# The END 
# =============================================================================
