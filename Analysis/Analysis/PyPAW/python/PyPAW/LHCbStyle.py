#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
# @file
# LHCb style file for ROOT-plots
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
LHCb Style for ROOT-plots 
"""
# =============================================================================
import ROOT
__all__ = (
    'lhcbStyle' ,
    'lhcbLabel' ,
    'LHCbStyle' ,
    'lhcbLatex' ,
    )
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.LHCbStyle' module instead of 'PyPAW.LHCbStyle'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.LHCbStyle import *

# =============================================================================
if '__main__' == __name__ :
    
    print '*'*120
    print                      __doc__
    # print ' Author  : %s ' %   __author__    
    # print ' Version : %s ' %   __version__
    # print ' Date    : %s ' %   __date__
    print ' Symbols : '    ,   __all__
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
