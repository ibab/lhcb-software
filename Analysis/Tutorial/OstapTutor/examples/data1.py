#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
# $Id:$ 
# =========================================================================================
## @file
#  simple examples how to  get data
#    - collect files
#    - build the chain
#    - get integrated lumi
#
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @thanks Alexander BARANOV
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
from   Ostap.Data            import DataAndLumi 
from   AnalysisPython.Logger import getLogger
# ==========================================================================================
if '__main__' == __name__ : logger = getLogger('OstapTutor/data1')
else                      : logger = getLogger( __name__         )
# ==========================================================================================
## define the default storage for data set 
RAD = ROOT.RooAbsData
if RAD.Tree != RAD.getDefaultStorageType() :
    logger.info ( 'DEFINE default storage type to be TTree! ') 
    RAD.setDefaultStorageType ( RAD.Tree )

ganga = '/afs/cern.ch/work/i/ibelyaev/public/GANGA/workspace/ibelyaev/LocalXML'
## ganga = '/mnt/shared/VMDATA/LocalXML'

patterns = [ ganga + '/%d/*/output/YY.root' % i for i in range( 416 , 420 ) ]

# =================================================================================
## The most common case:  Get chain with data and luminosity
#  there are other cases:
#  - collect only the list of files          ("Files")
#  - collect files and build only one chain  ("Data")    
#  - collect files and build two  chain      ("Data2")       (useful for MC)
#  - collect fiels and count luminosity      ("DataAndLumi") (useful fro real data)
# =================================================================================
data_YY = DataAndLumi ( 'YY/Y' , patterns )


print data_YY 
print data_YY.chain 



# =================================================================================
# The END 
# =================================================================================
