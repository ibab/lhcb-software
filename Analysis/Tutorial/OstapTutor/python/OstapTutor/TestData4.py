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
  - J/psi-like along  the first axis,
  - D0-like gaussian along the second axis 
  - double exponential background  
"""
# ============================================================================
__version__ = "$Revision:$"
__author__  = "Vanya BELYAEV  Ivan.Belyaev@itep.ru"
__date__    = "2014-12-10"
__all__     = (  'data' , 'm_psi' , 'm_phi')
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

from OstapTutor.TestVars1 import m_psi, m_D0

#
## create data set
# 
varset = ROOT.RooArgSet  ( m_psi , m_D0  ) 

#
## create model
#
import  Ostap.FitModels as Models

jpsi  = Models.Needham_pdf ( 'N0' , mass = m_psi , mean = 3.096  , sigma = 0.013 )
D0    = Models.Gauss_pdf   ( 'G0' , mass = m_D0  , mean = 1.864  , sigma = 0.007 )


bkg_jpsi = Models.Bkg_pdf  ( 'B01' , mass = m_psi )
bkg_D0   = Models.Bkg_pdf  ( 'B02' , mass = m_D0  )
bkg_jpsi.tau.fix ( -10 )
bkg_D0  .tau.fix (  +5 )

model = Models.Fit2D (
    signal_1 = jpsi ,
    signal_2 = D0   ,
    bkg1     = bkg_jpsi ,
    bkgA     = bkg_jpsi ,
    bkg2     = bkg_D0   ,
    bkgB     = bkg_D0   )


model.ss.fix (  5000  )
model.sb.fix (  2500  )
model.bs.fix (  2500  )
model.bb.fix ( 10000  )

data   = model.pdf.generate ( varset , 20000 )

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
