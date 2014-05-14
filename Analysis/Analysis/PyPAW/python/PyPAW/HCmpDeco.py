#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Module with utilities for specific comparion of histograms/functions/shapes 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-10
#  
#                    $Revision: 172094 $
#  Last modification $Date: 2014-04-29 16:02:06 +0200 (Tue, 29 Apr 2014) $
#  by                $Author: albarano $
# =============================================================================
"""
Module with utilities for specific comparion of histograms/functions/shapes 
"""
# =============================================================================
__version__ = "$Revision: 172094 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT             ## attention here!!
import PyPAW.HParamDeco 
from   PyPAW.PyRoUts     import hID 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Some specific comparison of histo-objects')
# =============================================================================
## Can 1D-histogram can be considered as ``constant'' ?  
def _h1_constant_ ( h1 , mn , mx , prob = 0.90 , delta = 0.001 ) :
    """
    Can  1D-historgam be considered as constant ? 
    """
    # 
    if not isinstance ( h1 , ( ROOT.TH1D , ROOT.TH1F ) ) : return False 
    #
    r  = h1.Fit ( 'pol0','S0Q' )
    if 0 != r.Status() : 
        logger.warning("Can't fit with constant function %s" % r.Status() )
        return False
    #
    for i in range ( 0 , 5 ) :
        v   = r[0]
        h1 /= v
        r   = h1.Fit ( 'pol0','S0Q' )
        if 0 != r.Status() : 
            logger.warning("Can't fit with constant function %s" % r.Status() )
            return False
        ## compare!
        v = r[0]
        if abs ( v - 1.0 ) < delta and r.Prob() >= prob : return r
        
    return False

ROOT.TH1D.is_constant = _h1_constant_
ROOT.TH1F.is_constant = _h1_constant_

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
