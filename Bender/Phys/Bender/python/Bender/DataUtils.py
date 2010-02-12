#!/usr/bin/env python
# =============================================================================
# @file Bender/DataUtils.py
# Helper module to deal with data
# @author VanyaBELYAEV Ivan.Belyaev@nikhef.nl
# @date   2010-02-12
# =============================================================================
"""
Helper module to deal with data
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2010-02-12"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
## Helper function to 'extend' the short file name
#  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
#  @date 2010-02-12
def extendfile ( filename ) :
    """
    Helper function to 'extend' the short file name 
    """
    if 0 <= filename.find(' ') : return filename
    ## 
    if 0 == filename.find (  '/castor'   ) : filename = 'castor:' + filename
    if 0 == filename.find ( '//castor'   ) : filename = 'castor:' + filename
    if 0 == filename.find ( '/lhcb/data' ) : filename = 'LFN:'    + filename
    ##
    pattern = "DATAFILE='%s' %s OPT='READ'"
    typ     = "TYPE='POOL_ROOTTREE'"
    if 0 <= filename.find ( 'mdf' ) : type = "SVC='LHCb::MDFSelector'"
    if 0 <= filename.find ( 'raw' ) : type = "SVC='LHCb::MDFSelector'"
    return pattern %  ( filename , type )

# =============================================================================
# The END 
# =============================================================================
