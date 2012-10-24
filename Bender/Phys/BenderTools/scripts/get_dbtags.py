#!/usr/bin/env python
# =============================================================================
# $Id:$ 
# =============================================================================
## @file 	
#  Simple script to extract DB-tags from the file 
#
#  @code
# 
#  Usage:
#
#     get_dbtags [options] file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode
#
#  @author Thomas RUF 
#  @author Vanya BELYAEV 
#  @date   2012-10-24
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
#
# =============================================================================
"""
Simple script to extract DB-tags from the file

Usage:

    get_dbtags [options] file1 [ file2 [ file3 [ file4 ....'

"""
# =============================================================================
__author__  = "Thomas RUF, Vanya BELYAEV"
__date__    = "2012-10-24"
__version__ = "Revision"

# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print __doc__ 
    print ' Author(s)  : ', __author__ 
    print ' Version    : ', __version__
    print ' Date       : ', __date__ 
    print 120*'*'
    
    from BenderTools.GetDBtags  import extractTags 

    import sys
    extractTags ( sys.argv[1:] )
    
# =============================================================================
# The END 
# =============================================================================
