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
__all__     = ( 'm_psi'  ,
                'm_psi1' ,
                'm_phi'  ,
                'm_phi1' ,
                'm_D0'   ,
                'm_ups'  )
# ============================================================================
import ROOT, random
from   Ostap.PyRoUts  import *
import Ostap.ZipShelve as     DBASE 
# ============================================================================
# logging
# ============================================================================
from AnalysisPython.Logger import getLogger
if __name__  in ( '__main__' , '__builtin__' ) :
    logger = getLogger( 'OstapTutor.TestVars1')
else : logger = getLogger( __name__ )
logger.info ('Create some varibales for Ostyap tutorial')
# ============================================================================

# make simple test variable
m_psi   = ROOT.RooRealVar ( 'mPsi'  , 'mass(mu+mu-)'  , 3.000  , 3.200 )
m_psi1  = ROOT.RooRealVar ( 'mPsi1' , 'mass(mu+mu-)'  , 3.000  , 3.200 )

# make simple test variable
m_phi  = ROOT.RooRealVar  ( 'mPhi'   , 'mass(K+K-)'    , 0.990  , 1.050 )
m_phi1 = ROOT.RooRealVar  ( 'mPhi1'  , 'mass(K+K-)'    , 1.000  , 1.050 )

# make simple test variable
m_D0   = ROOT.RooRealVar  ( 'm_D0' , 'mass(K^{-}#pi^{+})'        , 1.82 , 1.92 )
m_Dp   = ROOT.RooRealVar  ( 'm_Dp' , 'mass(K^{-}#pi^{+}#pi^{+})' , 1.82 , 1.91 )
m_Ds   = ROOT.RooRealVar  ( 'm_Ds' , 'mass(K^{-}K^{+}#pi^{+})'   , 1.90 , 2.04 )
m_Lc   = ROOT.RooRealVar  ( 'm_Lc' , 'mass(pK^{-}#pi^{+})'       , 2.24 , 2.33 )

# make test variable
m_ups  = ROOT.RooRealVar ( 'mY'     , 'mass(mu+mu-)'   , 9.0 , 11.0 ) 
m_Y    = m_ups  ## ditto 

# ============================================================================
# The END 
# ============================================================================

