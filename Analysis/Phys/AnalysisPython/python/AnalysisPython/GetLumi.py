#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Helper function to extract luminosity 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-16
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Helper function to extract luminosity 

Get lumi :

>>> l1 = getLumi ( 'myfile.root' )
>>> l2 = getLumi ( tree  )
>>> l3 = getLumi ( chain )
>>> l4 = getLumi ( file  )
>>> l5 = getLumi ( [ any sequence of above ]  )

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2012-10-16"
# =============================================================================
__all__     = (
    #
    'getLumi'  ,  ## get the lumi
    #
    )
# =============================================================================
import warnings
warnings.warn (
    """AnalysisPython:
    Use 'PyPAW.GetLumi' module instead of 'AnalysisPython.GetLumi'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from PyPAW.GetLumi import getLumi

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
