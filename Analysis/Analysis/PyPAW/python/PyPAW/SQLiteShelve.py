#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
# @file SQLiteShelve.py
# 
# This is SQLite-version of shelve database with compressed content 
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
# The compression (with zlib) is added atop of original code 
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
The compression (with zlib) is added atop of original code 
 
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
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.SQLiteShelve' module instead of 'PyPAW.SQLiteShelve'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.SQLiteShelve import *

# =============================================================================
if '__main__' == __name__ :

    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' Symbols : %s'  %  list (  __all__ ) 
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
