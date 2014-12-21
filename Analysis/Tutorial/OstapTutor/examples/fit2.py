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
from OstapTutor.TestData2 import m_psi, data

logger.info ( 'Data: %s' % data ) 


## 2) create the model: signnal + background

import Ostap.FitModels as Models

## 2a) create the  signal : 


signal = Models.CrystalBall_pdf ( 'CB'          ,
                                  mass  = m_psi , 
                                  sigma = 0.013 ,
                                  alpha = 1.5   ,
                                  n     = 1     , 
                                  mean  = 3.096 )   

# signal = Models.CB2_pdf ( 'CB'          ,
#                          mass   = m_psi , 
#                          sigma  = 0.013 ,
#                          alphaL = 1.5   ,
#                          alphaR = 1.5   ,
#                          nL     = 1     , 
#                          nR     = 1     , 
#                          mean   = 3.096 )   

# signal = Models.Bukin_pdf ( 'Bk'          ,
#                            mass   = m_psi , 
#                            sigma  = 0.013 ,
#                            xi     = 0.000 ,
#                            rhol   = 0     ,
#                            rhor   = 0     , 
#                            mean   = 3.096 )   

# signal = Models.BifurcatedGauss_pdf ( 'Bg'          ,
#                                      mass   = m_psi , 
#                                      sigma  = 0.013 ,
#                                      mean   = 3.096 )   

# signal = Models.Apolonios_pdf ( 'Ap'           ,
#                                mass   = m_psi , 
#                                sigma  = 0.013 ,
#                                alpha  = 2.0   ,
#                                n      = 2.0   , 
#                                mean   = 3.096 )   

# signal = Models.Apolonios2_pdf ( 'A2'           ,
#                                 mass   = m_psi , 
#                                 sigma  = 0.013 ,
#                                 mean   = 3.096 )   

# signal = Models.SkewGauss_pdf ( 'Sk'           ,
#                                mass   = m_psi ,
#                                omega  = 0.013 , 
#                                mean   = 3.096 )   

# signal = Models.StudentT_pdf ( 'St'           ,
#                               mass   = m_psi ,
#                               sigma  = 0.013 , 
#                               mean   = 3.096 )   

# signal = Models.BifurcatedStudentT_pdf ( 'Sb'           ,
#                                         mass   = m_psi ,
#                                         sigma  = 0.013 , 
#                                         mean   = 3.096 )   

# signal = Models.Needham_pdf ( 'Nh'           ,
#                              mass   = m_psi ,
#                              sigma  = 0.013 , 
#                              mean   = 3.096 )   


## 2b) create the  background : exponential times 1st order polymonial  

bkg   = Models.Bkg_pdf  ( 'B'  , mass = m_psi , power = 1 )


##  2c) create the model

model = Models.Fit1D ( signal     = signal ,
                       background = bkg    )  


## 3) try to fit:

r,f = model.fitTo  ( data , silence = True , ncpu = 8 ) 

signal .mean .release()
signal .sigma.release()

r,f = model.fitTo  ( data , ncpu = 8 , draw = True , silence = True ) 



## 
## signal0 = Models.Gauss_pdf ( 'G'           ,
##                            mass   = m_psi ,
##                            sigma  = 0.013 , 
##                            mean   = 3.096 )   
## 
## model2 = Models.Fit1D ( signal       =   signal   ,
##                         othersignals = [ signal0] , 
##                         background   = bkg        )  
## 

# ============================================================================
# The END 
# ============================================================================
