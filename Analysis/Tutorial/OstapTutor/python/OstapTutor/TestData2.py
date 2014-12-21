#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id:$
# ============================================================================
## @file TestData2.py
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
  - J/psi-like signal
  - exponential background
  
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

from OstapTutor.TestVars1 import m_psi

#
## create data set
# 
varset = ROOT.RooArgSet  ( m_psi ) 


#
## create model
#
import  Ostap.FitModels as Models


signal = Models.Needham_pdf ( 'N0' , mass = m_psi )
bkg    = Models.Bkg_pdf     ( 'B0' , mass = m_psi )
bkg.tau.setVal( -10.0 )


model = Models.Fit1D( signal     = signal ,
                      background = bkg    ) 

model.s.setVal (  10000 )
model.B.setVal (   5000 )

data   = model.pdf.generate ( varset , 15000 )


# ============================================================================
if '__main__' == __name__ :

    import Ostap.Line
    logger.info ( __file__ + '\n' + Ostap.Line.line )
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*'   )
    logger.info ( ' Author  : %s' %         __author__    )
    logger.info ( ' Version : %s' %         __version__   )
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' )
    
# ============================================================================
# The END 
# ============================================================================

