#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
# @file ZipShelve.py
# 
# This is zip-version of shelve database.
# 
# Keeping the same interface and functionlity as shelve data base,
# ZipShelf allows much more compact file size through the on-flight
# compression of the content
#
# The actual code has been inspired by <c>zipshelve</c> ( see Google...)
#
# However is contains several new features:
# 
#  - Optionally it is possible to perform the compression
#    of the whole data base, that can be rathe useful fo data base
#    with large amout of keys 
#
# The module has been developed and used with great success in
# ``Kali, framework for fine calibration of LHCb Electormagnetic Calorimeter''
#
#
# Create new DB:
#
# @code
#
# >>> import ZipShelve  ## import the ZipShelve module 
# >>> db = zipShelve.open ('a_db', 'n')    ## create new DB
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
# >>> import ZipShelve  ## import the ZipShelve module 
# >>> db = zipShelve.open ('a_db' , 'r' )    ## access existing dbase in read-only mode
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
# >>> import ZipShelve  ## import the ZipShelve module 
# >>> db = zipShelve.open ('a_db' )    ## access existing dbase in update mode
# ...
# >>> for key in db : print key
# ...
# >>> abcd = db['some_key']
#
# @endcode 
#
# @attention: In case DB-name has extention "gz", the whoel data base
#             will be gzipped. 
#
#
# 
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2010-04-30
# 
#                    $Revision$
#  Last Modification $Date$
#                 by $Author$
# =============================================================================
"""

This is zip-version of shelve database.

Keeping the same interface and functionlity as shelve data base,
ZipShelf allows much more compact file size through the on-flight
compression of the content

The actual code has been inspired by zipshelve ( see Google...)

However is contains several new features:

 - Optionally it is possible to perform the compression
   of the whole data base, that can be rathe useful fo data base
   with large amout of keys 
   
The module has been developed and used with great success in
 ``Kali, framework for fine calibration of LHCb Electormagnetic Calorimeter''

 Create new DB:

 >>> import ZipShelve  ## import the ZipShelve module 
 >>> db = zipShelve.open ('a_db', 'n')    ## create new DB
 ...
 >>> abcde = ...
 >>> db['some_key'] =  abcde              ## add information to DB
 ...
 >>> db.close()

 Access to DB in read-only mode :

 >>> import ZipShelve  ## import the ZipShelve module 
 >>> db = zipShelve.open ('a_db' , 'r' )    ## access existing dbase in read-only mode
 ...
 >>> for key in db : print key
 ...
 >>> abcd = db['some_key']

 Access existing DB in update mode :

 >>> import ZipShelve  ## import the ZipShelve module 
 >>> db = zipShelve.open ('a_db' )    ## access existing dbase in update mode
 ...
 >>> for key in db : print key
 ...
 >>> abcd = db['some_key']

 In case DB-name has extention 'gz', the whole data base will be gzipped

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2010-04-30"
__version__ = "$Revision$" 
# =============================================================================
__all__ = (
    'ZipShelf' ,   ## The DB-itself
    'open'         ## helper function to hide the actual DB
    )
# =============================================================================
import warnings
warnings.warn (
    """AnalysisPython:
    Use 'PyPAW.ZipShelve' module instead of 'AnalysisPython.ZipShelve'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from PyPAW.ZipShelve import *

# =============================================================================
if '__main__' == __name__ :

    print 100*'*'
    print __doc__
    print 100*'*'
    print ' Author  : ' , __author__
    print ' Date    : ' , __date__
    print ' Version : ' , __version__
    print ' Symbols : ' , __all__
    print 100*'*'
    
# =============================================================================
# The END 
# =============================================================================
