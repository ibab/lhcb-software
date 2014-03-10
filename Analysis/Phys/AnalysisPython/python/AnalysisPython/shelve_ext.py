#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
# @file shelve_ext
# 
# This is small extension for standars shelve module to allow more
# flexible treatment for the data base 
#
# @code
#
# >>> db1 = shelve.open ('$HOME/newdb'  , 'n')    ## create new DB
# >>> db2 = shelve.open ('../dir/newdb' , 'n')    ## create new DB
# ...
# >>> db1.ls()  ## list the content 
#
# @endcode 
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2012-05-13
# 
#                    $Revision$
#  Last Modification $Date$
#                 by $Author$
# =============================================================================
"""
This is small extension for standars shelve module to allow more
flexible treatment for the data base 

>>> db1 = shelve.open ('$HOME/newdb'  , 'n')    ## create new DB
>>> db2 = shelve.open ('../dir/newdb' , 'n')    ## create new DB
...
>>> db1.ls()  ## list the content 

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2012-05-13"
__version__ = "$Revision$" 
__all__     = ()   ## nothing to import 
# =============================================================================
import warnings
warnings.warn (
    """AnalysisPython:
    Use 'PyPAW.shelve_ext' module instead of 'AnalysisPython.shelve_ext'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from PyPAW.shelve_ext import *

# =============================================================================
if '__main__' == __name__ :

    print 100*'*'
    print __doc__
    print 100*'*'
    print ' Author  : ' , __author__
    print ' Date    : ' , __date__
    print ' Version : ' , __version__
    print 100*'*'
    
# =============================================================================
# The END 
# =============================================================================
