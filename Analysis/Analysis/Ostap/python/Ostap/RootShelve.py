#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
# @file RootShelve.py
# 
# This is shelve-like database with ROOT.TFile as internal storage 
#
# @see ZipShelve
# @see SQLiteShelve
#
#
# Create new DB:
#
# @code
#
# >>> import RootShelve as DBASE   ## import the RootShelve module 
# >>> db = DBASE.open ('a_db', 'n')    ## create new DB
# ...
# >>> abcde = ...
# >>> db['some_key'] =  abcde              ## add information to DB
# ...
# >>> db.close()
#
# @endcode 
#
# Access to DB in read-only mode :
#
# @code
#
# >>> import RootShelve  as DBASE ## import the ZipShelve module 
# >>> db = DBASE.open ('a_db' , 'r' )    ## access existing dbase in read-only mode
# ...
# >>> for key in db : print key
# ...
# >>> abcd = db['some_key']
#
# @endcode 
#
# Access existing DB in update mode :
#
# @code
#
# >>> import RootShelve as DBASE  ## import the ZipShelve module 
# >>> db = DBASE.open ('a_db' )    ## access existing dbase in update mode
# ...
# >>> for key in db : print key
# ...
# >>> abcd = db['some_key']
#
# @endcode 
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2015-07-31
# 
#                    $Revision$
#  Last Modification $Date$
#                 by $Author$
# =============================================================================
""" This is ROOT-based version of shelve database.

 Create new DB:

 >>> import RootShelve as DBASE ## import the ZipShelve module 
 >>> db = DBASE.open ('a_db', 'n')    ## create new DB
 ...
 >>> abcde = ...
 >>> db['some_key'] =  abcde              ## add information to DB
 ...
 >>> db.close()

 Access to DB in read-only mode :

 >>> import RootShelve as DBASE  ## import the ZipShelve module 
 >>> db = DBASE.open ('a_db' , 'r' )    ## access existing dbase in read-only mode
 ...
 >>> for key in db : print key
 ...
 >>> abcd = db['some_key']

 Access existing DB in update mode :

 >>> import RootShelve as DBASE ## import the ZipShelve module 
 >>> db = DBASE.open ('a_db' )    ## access existing dbase in update mode
 ...
 >>> for key in db : print key
 ...
 >>> abcd = db['some_key']
 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-07-31"
__version__ = "$Revision$" 
# =============================================================================
__all__ = (
    'RootShelf' , ## The DB-itself
    'open'        ## helper function to hide the actual DB
    )
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'RootShelve' )
else                      : logger = getLogger ( __name__     )
# =============================================================================
logger.debug ( "Simple generic ROOT-based shelve-like-database" )
# =============================================================================
import ROOT, shelve
# =============================================================================
## @class RootShelfBase
#  Plain vanilla DBASE for ROOT-object (only)
#  essentially it is nothing more than just shelve-like interface for ROOT-files
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2015-07-31
#  @attention it depends on proper TFile-decorations in Ostap.TFileDeco module
#  @see Ostap.TFileDeco
class RootShelfBase(shelve.Shelf):
    """
    Plain vanilla DBASE for ROOT-object (only)
    Essentially it is nothing more than just shelve-like
    interface for ROOT-files
    """
    ## constructors 
    #  @attention it depends on proper TFile-decorations in Ostap.TFileDeco module
    def __init__( self, filename , mode , writeback=False , *args ):
        
        _modes = { 'n'  : 'NEW'      ,
                   'c'  : 'RECREATE' ,
                   'r'  : 'READ'     ,
                   'w'  : 'WRITE'    ,
                   'u'  : 'UPDATE'   ,
                   'w+' : 'UPDATE'   ,
                   'rw' : 'UPDATE'   ,
                   '+'  : 'UPDATE'   ,
                   'a'  : 'UPDATE'   }
        
        from Ostap.TFileDeco import ROOTCWD 
        with ROOTCWD() : ## preserve current directory in ROOT 
            mode  = _modes.get ( mode.lower()  , mode.upper() ) 
            rfile = ROOT.TFile.Open ( filename , mode , *args  )
            shelve.Shelf.__init__ ( self , rfile , writeback ) 
            
    def __enter__   ( self       ) : return self 
    def __exit__    ( self , *_  ) : self.close ()

# =============================================================================
## get item from ROOT-file
#  @code
#  obj = db['A/B/C/histo']
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2015-07-31 
def _root_getitem_ ( self , key ) :
    """
    Get item from ROOT-file
    >>> obj = db['A/B/C/histo']
    """
    try:
        value = self.cache[key]
    except KeyError:
        value = self.dict[key] 
        if self.writeback:
            self.cache[key] = value
    return value
    
# =============================================================================
## put item into ROOT-file 
#  @code
#  db['A/B/C/histo'] = obj
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2015-07-31 
def _root_setitem_ ( self , key , value ) :
    """
    Put item into ROOT-file
    >>> db['A/B/C/histo'] = obj
    """
    if self.writeback:
        self.cache[key] = value
    self.dict[key] = value 


RootShelfBase.__getitem__ = _root_getitem_
RootShelfBase.__setitem__ = _root_setitem_

# =============================================================================
## @class RootShelf
#  The actual class for ROOT-based shelve-like data base
#  it implement shelve-intergase with underlyinog ROOT-fiel storage
#  - ROOT-object are store ddirectly in the ROOT-file,
#  - other objects are pickled and stored in ROOT.TObjString
#  @code
#  db = RootShelf( 'mydb.root' , 'c' )
#  db['histo'] = h1
#  db['tuple'] = ('a',1,h1) 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2015-07-31 
class RootShelf(RootShelfBase):
    """
    The actual class for ROOT-based shelve-like data base
    it implement shelve-intergase with underlyinog ROOT-fiel storage
    - ROOT-object are store ddirectly in the ROOT-file,
    - other objects are pickled and stored in ROOT.TObjString
    
    >>> db = RootShelf( 'mydb.root' , 'c' )
    >>> db['histo'] = h1
    >>> db['tuple'] = ('a',1,h1)
    """
    def __init__( self, filename , mode , writeback=False , *args ):
        RootShelfBase.__init__ ( self , filename , mode , writeback , *args )
        

# =============================================================================
##  get object (unpickle if needed)  from dbase
#   @code
#   obj = db['A/B/C']
#   @endcode
#   @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#   @date   2015-07-31 
def _pickled_getitem_ (self, key):
    """
    Get object (unpickle if needed)  from dbase
    >>> obj = db['A/B/C']
    """
    ##
    from   shelve import StringIO, Unpickler 
    ##
    try:
        value = self.cache[key]
    except KeyError:
        value = self.dict[key]
        ## object string?
        if isinstance ( value , ROOT.TObjString ) :
            ## unpack it unpickle!
            s     = value.GetName()
            ##
            if 0<= s.find ('\377\001') or 0<=s.find('\377\376') : 
                ## restore zeroes
                s     = s.replace('\377\001', '\000').replace('\377\376', '\377')
            ## unpickle! 
            f     = StringIO ( s )
            value = Unpickler( f ).load()
        if self.writeback:
            self.cache[key] = value
    return value

# =============================================================================
##  Add object (pickle if needed)  to dbase
#   @code
#   db['A/B/C'] = obj
#   @endcode
#   @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#   @date   2015-07-31 
def _pickled_setitem_ ( self , key , value ) :
    """
    Add object (pickle if needed)  to dbase
    >>> db['A/B/C'] = obj
    """
    ##
    from   shelve import StringIO, Pickler
    ##
    if self.writeback:
        self.cache[key] = value
    ## not TObject? pickle it and convert to TObjString
    if not isinstance  ( value , ROOT.TObject ) :
        ## pickle it 
        f = StringIO ( )
        p = Pickler  ( f , 2 ) ## PROTOCOL 
        p.dump( value )
        s = f.getvalue()
        ##
        if 0<= s.find('\377') or 0<=s.find('\000') : 
            ## avoid appearence of zeroes in TObjString
            s = s.replace('\377', '\377\376').replace('\000', '\377\001')
        value = ROOT.TObjString( s )
    ## finally use ROOT 
    self.dict[key] = value
    
RootShelf.__getitem__ = _pickled_getitem_
RootShelf.__setitem__ = _pickled_setitem_

# =============================================================================
## helper function to open RootShelve data base
#  @code
#  import RootShelve as DBASE
#  db = DBASE.open ( 'mydb.root' , 'c' )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date   2010-04-30
def open ( filename              ,
           mode          = 'c'   ,
           writeback     = False , *args ) : 
    """
    Helper function to open RootShelve data base
    >>> import RootShelve as DBASE
    >>> db = DBASE.open ( 'mydb.root' , 'c' )
    """    
    return RootShelf ( filename  ,
                       mode      ,
                       writeback , * args )

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
