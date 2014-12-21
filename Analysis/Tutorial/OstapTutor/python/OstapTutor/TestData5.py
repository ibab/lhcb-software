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
    logger = getLogger( 'OstapTutor.TestData5')
else : logger = getLogger( __name__ )
logger.info ('Create 1D dataset for Ostap Tutorial ')
# ============================================================================
RAD = ROOT.RooAbsData
if RAD.Tree != RAD.getDefaultStorageType() :
    logger.info ('DEFINE default storage type to be TTree! ')
    RAD.setDefaultStorageType ( RAD.Tree )
# ============================================================================

x  =  ROOT.RooRealVar( 'x' , 'x' , 0 , 10 )
varset = ROOT.RooArgSet ( x )


import Ostap.FitModels as Models

gamma = Models.GammaDist_pdf ( 'GPDF' , x )
gamma.k    .setVal ( 2 )
gamma.theta.setVal ( 1 )

data = gamma.pdf.generate ( varset , 100000  )

h1 = x.histo ( 100 )
h2 = x.histo ( 500 )
h3 = h1_axis ( [0, 1, 2, 3, 6 , 8 , 10 ] ) 

data.project ( h1 , 'x' )
data.project ( h2 , 'x' )
data.project ( h3 , 'x' )

h4 = h1_axis ( h2.equal_edges (  5 ) ) 
h5 = h1_axis ( h2.equal_edges ( 10 ) ) 
h7 = h1_axis ( h2.equal_edges ( 20 ) ) 
h6 = h1_axis ( h2.equal_edges ( 50 ) ) 

data.project ( h4 , 'x' )
data.project ( h5 , 'x' )
data.project ( h6 , 'x' )
data.project ( h7 , 'x' )



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
