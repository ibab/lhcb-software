#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id:$
# ============================================================================
## @file fit1.py
#  Exmaple for simple fits 
#  @author Vanya BELYAEV Ivan..Belyaev@itep.ru
#  @date   2014-12-10
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# ============================================================================
"""
Example fo simmple fit
"""
# ============================================================================
__version__ = "$Revision:$"
__author__  = "Vanya BELYAEV  Ivan.Belyaev@itep.ru"
__date__    = "2014-12-10"
__all__     = (  'data' , 'm_psi' )
# ============================================================================
import ROOT, random
from   Ostap.PyRoUts  import *
# ============================================================================
# logging
# ============================================================================
from AnalysisPython.Logger import getLogger
if __name__  in ( '__main__' , '__builtin__' ) :
    logger = getLogger( 'fit1')
else : logger = getLogger( __name__ )
logger.info ('Simple fit')
# ============================================================================

## 1) import dataset and variable 
from OstapTutor.TestData4 import m_psi, m_D0, data

logger.info ( 'Data: %s' % data ) 


## 2) create the model: signnal + background

#
## create model
#
import  Ostap.FitModels as Models

jpsi  = Models.Needham_pdf ( 'N' , mass = m_psi , mean = 3.096  , sigma = 0.013 )
D0    = Models.Gauss_pdf   ( 'G' , mass = m_D0  , mean = 1.864  , sigma = 0.007 )

model = Models.Fit2D (
    signal_1 = jpsi ,
    signal_2 = D0   ,
    power1   = 2    ,
    power2   = 2    ,
    powerA   = 2    ,
    powerB   = 2    )



r,f = model.fitTo  ( data , silence = True , ncpu = 8 ) 
r,f = model.fitTo  ( data , ncpu = 8 , silence = True ) 

model.draw1 ( data  , nbins = 50 )




# ============================================================================
# The END 
# ============================================================================
