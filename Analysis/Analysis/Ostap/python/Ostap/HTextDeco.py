#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of histo&text objects for efficient use in python,
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
""" Module with decoration of histo&text objects for efficient use in python
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT, cppyy              ## attention here!!
cpp = cppyy.makeNamespace('')
VE  = cpp.Gaudi.Math.ValueWithError 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.HTextDeco' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'Some useful decorations for Histo&Text objects')
# =============================================================================
## further decoration
try : 
    import GaudiPython.HistoUtils
    logger.debug   ( 'Histogram utilities from GaudiPython.HistoUtils' )
except:
    logger.warning ( 'Histogram utilities from GaudiPython.HistoUtils are not loaded' )
    
# =============================================================================
_HS = cpp.Gaudi.Utils.Histos.HistoStrings
# =============================================================================
## convert histogram to string
def _h_toString_ ( h , asXml = False  ) :
    """
    Convert histogram to string (or XML)

    >>> h = ... # the histo
    >>> s = h.toString()
    >>> print s 
    
    """
    return _HS.toString( h , asXml )

_h_toString_ . __doc__  += '\n' + _HS.toString. __doc__

# =============================================================================
## convert histogram to XML
def _h_toXml_ ( h ) :
    """
    Convert histogram to XML

    >>> h = ... # the histo
    >>> s = h.toXml()
    >>> print s 
    
    """
    return _HS.toXml( h )

_h_toXml_ . __doc__  += '\n' + _HS.toXml. __doc__

# =============================================================================
## convert XML to histogram 
def _h_fromString_ ( h , input  ) :
    """
    Convert string (or XLM) to histogram 

    >>> input  = ... # the string(or XML)
    >>> histo  = ... # book some the histo
    >>> status = histo.fromString ( input ) 

    """
    return _HS.fromString ( h , input )

_h_fromString_ . __doc__  += '\n' + _HS.fromString. __doc__

# =============================================================================
## convert XML to histogram 
def _h_fromXml_ ( h , xml ) :
    """
    Convert XML to histogram 

    >>> xml   = ... # the XML-string
    >>> histo = ... # book some the histo
    >>> status = histo.fromXml ( xml ) 

    """
    return _HS.fromXml ( h , xml )

_h_fromXml_ . __doc__  += '\n' + _HS.fromXml. __doc__


for t in ( ROOT.TH1D       ,
           ROOT.TH1F       ,
           ROOT.TH2D       ,
           ROOT.TH2F       ,     
           ROOT.TH3D       ,     
           ROOT.TProfile   ,
           ROOT.TProfile2D ) : 
    
    t.toString   = _h_toString_
    t.fromString = _h_fromString_
    t.toXml      = _h_toXml_
    t.fromXml    = _h_fromXml_


# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
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
