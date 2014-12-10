#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id:$
# ============================================================================
## @file TestFit1.py
#  Fitting example for 1D  "J/psi-like" distribution 
#  @author Vanya BELYAEV Ivan..Belyaev@itep.ru
#  @date   2014-12-10
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# ============================================================================
"""
Helper dataset for Ostap tutorial
"""
# ============================================================================
__version__ = "$Revision:$"
__author__  = "Vanya BELYAEV  Ivan.Belyaev@itep.ru"
__date__    = "2014-12-10"
__all__     = (  'model'  , )
# ============================================================================
import ROOT
import Ostap.PyRoUts
import Ostap.FitModels as Models  
# ============================================================================
# logging
# ============================================================================
from AnalysisPython.Logger import getLogger
if __name__  in ( '__main__' , '__builtin__' ) :
    logger = getLogger( 'OstapTutor.TestFit1')
else : logger = getLogger( __name__ )
logger.info ('Fit 1D dataset for Ostap Tutorial with different models')
# ============================================================================

# get the variable
from  OstapTutor.TestData1 import  m_psi, data

## simplest signal model: Gauss 
signal = Models.Gauss_pdf ( 'G'              ,
                            mass = m_psi     ,
                            mean = (3.080 , 3.110 ) ,
                            sigma = 0.013    ) 

model = Models.Fit1D (
    signal      = signal ,
    background  = Models.Bkg_pdf('B'  , mass = m_psi , power = 1  )
    )



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

