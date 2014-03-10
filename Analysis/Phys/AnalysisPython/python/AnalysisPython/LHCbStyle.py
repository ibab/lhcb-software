#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
# @file LHCb style file
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
    """AnalysisPython:
    Use 'PyPAW.LHCbStyle' module instead of 'AnalysisPython.LHCbStyle'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from PyPAW.LHCbStyle import *


# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
