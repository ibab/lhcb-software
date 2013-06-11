#!/usr/bin/env python
# =============================================================================
# $Id: get_metainfo.py 152803 2013-02-23 13:29:42Z ibelyaev $ 
# =============================================================================
## @file 	
#  Simple script to extract MetaInfo from data files 
#
#  @code
# 
#  Usage:
#
#     get_metainfo [options] file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-24
#
#                    $Revision: 152803 $
#  Last modification $Date: 2013-02-23 14:29:42 +0100 (Sat, 23 Feb 2013) $
#                 by $Author: ibelyaev $
#
# =============================================================================
"""
Simple script to extract MetaInfo from data files 

Usage:

    get_metainfo [options] file1 [ file2 [ file3 [ file4 ....'

"""
# =============================================================================
__author__  = "Vanya BELYAEV"
__date__    = "2012-10-24"
__version__ = "$Revision:$"

# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print __doc__ 
    print ' Author(s)  : ', __author__ 
    print ' Version    : ', __version__
    print ' Date       : ', __date__ 
    print 120*'*'
    
    from BenderTools.GetDBtags  import extractMetaInfo

    import sys
    extractMetaInfo ( sys.argv[1:] )
    
# =============================================================================
# The END 
# =============================================================================
