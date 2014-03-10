#!/usr/bin/env python
# =============================================================================
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of some ROOT objects for efficient use in PyROOT
#
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""

Module with decoration of some ROOT objects for efficient use in PyROOT

Many native  root classes are equipped with new useful methods and operators,
in particular TH1(x) , TH2(x) , TAxis, TGraph(Errors), etc...

see also GaudiPython.HistoUtils 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__     = (
    #
    'cpp'             , ## global C++ namespace 
    'rootID'          , ## construct the (global) unique ROOT identifier
    'funcID'          , ## construct the (global) unique ROOT identifier
    'funID'           , ## construct the (global) unique ROOT identifier
    'hID'             , ## construct the (global) unique ROOT identifier
    'histoID'         , ## construct the (global) unique ROOT identifier
    'dsID'            , ## construct the (global) unique ROOT identifier
    #
    'VE'              , ## Gaudi::Math::ValueWithError
    'ValueWithError'  , ## Gaudi::Math::ValueWithError
    'histoGuess'      , ## guess the simple histo parameters
    'useLL'           , ## use LL for histogram fit?
    'allInts'         , ## natural histogram with natural entries?
    #
    'binomEff'        , ## calculate binomial efficiency
    'binomEff2'       , ## calculate binomial efficiency
    'zechEff'         , ## calcualte binomial efficiency using Zech's          prescription
    'wilsonEff'       , ## calcualte binomial efficiency using Wilson's        prescription
    'agrestiCoullEff' , ## calcualte binomial efficiency using Agresti-Coull's prescription
    #
    'binomEff_h1'     , ## calculate binomial efficiency for 1D-histos
    'binomEff_h2'     , ## calculate binomial efficiency for 2D-ihstos
    'binomEff_h3'     , ## calculate binomial efficiency for 3D-ihstos
    #
    'makeGraph'       , ## make ROOT Graph from input data
    'h2_axes'         , ## book 2D-histogram from axes
    'h1_axis'         , ## book 1D-histogram from axis 
    'axis_bins'       , ## convert list of bin edges to axis
    've_adjust'       , ## adjust the efficiency to be in physical range
    #
    )
# =============================================================================
import ROOT
# =============================================================================
import warnings
warnings.warn (
    """AnalysisPython:
    Use 'PyPAW.PyRoUts' module instead of 'AnalysisPython.PyRoUts'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from PyPAW.PyRoUts import *

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
