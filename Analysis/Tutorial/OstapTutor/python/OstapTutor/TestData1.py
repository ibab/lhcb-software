#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id:$
# ============================================================================
## @file TestData1.py
#  Helper dataset for Ostap tutorial
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
__all__     = (  'm_psi' , 'data' )
# ============================================================================
import ROOT, random
from   Ostap.PyRoUts    import *
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


# make simple test variable
m_psi  = ROOT.RooRealVar ( 'mass' , 'mass(mu+mu-)'  , 3.0  , 3.2 )

#
## create data set
# 
varset = ROOT.RooArgSet  ( m_psi ) 
data   = ROOT.RooDataSet ( dsID()  , 'Data set for Ostap tutorial' , varset  )

#
## fill it!
#

N_signal      = 10000 
N_background  =  1000  

random.seed(0)

s = VE(3.096, 0.013**2)

for i in range(0,N_signal ):
    m_psi.setVal  ( s.gauss() )
    data.add ( varset )

for i in range(0,N_background  ):
    m_psi.setVal  ( random.uniform ( *m_psi.minmax() ) )  
    data.add ( varset )


print __name__ 
print data  
                   
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

