#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of File/Canvas/.. objects for efficient use in python
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""

Module with decoration of File/Canvas/.. objects for efficient use in python

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
logger.info ( 'Some useful decorations for File/Canvas objects')
# =============================================================================

# ==============================================================================
## print ROOT file (actually a combination of ls&Print)
#  @code
#
#  >>> f = ROOT.TFile(... )
#  >>> print f
#
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-07-06
def _rf_print_ ( rfile , opts = '') :
    """
    Print ROOT file (actually a combination of ls&Print)
    
    >>> f = ROOT.TFile(... )
    >>> print f
    """
    #
    rfile.ls    ( opts  )
    #
    rfile.Print ( 'v'   )
    #
    return rfile.GetName()
    
ROOT.TFile.__repr__    = _rf_print_

ROOT.TFile.name        = ROOT.TFile.GetName
ROOT.TFile.__getitem__ = ROOT.TFile.Get 
ROOT.TFile.__getattr__ = ROOT.TFile.Get 


# =============================================================================
## define simplified print for TCanvas 
def _cnv_print_ ( cnv , fname , exts = [ 'pdf' , 'png' , 'eps', 'C' ] ) :
    """
    A bit simplified version for TCanvas print

    >>> canvas.print ( 'fig' )    
    """
    #
    p = fname.rfind ('.')
    #
    if 0 < p :
        
        if p+4 == len ( fname ) or fname[p:] in ( '.C', '.ps', '.jpeg', '.JPEG') :
            cnv.Print( fname )
            return cnv 

    for e in exts :
        cnv.Print ( fname + '.' + e )
        
    return cnv 

# =============================================================================
## define streamer for canvas 
def _cnv_rshift_ ( cnv , fname ) :
    """
    very simple print for canvas:
    
    >>> canvas >> 'a'
    
    """
    return _cnv_print_ ( cnv , fname )

ROOT.TCanvas.print_     = _cnv_print_
ROOT.TCanvas.__rshift__ = _cnv_rshift_



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
