#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
# @file SQLiteShelve.py
# 
# This is SQLite-version of shelve database.
# 
# Keeping the same interface and functionlity as shelve data base,
# SQLiteShelve allows much more compact file size through
# the compression of the content
#
# The actual code has been taken from <c>sqlitedict</c> code
#  by Radim Rehurek <radimrehurek@seznam.cz>
# Hacked together from:
#  * http://code.activestate.com/recipes/576638-draft-for-an-sqlite3-based-dbm/
#  * http://code.activestate.com/recipes/526618/   ( see Google...)
#
# Create new DB:
#
# @code
#
# >>> import SQLiteShelve                     ## import the SQLiteShelve module 
# >>> db = SQLiteShelve.open ('a_db', 'n')    ## create new DB
# ...
# >>> abcde = ...
# >>> db['some_key'] =  abcde                 ## add information to DB
# ...
# >>> db.close()
#
# @endcode 
#
# Access to DB in read-only mode :
#
# @code
#
# >>> import SQLiteShelve                    ## import the SQLiteShelve module 
# >>> db = SQLiteShelve.open ('a_db' , 'r' ) ## access existing dbase in read-only mode
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
# >>> import SQLiteShelve                   ## import the SQLiteShelve module 
# >>> db = SQLiteShelve.open ('a_db' )      ## access existing dbase in update mode
# ...
# >>> for key in db : print key
# ...
# >>> abcd = db['some_key']
#
# @endcode 
#
# 
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2010-04-30
# 
#                    $Revision: 172762 $
#  Last Modification $Date: 2014-05-14 18:13:25 +0200 (Wed, 14 May 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
"""This is SQLite-version of shelve database.

Keeping the same interface and functionlity as shelve data base,
SQLiteShelve allows much more compact file size through
the compression of the content

The actual code has been inspired by <c>sqlitedict</c> ( see Google...)

Create new DB:

# >>> import SQLiteShelve                     ## import the SQLiteShelve module 
# >>> db = SQLiteShelve.open ('a_db', 'n')    ## create new DB
# ...
# >>> abcde = ...
# >>> db['some_key'] =  abcde                 ## add information to DB
# ...
# >>> db.close()

Access to DB in read-only mode :

# >>> import SQLiteShelve                     ## import the SQLiteShelve module 
# >>> db = SQLiteShelve.open ('a_db' , 'r' )  ## access existing dbase in read-only mode
# ...
# >>> for key in db : print key
# ...
# >>> abcd = db['some_key']


 Access existing DB in update mode :

# >>> import SQLiteShelve                    ## import the SQLiteShelve module 
# >>> db = SQLiteShelve.open ('a_db' )       ## access existing dbase in update mode
# ...
# >>> for key in db : print key
# ...
# >>> abcd = db['some_key']
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-06-19"
__version__ = "$Revision: 172762 $" 
# =============================================================================
__all__ = (
    'SQLiteShelf' ,   ## The DB-itself
    'open'            ## helper function to hide the actual DB
    )
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'PyPAW.SQLiteShelve' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
from .sqlitedict import SqliteDict
# =============================================================================
## @class SQLiteShelf
#  SQLite-based ``shelve-like'' database
#  
class SQLiteShelf(SqliteDict):
    """
    """
    def __init__ ( self                     ,
                   filename     = None      ,
                   mode         = 'c'       ,
                   tablename    = 'PyPaw'   ,
                   writeback    = True      , ## original name: "autocommit"
                   journal_mode = "DELETE"  ) :
        """
        Initialize a thread-safe sqlite-backed dictionary.
        The dictionary will be a table ``tablename`` in database file
        ``filename``. A single file (=database) may contain multiple tables.
        
        If no ``filename`` is given, a random file in temp will be used
        (and deleted from temp once the dict is closed/deleted).
        
        If you enable ``writeback/autocommit`` changes will be committed
        after each operation (more inefficient but safer).
        Otherwise, changes are committed on
        ``self.commit()``,
        ``self.clear()`` and
        ``self.close()``.
        
        Set ``journal_mode`` to ``OFF``
        if you're experiencing sqlite I/O problems
        or if you need performance and don't care about crash-consistency.
        
        The `flag` parameter:
        - 'c': default mode, open for read/write, creating the db/table if necessary.
        - 'w': open for r/w, but drop `tablename` contents first (start with empty table)
        - 'n': create a new database (erasing any existing tables, not just `tablename`!).
        
        """
        if not filename is None :
            import os 
            filename  = os.path.expandvars ( filename )
            filename  = os.path.expanduser ( filename )
            filename  = os.path.expandvars ( filename )
            
        SqliteDict.__init__ ( self                        ,
                              filename     = filename     ,
                              tablename    = tablename    ,
                              flag         = mode         ,
                              autocommit   = writeback    ,
                              journal_mode = journal_mode ) 


    ## list the avilable keys 
    def __dir ( self , pattern = '' ) :
        """
        List the avilable keys (patterns included).
        Pattern matching is performed accoriding to
        fnmatch/glob/shell rules [it is not regex!] 
        
        >>> db = ...
        >>> db.ls() ## all keys
        >>> db.ls ('*MC*')
        
        """
        keys_ = self.keys()
        keys_.sort()
        if pattern :
            import fnmatch
            _keys = [ k for k in keys_ if fnmatch.fnmatchcase ( k , pattern ) ]
            keys_ = _keys
        #
        for key in keys_ : print key
        
    ## list the avilable keys 
    def ls    ( self , pattern = '' ) :
        """
        List the avilable keys (patterns included).
        Pattern matching is performed accoriding to
        fnmatch/glob/shell rules [it is not regex!] 

        >>> db = ...
        >>> db.ls() ## all keys
        >>> db.ls ('*MC*')        
        
        """
        return self.__dir( pattern )
        

# =============================================================================
## open new SQLiteShelve data base
def open(*args, **kwargs):
    """See documentation of the SQLiteShelf class."""
    return SQLiteShelf(*args, **kwargs)

# =============================================================================
if '__main__' == __name__ :

    logger.info ( 100*'*' ) 
    logger.info ( __doc__ ) 
    logger.info ( 100*'*' ) 
    logger.info ( ' Author  : %s ' %        __author__    ) 
    logger.info ( ' Date    : %s ' %        __date__      ) 
    logger.info ( ' Version : %s ' %        __version__   ) 
    logger.info ( ' Logger  : %s ' % list ( __all__     ) ) 
    logger.info ( 100*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
