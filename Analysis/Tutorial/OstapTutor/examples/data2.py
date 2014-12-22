#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
# $Id:$ 
# =========================================================================================
## @file
#  simple examples how to build dataset from TChain/Tree
#  - book and define variables/columns
#  - define filter criteria
#  This is not the only  way to build dataset (n particular it is rather slow),
#  but it has certain advantages  
#  @see RooDataSet
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# =========================================================================================
""" Simple example  to fill dataset for TChain/TTree
- book and define variables/columns
- define filter criteria  
This is not the only  way to build dataset  (in particular it is rather slow),
but it has certain advantages  
"""
# =========================================================================================
import ROOT
from   Ostap.PyRoUts         import * 
from   AnalysisPython.Logger import getLogger
from   Ostap.Selectors       import SelectorWithVars 
# ==========================================================================================
if '__main__' == __name__ : logger = getLogger('OstapTutor/data2')
else                      : logger = getLogger( __name__         )
# ==========================================================================================

from OstapTutor.TestVars1  import m_psi,m_ups 

## define variables to be used in data set 
variables = [
    ( m_psi     , lambda s : min(s.mass1,s.mass2) ) ,
    ( m_ups     , lambda s : max(s.mass1,s.mass2) ) ,
    ( "mass"    , "mass(4mu)" , 0 , 1e+6  , lambda s : s.mass ) ,
    ]


## get TChain
import data1 as     data 
chain = data.data_YY.chain

## get basic cuts:
from ycuts   import cuts

## make selector 
sel_Ypsi = SelectorWithVars (
    variables ,
    selection = cuts 
    )

## fill selector 
chain.process ( sel_Ypsi )

## get dataset
ds_Ypsi = sel_Ypsi.data

print ds_Ypsi  

# ==========================================================================================
# The END 
# ==========================================================================================
