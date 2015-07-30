#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Module with decoration of TFile objects for efficient use in python
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision: 185196 $
#  Last modification $Date: 2015-03-10 12:30:58 +0100 (Tue, 10 Mar 2015) $
#  by                $Author: ibelyaev $
# =============================================================================
"""Decoration of TFile objects for efficient use in python """
# =============================================================================
__version__ = "$Revision: 185196 $"
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
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.TFileDeco' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'Some useful decorations for TFile objects')
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
    
## write the object to ROOT-file 
def _rf_setitem_ ( rfile , name , tobj ) :
    """
    Write the object to ROOT-file

    >>> f['myhisto'] = h1
    
    """
    return rfile.WriteTObject( tobj , name , 'WriteDelete' )


## use ROOT-file with context-manager 
def _rf_enter_ ( self      ) :
    """
    Use ROOT-file with the context manager
    
    >>> with ROOT.TFile('ququ') as f : f.ls() 
    """ 
    return self

## use ROOT-file with context-manager 
def _rf_exit_  ( self , *_ ) :
    """
    Use ROOT-file with the context manager
    
    >>> with ROOT.TFile('ququ') as f : f.ls()
    """
    try :
        self.Close()
    except: pass
    
ROOT.TFile.__repr__    = _rf_print_
ROOT.TFile.  name      = ROOT.TFile.GetName
ROOT.TFile.__getitem__ = ROOT.TFile.Get 
ROOT.TFile.__getattr__ = ROOT.TFile.Get 
ROOT.TFile.__setitem__ = _rf_setitem_ 

if hasattr ( ROOT.TFile , '__enter__' ) and hasattr ( ROOT.TFile , '__exit__' ) : pass
else :
    ROOT.TFile.__enter__ = _rf_enter_
    ROOT.TFile.__exit__  = _rf_exit_
    

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
