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
import ROOT, os , cppyy              ## attention here!!
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
## @class ROOTCURRDIR
#  context manager to preserve current directory (rather confusing stuff in ROOT) 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
class ROOTCURRDIR(object) :
    """
    Context manager to preserve current directory (rather confusing stuff in ROOT) 
    """
    def __enter__ ( self ) :
        ## save global ROOT directory 
        self._dir = ROOT.gDirectory
        
    def __exit__  ( self , *_ ) :
        ## make global directory current again 
        if self._dir : self._dir.cd()

# =======================================================================================
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
    with ROOTCURRDIR() :
        #
        rfile.cd ()
        #
        rfile.ls    ( opts  )
        #
        rfile.Print ( 'v'   )
        #
        return rfile.GetName()
    
## write the object to ROOT-file 
def _rd_setitem_ ( rfile , name , tobj ) :
    """
    Write the object to ROOT-file
    >>> f['myhisto'] = h1
    """
    with ROOTCURRDIR() :
        #
        rfile.cd()
        return rfile.WriteTObject( tobj , name , 'WriteDelete' )

## get the object from ROOT-file 
def _rd_getitem_ ( rfile , name ) :
    """
    Get the object form ROOT-file
    >>> obj = f['A/tup']
    """
    if not rfile : return None
    with ROOTCURRDIR() :
        ##
        rfile.cd()
        ## 
        dn , s , ff = name.partition('/')
        ## 
        if not s or not f : return rfile.Get( name )
        #
        ## try to get directory:
        ldir = rdir.GetDirectory ( dn )
        if not ldir :
            print 'MISSING DIRECTORY %s ' % dn 
            return None
        # 
        return _rd_getitem_ ( ldir , fn )

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

# =============================================================================a
## get all keys from the directory 
def _rd_keys_ ( rdir , recursive = True ) :
    """
    Get all keys from the directory
    >>> keys = rfile.keys() 
    """
    if not rdir : return []
    ## get the actual list of keys 
    _lst = rdir.GetListOfKeys()
    _res = []
    for i in _lst :
        inam = i.GetName() 
        _res.append ( inam )    
        if i.IsFolder() :
            idir  = i.ReadObj ()
            ikeys = _rd_keys_ ( idir , recursive )
            for k in ikeys : _res.append ( inam + '/' + k )

    return _res  


ROOT.TDirectory.keys        = _rd_keys_
ROOT.TDirectory.__getitem__ = _rd_getitem_ 
ROOT.TDirectory.__setitem__ = _rd_setitem_

ROOT.TFile.__repr__    = _rf_print_
ROOT.TFile.  name      = ROOT.TFile.GetName

ROOT.TFile.close       = ROOT.TFile.Close 

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
