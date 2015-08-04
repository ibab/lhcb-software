#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of TFile objects for efficient use in python
#  It provides TFile (well, actually any TDirectory) with python-like protocol
#  @code
#  rfile = ...
#
#  obj   = rfile['A/B/C/myobj']     ## READ  object form the file/directory
#  rfile['A/B/C/myobj2'] = object2  ## WRITE object to the file/directory 
#
#  obj  = rfile.A.B.C.myobj        ## another way to access to the object in file
#
#  obj  = rfile.get ( 'A/B/C/q' , None )   ## one more way to get object 
#
#  for obj in rfile : print obj     ## loop over all objects in file
#  for key,obj in rfile.iteritems() : print key, obj             ## another loop
#  for key,obj in rfile.iteritems( ROOT.TH1 ) : print key, obj   ## advanced loop
#  for key in rfile.keys()     : print k   ## get all keys and loop over them 
#  for key in rfile.iterkeys() : print k   ## loop over all keys in the file 
#
#  del  rfile['A/B']                       ## delete the object from the file
#
#  if 'A/MyHisto' in rfile          : print 'OK!' ## check presence of the key
#  if rfile.has_key ( 'A/MyHisto' ) : print 'OK!' ## check presence of the key
#
#  with ROOT.TFile('aa.root') as rfile : rfile.ls() ## context manager protocol
#  @endcode 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Decoration of TFile objects for efficient use in python

It provides TFile (well, actually any TDirectory) with python-like protocol

>>> rfile = ...

>>> obj   = rfile['A/B/C/myobj']     ## READ  object form the file/directory
>>> rfile['A/B/C/myobj2'] = object2  ## WRITE object to the file/directory 

>>> obj  = rfile.A.B.C.myobj              ## another way to access to the object
>>> obj  = rfile.get ( 'A/B/C/q' , None ) ## one more way to get object 

>>> for obj in rfile : print obj            ## loop over all objects in file
>>> for key,obj in rfile.iteritems() : print key, obj             ## another loop
>>> for key,obj in rfile.iteritems( ROOT.TH1 ) : print key, obj   ## advanced loop
>>> for key in rfile.keys()     : print k   ## get all keys and loop over them 
>>> for key in rfile.iterkeys() : print k   ## loop over all keys in the file

>>> del  rfile['A/B']                       ## delete the object from the file
>>> rfile.rm ( 'A/B' )                      ## delete the object from the file

>>> if 'A/MyHisto' in rfile          : print 'OK!' ## check presence of the key
>>> if rfile.has_key ( 'A/MyHisto' ) : print 'OK!' ## check presence of the key

>>> with ROOT.TFile('aa.root') as rfile : rfile.ls() ## context manager protocol
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'ROOTCWD'      , ## context manager to keep Current Directory
    ) 
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
## @class ROOTCWD
#  context manager to preserve current directory (rather confusing stuff in ROOT)
#  @code
#  print ROOT.gROOT.CurrentDirectory() 
#  with ROOTCWD() :
#     print ROOT.gROOT.CurrentDirectory() 
#     rfile = ROOT.TFile( 'test.root' , 'recreate' )
#     print ROOT.gROOT.CurrentDirectory() 
#  print ROOT.gROOT.CurrentDirectory() 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
class ROOTCWD(object) :
    """
    Context manager to preserve current directory (rather confusing stuff in ROOT) 
    >>> print ROOT.gROOT.CurrentDirectory() 
    >>> with ROOTCWD() :
    ...     print ROOT.gROOT.CurrentDirectory() 
    ...     rfile = ROOT.TFile( 'test.root' , 'recreate' )
    ...     print ROOT.gROOT.CurrentDirectory() 
    ... print ROOT.gROOT.CurrentDirectory() 
    """
    ## context manager ENTER 
    def __enter__ ( self ) :
        "Save current working directory"
        self._dir = ROOT.gROOT.CurrentDirectory() 
        
    ## context manager EXIT 
    def __exit__  ( self , *_ ) :
        "Make the previous directory current again"
        if self._dir : self._dir.cd()

