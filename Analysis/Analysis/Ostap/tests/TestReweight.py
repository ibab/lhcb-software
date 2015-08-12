#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestReweigt.py
#
#  Test for reweighting machinery
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Test for reweighting machinery
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random, math, os 
from   Ostap.PyRoUts   import *
import Ostap.ZipShelve as     DBASE
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__'  == __name__ : 
    logger = getLogger ( 'Ostap.TestReweight' )
else : 
    logger = getLogger ( __name__ )
# =============================================================================    
RAD = ROOT.RooAbsData
if RAD.Tree != RAD.getDefaultStorageType() :
    logger.info ('DEFINE default storage type to be TTree! ')
    RAD.setDefaultStorageType ( RAD.Tree )
# =============================================================================
logger.info ( 'Test for Reweighting machinery')
# =============================================================================
testdatadb = 'testdata.db'
if not os.path.exists( testdatadb ) :
    #
    logger.info ( 'Test RANDOM data will be generated' )   
    ## prepare"data" histogram: lets use simple exponential and non-equidistance bins 
    hdata = h1_axis( [    i   for i in range(0,20) ] +
                     [ 20+i*2 for i in range(0,20) ] +
                     [ 60+i*4 for i in range(0,10) ] + [ 100 ] ) 
    for i in range( 0,100000 ) :
        v = random.random()
        v = -20*math.log(v)
        hdata.Fill(v)
        
    #
    ## prepare MC-dataset: use Gamma-distribution 
    #
    x        = ROOT.RooRealVar( 'x' , 'variable'   ,    0 , 100 )
    ## book very simple data set
    import Ostap.FitModels as     Models 
    m_gamma0 = Models.GammaDist_pdf('GD0'  , x )
    mean     =  35.0
    variance =  30**2 
    m_gamma0.k    .setVal( mean**2 /variance )
    m_gamma0.theta.setVal( variance/mean     )
    varset4  = ROOT.RooArgSet  ( x )
    dataset4 = m_gamma0.pdf.generate ( varset4 , 200000 )
    mctree   = dataset4.store().tree()
    
    ## store DATA in DBASE 
    logger.info ( 'Test data will be stored in DBASE "%s"' % testdatadb  )   
    with DBASE.open( testdatadb , 'c' ) as db : 
        db['DATAhistorgam'] = hdata 
        db['MCtree']        = mctree

#
## read data from DB
#
logger.info ( 'Test data will be fetched from DBASE "%s"' % testdatadb )   
with DBASE.open ( testdatadb , 'r' ) as db :
    hdata  = db['DATAhistorgam']
    mctree = db['MCtree']        

## prepare template histogram for MC 
hmc = ROOT.TH1D('hMC','histo-template for MC', 60,0,100 ) ; hmc.Sumw2() 

## prepare re-weighting machinery 
maxIter = 10  

## check database 
dbname = 'reweighting.db'
import os
if not os.path.exists( dbname ) :
    logger.info('Create new weights DBASE') 
    db = DBASE.open ( dbname , 'c' ) ##  create new empty db 
    db.close()
else :
    logger.info('Existing weights DBASE will be used') 
    
#
## make reweigthing iterations
# 
from Ostap.Reweighting import Weight, makeWeights 
from Ostap.Selectors   import SelectorWithVars 

## start iterations:
for iter in range ( 0 , maxIter ) :

    weighting = [
        ## variable          address in DB    
        ( lambda s : s.x , 'x-reweighting'  ) , 
        ]
    
    weighter   = Weight( dbname , weighting )
    ## variables to be used in MC-dataset 
    variables  = [
        ( 'pt_x'   , 'pt_x'   , 0      , 100   , lambda s : s.x            ) , 
        ( 'weight' , 'weight' , -1e+5  , 1.e+5 , lambda s : weighter ( s ) )  
        ]
    
    #
    ## create new "weighted" mcdataset
    # 
    selector = SelectorWithVars (
        variables ,
        '0<x && x<100 '
        )
    
    mctree.process ( selector )
    mcds = selector.data             ## new reweighted dataset

    #
    ## update weights
    #
    
    plots    = [
        ( 'pt_x'   , 'weight' , 'x-reweighting'  , hdata , hmc )  
        ]
    
    more = makeWeights ( mcds , plots , dbname , delta = 0.001 )

    ## make MC-histogram 
    mcds .project  ( hmc , 'pt_x' , 'weight'  )
    
    if 0 == iter % 2 or not more : 
        logger.info    ( 'Compare DATA and MC for iteration #%d' % iter )
        #
        ## compare the basic properties: mean, rms, skewness and kurtosis
        # 
        hdata.cmp_prnt ( hmc , 'DATA' , 'MC' , 'DATA vs MC' )
        #
        ## calculate the distances
        #
        dist = hdata.cmp_dist ( hmc , rescale = True )
        logger.info ('DATA-MC "distance"      %s' % dist )
        #
        ## calculate the 'orthogonality'
        #  
        cost = hdata.cmp_cos  ( hmc , rescale = True )
        logger.info ('DATA-MC "orthogonality" %s' % cost )
        #
        ## try to fit it DATA with MC and vice versa 
        #
        fit1 = hdata.cmp_fit ( hmc   , rescale = True )
        if fit1 and 0 == fit1.Status() :
            logger.info ( 'Fit DATA with MC   Prob=%.3g[%%] ' % ( fit1.Prob() * 100 ) )
        fit2 = hmc  .cmp_fit ( hdata , rescale = True )
        if fit2 and 0 == fit2.Status() :
            logger.info ( 'Fit MC   with DATA Prob=%.3g[%%] ' % ( fit2.Prob() * 100 ) )
        #
        ## make chi2-comparison between data and MC
        #
        c2ndf,prob = hdata.cmp_chi2 ( hmc   , rescale = True )
        logger.info ( 'DATA/MC: chi2/ndf (%.4g) and Prob %.5g%% ' % ( c2ndf , prob*100 ) )
        c2ndf,prob = hmc  .cmp_chi2 ( hdata , rescale = True )
        logger.info ( 'MC/DATA: chi2/ndf (%.4g) and Prob %.5g%% ' % ( c2ndf , prob*100 ) )

    if not more : 
        logger.info    ( 'No more iterations are needed #%d' % iter )
        break

    del    mcds , selector

    
# =============================================================================
# The END 
# =============================================================================
