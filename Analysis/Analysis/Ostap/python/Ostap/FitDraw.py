#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
## @file FitBasic.py
#
#  Set of useful basic utilities to build various fit models 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision: 192547 $
#  Last modification $Date: 2015-07-25 18:04:58 +0200 (Sat, 25 Jul 2015) $
#                 by $Author: ibelyaev $
# =============================================================================
"""Set of useful basic utilities to build various fit models"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    ##
    'lineWidth'             , ## line width
    'lineColor'             , ## line color 
    'lineStyle'             , ## line style 
    ##
    'data_options'          , ## option for data
    'data_options_nobars'   , ## option for data without bars 
    'signal_options'        , ## signal options    
    'background_options'    , ## background options 
    'component_options'     , ## other components
    'fit_options'           , ## the fit curve
    ##
    'base_signal_color'     , ## base signal color 
    'base_background_color' , ## base signal color 
    'base_component_color'  , ## base signal color 
    )
# =============================================================================
import ROOT
from   AnalysisPython.Logger     import getLogger
# =============================================================================
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.FitDraw' )
else                       : logger = getLogger ( __name__         )
# =============================================================================
def lineWidth ( w ) : return ROOT.RooFit.LineWidth ( w )
def lineStyle ( s ) : return ROOT.RooFit.LineStyle ( s )
def lineColor ( c ) : return ROOT.RooFit.LineColor ( c )
#

## plain, default
data_options          = ()

## suppress small bars at the end of error bars 
data_options_nobars   = ( ROOT.RooFit.MarkerStyle ( 20   ) ,
                          ROOT.RooFit.DrawOption  ( "zp" ) )

## signal:      thin dotted line
signal_options        = lineWidth ( 1 ) , lineStyle ( ROOT.kDotted     )

## background:  thin long-dashed line
background_options    = lineWidth ( 1 ) , lineStyle ( 7 ) 

## "component": thin dash-dotted line
component_options     = lineWidth ( 1 ) , lineStyle ( ROOT.kDashDotted )

## fir curve:   thick red solid line 
fit_options           = lineWidth ( 2 ) , lineColor ( ROOT.kRed ) , lineStyle ( 1 ) 


base_signal_color     = ROOT.kMagenta
base_background_color = ROOT.kBlue 
base_component_color  = ROOT.kCyan 

# =============================================================================
if '__main__' == __name__ :
    
    from Ostap.Line import line 
    logger.info ( __file__ + '\n' + line  )
    logger.info ( 80*'*' )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# =============================================================================
# The END 
# =============================================================================
