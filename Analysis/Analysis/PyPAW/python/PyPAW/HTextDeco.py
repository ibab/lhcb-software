#!/usr/bin/env python
# =============================================================================
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of histo&text objects for efficient use in python,
#  including HEPDATA format 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""

Module with decoration of histo&text objects for efficient use in python
including HEPDATA format 

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
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Some useful decorations for Histo&Text objects')
# =============================================================================
## further decoration
try : 
    import GaudiPython.HistoUtils
    logger.info    ( 'Histogram utilities from GaudiPython.HistoUtils' )
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
## HEPDATA format
# 
# @code 
# The format we accept data in is very wide and generally we require only
# a flat file containing the numerical values.
# Postscript and pdf figures are not suitable.
# 
# Ideally the format should be:
#
# xlow xhigh y +stat -stat +sys1 -sys1 +sys2 -sys2 ......
# where:
#  xlow and xhigh are the xbin edges
#  y is the measured quantity
# +stat and -stat are the positive and negative statistical errors (could also be +-stat)
# +sysn and -sysn are any number of positive and negative systematic errors (again could be +-sysn)
# @endcode 
# 
# =============================================================================
def _h1_hepdata_ ( h1 , fmt = " %13.6g %-13.6g   %13.6g +-%-12.5g \n" ) :
    '''
    Dump 1D-histogram in HepData -compatible format
    
    >>> h = ... # the histogram
    >>> data = h.toHepDATA ()
    
    """
    The format we accept data in is very wide and generally we require only
    a flat file containing the numerical values.
    Postscript and pdf figures are not suitable.
    
    Ideally the format should be:
    
    xlow xhigh y +stat -stat +sys1 -sys1 +sys2 -sys2 ......
    
    where:
    xlow and xhigh are the xbin edges
    y is the measured quantity
    +stat and -stat are the positive and negative statistical errors (could also be +-stat)
    +sysn and -sysn are any number of positive and negative systematic errors (again could be +-sysn)
    """
    '''    
    data = ''
    for  ibin in h1.iteritems () :
        
        x = ibin[1]
        y = ibin[2]
        
        x_low  = x.value() - x.error()
        x_high = x.value() + x.error()
        
        data += fmt  % ( x_low      ,
                         x_high     ,
                         y.value () ,
                         y.error () )
        
    return data

# ================================================================================================
def _h2_hepdata_ ( h2 ,
                   fmt = " %13.6g %-13.6g  %13.6g %-13.6g  %13.6g +-%-12.5g \n" )  : 
    """
    Dump 2D-histogram in HepData -compatible format
    
    >>> h = ... # the histogram
    >>> data = h.toHepDATA ()
    
    """
    data = ''
    for  ibin in h2.iteritems () :

        x = ibin[2]
        y = ibin[3]
        z = ibin[4]
        
        x_low  = x.value () - x.error ()
        x_high = x.value () + x.error ()
        y_low  = y.value () - y.error ()
        y_high = y.value () + y.error ()
        
        data += fmt  % ( x_low      ,
                         x_high     ,
                         y_low      ,
                         y_high     ,
                         z.value () ,
                         z.error () )
        
        
    return data
    
    
for t in ( ROOT.TH1D ,
           ROOT.TH1F ) :
    
    t . toHepDATA = _h1_hepdata_
    t . toHepData = _h1_hepdata_
    
for t in ( ROOT.TH2D ,
           ROOT.TH2F ) :
    
    t . toHepDATA = _h2_hepdata_
    t . toHepData = _h2_hepdata_
    
# =============================================================================


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
