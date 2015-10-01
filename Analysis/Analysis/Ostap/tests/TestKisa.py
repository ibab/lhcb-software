#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestKisa.py
#
#  Test for parallel data processing 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2015-05-17
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Test for parallel data processing 
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-08"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT,os 
import Ostap.PyRoUts 
from   Ostap.Data    import Data, DataAndLumi
from   Ostap.Utils   import timing 
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
if os.path.exists ( '/mnt/shared/VMDATA/LocalXML' ) :
    ganga = '/mnt/shared/VMDATA/LocalXML'
# =============================================================================
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
data8 = DataAndLumi ( 'Y/Y' , patterns[2:] )
dataY = DataAndLumi ( 'Y/Y' , patterns     )
logger.info ( 'DATA@  7TeV %s' % data7    )
logger.info ( 'DATA@  8TeV %s' % data8    )
logger.info ( 'DATA@7+8TeV %s' % dataY    )

logger.info ( 'TChain  %s' % data7.chain )

h1 = ROOT.TH1D( 'h1' , '' , 200 , 8.5 , 12.5 )
h2 = h1.clone()

chain = dataY.chain

with timing('SEQUENTIAL(1M):') :
    chain.project ( h1 , 'mass' ,'minann_mu>0.5 && 8.5<=mass && mass<12.5' ,'', 1000000 )

print h1.dump(40,20)

import Ostap.Kisa

with timing('PARALLEL(%dM):' % int(len(chain)/1.e+6) ) :
    chain._project ( h2 , 'mass' ,'minann_mu>0.5 && 8.5<=mass && mass<12.5' )


print h2.dump(40,20)



# =============================================================================
# The END 
# =============================================================================
