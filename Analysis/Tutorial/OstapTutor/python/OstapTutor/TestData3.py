#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id:$
# ============================================================================
## @file TestData3.py
#  More realistic dataset for Ostap tutorial
#  @author Vanya BELYAEV Ivan..Belyaev@itep.ru
#  @date   2014-12-10
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# ============================================================================
"""
Helper dataset for Ostap tutorial:
  - Breit-Wigner
  - Flatte2 
  - phase-space background
  
"""
# ============================================================================
__version__ = "$Revision:$"
__author__  = "Vanya BELYAEV  Ivan.Belyaev@itep.ru"
__date__    = "2014-12-10"
__all__     = (  'data' , 'm_psi' )
# ============================================================================
import ROOT, random
from   Ostap.PyRoUts  import *
import Ostap.ZipShelve as     DBASE 
# ============================================================================
# logging
# ============================================================================
from AnalysisPython.Logger import getLogger
if __name__  in ( '__main__' , '__builtin__' ) :
    logger = getLogger( 'OstapTutor.TestData1')
else : logger = getLogger( __name__ )
logger.info ('Create 1D dataset for Ostap Tutorial ')
# ============================================================================

from OstapTutor.TestVars1 import m_phi

#
## create data set
# 
varset = ROOT.RooArgSet  ( m_phi ) 


#
## create model
#
import  Ostap.FitModels as Models


mK    = 0.49369 
mPion = 0.1396

bw = cpp.Gaudi.Math.Phi0 (  1.0195 , 0.0043 , mK  )

signal = Models.BreitWigner_pdf ( 'BW0' , bw , mass = m_phi ,
                                  gamma       = 0.0043 ,
                                  mean        = 1.0195 ,
                                  convolution = 0.0015 )

## Phase space as background:
ps  = cpp.Gaudi.Math.PhaseSpaceNL ( 2*mK  , 10.0 , 2 , 5 )
bkg = Models.PSPol_pdf            ( 'PS0' , mass = m_phi , phasespace = ps , power = 1 )


f2     = cpp.Gaudi.Math.Flatte2( 0.980 ,
                                 165   ,
                                 4.21  ,
                                 mPion ,
                                 mPion ,
                                 mK    ,
                                 mK    )

flatte = Models.Flatte2_pdf ( 'F20'           ,
                              f2              ,
                              mass   = m_phi  ,
                              m0_980 = 1.000  ,
                              m0g1   = 0.165  )

flatte.mean.fix(0.980)

                
model = Models.Fit1D( signal       =   signal   ,
                      othersignals = [ flatte ] , 
                      background   = bkg        ) 

model.S  .fix (  10000 )
model.B  .fix (   5000 )
model.S_1.fix (   5000 )

data   = model.pdf.generate ( varset , 20000 )

