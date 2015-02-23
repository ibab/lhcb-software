#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestData.py
#
#  tests for simple data-access 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2014-06-06
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
tests for simple data-access 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-08"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT
import Ostap.PyRoUts 
from   Ostap.Data    import Data, DataAndLumi  
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__ or '__builtin__' == __name__ : 
    logger = getLogger( 'Ostap.TestData' )
else : 
    logger = getLogger( __name__ )
# =============================================================================
logger.info('Test  Data')
# =============================================================================
# data patterns: 
ganga    = '/afs/cern.ch/work/i/ibelyaev/public/GANGA/workspace/ibelyaev/LocalXML'
patterns = [
    ganga +  '/90/*/output/Bc.root' ,  ## 2011-Mag-Down
    ganga +  '/91/*/output/Bc.root' ,  ## 2011-Mag-Up 
    ]

data1 = Data ( 'Bc/Bc' , patterns )
logger.info  ( 'DATA       %s' % data1       )

# =============================================================================
logger.info('Test  Data&Lumi')
# =============================================================================

data2 = DataAndLumi ( 'Bc/Bc' , patterns    )
logger.info ( 'DATA       %s' % data2       )

# =============================================================================
# The END 
# =============================================================================
