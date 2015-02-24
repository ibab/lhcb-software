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
ganga = '/afs/cern.ch/work/i/ibelyaev/public/GANGA/workspace/ibelyaev/LocalXML'
patterns = [
    ganga + '/358/*/output/CY.root' , ## 2k+11,down
    ganga + '/359/*/output/CY.root' , ## 2k+11,up
    ganga + '/356/*/output/CY.root' , ## 2k+12,down
    ganga + '/357/*/output/CY.root' , ## 2k+12,up
    ]

data1 = Data ( 'YD0/CY'   , patterns )
logger.info  ( 'DATA %s' % data1       )

# =============================================================================
logger.info('Test  Data&Lumi')
# =============================================================================

data7 = DataAndLumi ( 'Y/Y' , patterns[ :2])
data8 = DataAndLumi ( 'Y/Y' , patterns[2:])
logger.info ( 'DATA@7TeV %s' % data7    )
logger.info ( 'DATA@8TeV %s' % data8    )

logger.info ( 'TChain  %s' % data7.chain )

# =============================================================================
# The END 
# =============================================================================