# ===============================================================================
## write the (T)object to ROOT-file/directory
#  @code
#  histo1 = ...
#  histo2 = ...
#  rfile['MyHisto'      ] = histo1 
#  rfile['MyDir/MyHisto'] = histo2
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_setitem_ ( rdir , name , tobj ) :
    """
    Write the object to ROOT-file/directory
    >>> rfile['myhisto'] = h1
    >>> rfile['MyDir/MyHisto'] = h2
    """
    ##
    if   not rdir              :
        raise IOError("TDirectory is invalid")
    elif not rdir.IsWritable() :
        raise IOError("TDirectory '%s' is not writable" % rdir.GetPath() ) 
    ##
    with ROOTCWD() :
        ##
        dirname , sep , fname = name.partition('/')
        ##
        while sep :
            rdir.cd() 
            subdir = rdir.GetDirectory(dirname)
            if not subdir :
                rdir.cd() 
                subdir = rdir.mkdir ( dirname  , name.replace('/','_') )
                rdir.cd() 
                subdir = rdir.GetDirectory( dirname )
                if not subdir :
                    raise KeyError("TDirectory[]: can't create '%s%s'" % ( rdir.GetPath() , dirname ) )
            rdir   = subdir
            dirname, sep , fname = fname.partition ('/') 

        rdir.cd()
        return  rdir.WriteTObject( tobj , dirname , 'WriteDelete' )
        ## DO NOT DELETE OBJECT IN ROOT 
        ## ROOT.SetOwnership( tobj , False )
        ## return val 
        
# ===============================================================================
## get the object from ROOT-file/directory
#  @code
#  h1 = rfile['MyHisto'      ] 
#  h2 = rfile['MyDir/MyHisto'] 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_getitem_ ( rdir , name ) :
    """
    Get the object from ROOT-file/directory
    >>> t = f['A/tup']
    >>> h = f['histo']
    """
    ##
    if not rdir : raise IOError("TDirectory is invalid")
    ##
    with ROOTCWD() :
        ##
        dirname , sep , fname = name.partition('/')
        ##
        while sep :
            rdir.cd()
            subdir = rdir.GetDirectory(dirname)
            if not subdir :
                raise KeyError("TDirectory[]: unknown directory name '%s%s'" % (rdir.GetPath() , dirname  ) ) 
            rdir = subdir
            dirname, sep , fname = fname.partition ('/') 

        rdir.cd()
        obj = rdir.Get ( dirname ) 
        if not obj : raise KeyError ("TDirectory[]: unknown object '%s%s'" % ( rdir.GetPath(), dirname ) )
        return obj 

# ===============================================================================
## get the object from ROOT-file/directory 
#  @code
#  h1 = rfile.MyHisto
#  h2 = rfile.MyDir.MyHisto
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_getattr_ ( rdir , name ) :
    """
    Get the object from ROOT-file
    >>> t = f.A.tup
    >>> h = f.myhisto
    """
    ##
    if not rdir : return None
    ##
    with ROOTCWD() :
        ##
        dirname , sep , fname = name.partition('/')
        if sep : raise AttributeError('TDirectory[]: invalid attribute %s.%s' % rdir.GetPath() , dirname )

        rdir.cd()
        obj = rdir.Get ( dirname ) 
        if not obj : raise AttributeError ( 'TDirectory[]: unknown attribute %s.%s' % ( rdir.GetPath(), dirname ) )
        return obj 
    
# ===============================================================================
## check the existence of key in the root-file/directory
#  @code
#  if not 'myhisto'   in rfile : break 
#  if 'MyDir/MyHisto' in rfile : print 'OK!'
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_contains_ ( rdir , name ) :
    """
    Check the existence of key in the root-file/directory
    >>> if not 'myhisto'  in rfile : break 
    >>> if 'MyDir/MyHisto' in rfile : print 'OK!'
    """
    ##
    if not rdir : return False 
    ##
    with ROOTCWD() :
        ##
        dirname , sep , fname = name.partition('/')
        ##
        while sep :
            rdir.cd()
            subdir = rdir.GetDirectory(dirname)
            if not subdir : return False
            rdir = subdir
            dirname, sep , fname = fname.partition ('/') 

        rdir.cd()
        return rdir.FindKey  ( dirname ) 

