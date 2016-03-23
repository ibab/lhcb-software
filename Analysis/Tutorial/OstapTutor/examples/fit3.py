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
from OstapTutor.TestData3 import m_phi, data

logger.info ( 'Data: %s' % data ) 


## 2) create the model: signnal + background

import Ostap.FitModels as Models

#
## create model
#
import  Ostap.FitModels as Models


mK    = 0.49369 
mPion = 0.1396
bw    = cpp.Gaudi.Math.Phi0 (  1.0195 , 0.0043 , mK  )

signal = Models.BreitWigner_pdf ( 'BW' , bw , mass = m_phi ,
                                  gamma       = 0.0043 ,
                                  mean        = 1.0195 ,
                                  convolution = 0.0015 )

## Phase space as background:
ps  = cpp.Gaudi.Math.PhaseSpaceNL ( 2*mK , 10.0 , 2 , 5 )
bkg = Models.PSPol_pdf            ( 'PS'  , mass = m_phi , phasespace = ps , power = 1 )

f2     = cpp.Gaudi.Math.Flatte2( 0.980 ,
                                 165   ,
                                 4.21  ,
                                 mPion ,
                                 mPion ,
                                 mK    ,
                                 mK    )

flatte = Models.Flatte2_pdf ( 'F2'            ,
                              f2              ,
                              mass   = m_phi  ,
                              m0_980 = 1.000  ,
                              m0g1   = 0.165  ) 

flatte.mean.fix(0.980)

model = Models.Fit1D( signal       =   signal   ,
                      othersignals = [ flatte ] , 
                      background   = bkg        ) 


r,f = model.fitTo  ( data , silence = True , ncpu = 8 ) 

## signal .mean .release()
## signal .sigma.release()

r,f = model.fitTo  ( data , ncpu = 8 , draw = True , silence = True ) 



# ============================================================================
# The END 
# ============================================================================
