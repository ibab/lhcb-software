#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
# $Id:$ 
# =========================================================================================
## @file
#  Another examples how to  get data
#    - collect files
#    - build the chain
#    - get integrated lumi
#
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# =========================================================================================
""" Simple examples how to  get data
- collect files
- build the chain
- get integrated lumi
"""
# =========================================================================================
import ROOT
from   Ostap.PyRoUts         import * 
from   Ostap.Data            import DataAndLumi as Data 
from   AnalysisPython.Logger import getLogger
# ==========================================================================================
if '__main__' == __name__ : logger = getLogger('OstapTutor/data3')
else                      : logger = getLogger( __name__     )
# ==========================================================================================
## define the default storage for data set 
RAD = ROOT.RooAbsData
if RAD.Tree != RAD.getDefaultStorageType() :
    logger.info ( 'DEFINE default storage type to be TTree! ') 
    RAD.setDefaultStorageType ( RAD.Tree )

    
ganga = '/afs/cern.ch/work/i/ibelyaev/public/GANGA/workspace/ibelyaev/LocalXML'
## ganga = '/mnt/shared/VMDATA/LocalXML'


patterns = [
    ganga + '/356/*/output/CY.root' , ## 2k+12,down
    ganga + '/357/*/output/CY.root' , ## 2k+12,up
    ganga + '/358/*/output/CY.root' , ## 2k+11,down
    ganga + '/359/*/output/CY.root' , ## 2k+11,up
    ]

data_D0Y   = Data ( 'YD0/CY' , patterns )
data_DpY   = Data ( 'YD+/CY' , patterns )
data_DsY   = Data ( 'YDs/CY' , patterns )
data_LcY   = Data ( 'YLc/CY' , patterns )


logger.info ( 'DATA(Y&D0) %s' % data_D0Y  ) 
logger.info ( 'DATA(Y&D+) %s' % data_DpY  ) 
logger.info ( 'DATA(Y&Ds) %s' % data_DsY  ) 
logger.info ( 'DATA(Y&Lc) %s' % data_LcY  ) 


## prompt Y-signals  
data_Y  = Data ( 'Y/Y'    , patterns )
logger.info ( 'DATA(Y  all) %s' % data_Y  ) 


## get the chains from 
tD0Y       = data_D0Y.chain
tDpY       = data_DpY.chain
tDsY       = data_DsY.chain
tLcY       = data_LcY.chain



# ==========================================================================================
# The END 
# ==========================================================================================