# =============================================================================
## delete object from ROOT file/directory
#  @code
#  del rfile['h1']
#  del rfile['mydir/h1']
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_delitem_ ( rdir , name , cycle=';*') :
    """
    Delete an object from ROOT file/directory
    >>> del rfile['h1']
    >>> del rfile['mydir/h1']
    """
    ##
    if   not rdir              :
        raise IOError("TDirectory is invalid")
    elif not rdir.IsWritable() :
        raise IOError("TDirectory '%s' is not writable" % rdir.GetPath() ) 
    ##
    with ROOTCWD () :
        ##
        dirname , sep , fname = name.partition('/')
        ##
        while sep :
            rdir.cd()
            subdir = rdir.GetDirectory(dirname)
            if not subdir :
                raise KeyError("TDirectory, can't delete %s" % name ) 
            rdir = subdir
            dirname, sep , fname = fname.partition ('/') 

        rdir.cd()
        if not rdir.FindKey( dirname ) :
            raise KeyError("TDirectory, can't delete %s" % dirname ) 
        return rdir.Delete( dirname + cycle ) 

# =============================================================================a
## get all keys from ROOT file/directory
#  @code
#  keys = rfile.keys()
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_keys_ ( rdir , recursive = True ) :
    """
    Get all keys from ROOT file/directory
    >>> keys = rfile.keys() 
    """
    _res = []
    if not rdir : return _res 
    ##
    with ROOTCWD() :
        ##
        rdir.cd() 
        _lst = rdir.GetListOfKeys()
        for i in _lst :
            inam = i.GetName()
            _res.append ( inam )
            if recursive and i.IsFolder() :
                idir  = rdir.GetDirectory( inam ) 
                ikeys = _rd_keys_ ( idir , recursive )
                for k in ikeys : _res.append ( inam + '/' + k )
                
        return _res
    
# =============================================================================a
## Iterate over the content of ROOT file/directory 
#  @code
#  for key,obj in rfile.iteritems() : print key,obj
#  @endcode
#  The scan can be limited only by objcets of certain type, e.g. histograms: 
#  @code
#  for key,hist in rfile.iteritems( ROOT.TH1 ) : print key,hist
#  @endcode
#  More complicated rules can be specified:
#  @code 
#  for key,obj in rfile.iteritems ( lambda k,o : k[0]=='M' ) : print key,obj
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_iteritems_ ( rdir , fun = lambda k,o : True , recursive = True ) :
    """
    Iterate over the content of ROOT directory/file:
    >>> for key,obj  in rfile.iteritems()           : print key , obj
    >>> for key,hist in rfile.iteritems( ROOT.TH1 ) : print key , hist
    >>> for key,obj  in rfile.iteritems( lambda name,tkey,obj : name[0]=='M' ) : print key,obj
    """
    ##
    if isinstance ( fun , type ) and issubclass ( fun , ( ROOT.TObject, cpp.TObject) ) : 
        tobj = fun 
        fun  = lambda k,t,o : isinstance ( o , tobj )
    ##
    with ROOTCWD() :
        ##
        rdir.cd() 
        _lst = rdir.GetListOfKeys()
        for i in _lst :
            inam = i.GetName()
            obj  = rdir.Get ( inam )
            if fun ( inam , i , obj ) : yield inam , obj
            if recursive and i.IsFolder() :
                idir  = rdir.GetDirectory( inam ) 
                for k, o in _rd_iteritems_ ( idir , fun , recursive ) :
                    yield k,o

# =============================================================================a
## Iterate over the keys in ROOT file/directory 
#  @code
#  for k in rfile.iterkeys() : print k
#  @endcode
#  The scan can be limited only by objects of certain type, e.g. histograms: 
#  @code
#  for k in rfile.iterkeys ( ROOT.TH1 ) : print k
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_iterkeys_ ( rdir , typ = None , recursive = True ) :
    """
    Iterate over the keys in ROOT file/directory 
    >>> for key,obj  in rfile.iteritems()           : print key , obj
    >>> for key,hist in rfile.iteritems( ROOT.TH1 ) : print key , hist
    """
    ##
    with ROOTCWD() :
        ##
        rdir.cd() 
        _lst = rdir.GetListOfKeys()
        for i in _lst :
            inam = i.GetName()
            if typ is None  or isinstance ( rdir.Get ( inam ) , typ ) : yield inam 
            if recursive and i.IsFolder() :
                idir  = rdir.GetDirectory( inam ) 
                for k in _rd_iterkeys_ ( idir , typ , recursive ) :
                    yield k

