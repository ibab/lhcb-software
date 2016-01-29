#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestTMVA.py
#
#  Test for TMVA machinery
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-10-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Test for TVMA machinery
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-10-26"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT, random, os 
from   array              import array
from   Ostap.PyRoUts      import *
from   Ostap.TFileDeco    import ROOTCWD 
from   Ostap.progress_bar import progress_bar 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__'  == __name__ : 
    logger = getLogger ( 'Ostap.TestTMVA' )
else : 
    logger = getLogger ( __name__ )
# =============================================================================    
data_file = 'tmva_data.root'
tmva_file = 'TMVA_MLP_weights.xml'

if not os.path.exists( data_file ) :

    nB = 20000
    nS = 10000
    logger.info('Prepare input ROOT file with data')
    with ROOT.TFile.Open( data_file ,'recreate') as test_file:
        ## test_file.cd()
        treeSignal = ROOT.TTree('S','signal     tree')
        treeBkg    = ROOT.TTree('B','background tree')
        treeSignal.SetDirectory ( test_file ) 
        treeBkg   .SetDirectory ( test_file ) 
        
        from array import array 
        var1 = array ( 'd', [0])
        var2 = array ( 'd', [0])
        
        treeSignal.Branch ( 'var1' , var1 , 'var1/D' )
        treeSignal.Branch ( 'var2' , var2 , 'var2/D' )
        treeBkg   .Branch ( 'var1' , var1 , 'var1/D' )
        treeBkg   .Branch ( 'var2' , var2 , 'var2/D' )
        
        ## fill background tuple: 
        #for i in progress_bar ( xrange ( nB ) ) : 
        for i in xrange ( nB ) : 
            
            x = random.uniform ( -2 , 2 )
            y = random.uniform ( -2 , 2 )
            
            if x*x + y*y < 0.04 : continue
            
            var1[0] = x
            var2[0] = y
            treeBkg.Fill()
            
        ## fill signal tuple: 
        #for i in progress_bar ( xrange ( nS ) ) : 
        for i in xrange ( nS ) : 
            
            x = random.gauss ( 0 , 0.1 )
            y = random.gauss ( 0 , 0.1 )
            
            var1[0] = x
            var2[0] = y
            treeSignal.Fill()
            
        test_file.Write()
        test_file.ls()

if not os.path.exists ( tmva_file ) :
    
    logger.info('Create and train TMVA')
    with ROOT.TFile.Open( data_file ,'READ') as datafile : 
        datafile.ls()
        tSignal  = datafile['S']
        tBkg     = datafile['B']
        
        #
        ## book TMVA trainer
        #
        from Ostap.PyTMVA import Trainer 
        trainer = Trainer (
            methods = [
            # type                   name   configuration 
            ( ROOT.TMVA.Types.kMLP , "MLP", "H:!V:EstimatorType=CE:VarTransform=N:NCycles=600:HiddenLayers=N+7:TestRate=5:!UseRegulator" ) 
            ] ,
            verbose = False )
        
        xml_file = trainer.train (
            [ 'var1' , 'var2' ] , 
            signal         = tSignal ,
            background     = tBkg    )
        
        logger.info ( 'Weight file %s is created ' % xml_file  )
        

#
## create the datasets,
#  copy the input data into datasets and add TMVA information 
#
from Ostap.PyTMVA import Reader
reader = Reader( 'MyMLP' ,
                 variables    = [ ('var1' , lambda s : s.var1 )  ,
                                  ('var2' , lambda s : s.var2 ) ] ,
                 weights_file = tmva_file        ) 

variables = [
    ( 'var1' , 'variable#1', -10     , 10    , lambda s : s.var1 ) ,
    ( 'var2' , 'variable#2', -10     , 10    , lambda s : s.var2 ) ,
    ( 'tmva' , 'TMVA(MLP)' , -1.e+10 , 1e+10 , reader            ) ,
    ]    

from Ostap.Selectors import SelectorWithVars     
dsS = SelectorWithVars (
    variables = variables + [ ( 'signal' , 'signal' , -1 , 3 , lambda s : 1 ) ] ,
    selection = "var1 < 100" , 
    silence   = True          
    )

dsB = SelectorWithVars (
    variables = variables + [ ( 'signal' , 'signal' , -1 , 3 , lambda s : 0 ) ] ,
    selection = "var1 < 100" ,
    silence   = True           
    )

with ROOT.TFile.Open( data_file ,'READ') as datafile : 
    datafile.ls()
    tSignal  = datafile['S']
    tBkg     = datafile['B']

    tSignal.process ( dsS )
    tBkg   .process ( dsB )
    ds1 = dsS.data
    ds2 = dsB.data


    ds = ds1.reduce('var1>-100')
    ds.append(ds2)

    logger.info('<TMVA> for signal     %s' % ds1.statVar('tmva') )
    logger.info('<TMVA> for background %s' % ds2.statVar('tmva') )
    logger.info('<TMVA> for S+B sample %s' % ds .statVar('tmva') )




# =============================================================================
# The END
# =============================================================================    