# =============================================================================a
## Iterate over the content of ROOT file/directory 
#  @code
#  for  obj  in rfile.itervalues() : print obj
#  @endcode
#  The scan can be limited only by objcets of certain type, e.g. histograms: 
#  @code
#  for  hist in rfile.itervalues( ROOT.TH1 ) : print hist 
#  @endcode
#  More complicated rules can be specified:
#  @code 
#  for  obj in rfile.itervalues( lambda k,o : k[0]=='M' ) : print obj
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_itervalues_ ( rdir , fun = lambda k,o : True , recursive = True ) :
    """
    Iterate over the content of ROOT directory/file:
    >>> for obj  in rfile.itervalues ()           : print obj
    >>> for hist in rfile.itervalues ( ROOT.TH1 ) : print hist
    >>> for obj  in rfile.itervalues ( lambda k,o : k[0]=='M' ) : print obj
    """
    ##
    with ROOTCWD() :
        ##
        rdir.cd()
        for key , obj in _rd_iteritems_ ( rdir , fun , recursive ) :
            yield obj 

# =============================================================================a
## Iterate (recursive) over the content in ROOT file/directory
#  @code
#  for obj in rfile : print obj
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_iter_ ( rdir ) :
    """
    Iterate (recursive) over the content in ROOT file/directory
    >>> for obj  in rfile : print obj
    """
    ##
    with ROOTCWD() :
        ##
        rdir.cd()
        for obj in _rd_itervalues_ ( rdir , recursive = True ) :
            yield obj


# =============================================================================
## delete object from ROOT file/directory
#  @code
#  rfile.rm('h1')
#  rfile.rm('mydir/h1')
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_rm_ ( rdir , name , cycle=';*') :
    """
    Delete an object from ROOT file/directory
    >>> rfile.rm( 'h1')
    >>> rfile.rm('mydir/h1')
    """
    ##
    try :
        return _rd_delitem_ ( rdir , name , cycle )
    except KeyError :
        return
            
# ===============================================================================
## get the object from ROOT-file 
#  @code
#  h1 = rfile.get( 'MyHisto' , default_value ) 
#  h2 = rfile.get( 'MyDir/MyHisto' )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
def _rd_get_ ( rdir , name , default = None ) :
    """
    Get the object from ROOT-file
    >>> h1 = rfile.get( 'MyHisto' , default_value ) 
    >>> h2 = rfile.get( 'MyDir/MyHisto')
    """
    ##
    if not rdir : return default 
    ##
    try :
        return _rd_getitem_ ( name )
    except KeyError :
        return default
    
# =============================================================================
## use ROOT-file with context-manager
#  @code
#  with ROOT.TFile('ququ.root') as f : f.ls() 
#  @endcode
def _rf_enter_ ( self      ) :
    """
    Use ROOT-file with the context manager
    >>> with ROOT.TFile('ququ.root') as f : f.ls() 
    """ 
    return self

# =============================================================================
## use ROOT-file with context-manager 
#  @code
#  with ROOT.TFile('ququ.root') as f : f.ls() 
#  @endcode
def _rf_exit_  ( self , *_ ) :
    """
    Use ROOT-file with the context manager    
    >>> with ROOT.TFile('ququ.root') as f : f.ls()
    """
    try :
        self.Close()
    except: pass

            
# =============================================================================a

## basic protocol:

ROOT.TDirectory.__getitem__  = _rd_getitem_ 
ROOT.TDirectory.__setitem__  = _rd_setitem_
ROOT.TDirectory.__contains__ = _rd_contains_
ROOT.TDirectory.__getattr__  = _rd_getattr_
ROOT.TDirectory.__delitem__  = _rd_delitem_
ROOT.TDirectory.__iter__     = _rd_iter_

## extended protocol

ROOT.TDirectory.get          = _rd_get_
ROOT.TDirectory.keys         = _rd_keys_
ROOT.TDirectory.has_key      = _rd_contains_
ROOT.TDirectory.iteritems    = _rd_iteritems_
ROOT.TDirectory.iterkeys     = _rd_iterkeys_
ROOT.TDirectory.itervalues   = _rd_itervalues_

## extra

ROOT.TDirectory.rm           = _rd_rm_

if not hasattr ( ROOT.TFile , 'close' ) :
    ROOT.TFile.close = ROOT.TFile.Close 

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
