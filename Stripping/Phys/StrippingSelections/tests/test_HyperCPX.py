#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file test_HyperCPX.py
#  helper script to test HyperCPXConf-configurabloid
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-04-20
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Helper script to test HyperCPXConf-configurabloid
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.ch'
__date__    = '2011-04-20'
__version__ = '$Revision$'
# =============================================================================

from StrippingUtils import LineBuilderTests

from StrippingSelections.StrippingHyperCPX import HyperCPXConf as builder

# =============================================================================
## the actual function to test stripping configurabloid 
def test_line_builder() :
    """
    The actual function to test stripping configurabloid 
    """
    LineBuilderTests.test_line_builder(builder, builder.defaultConfiguration() )
    
# =============================================================================
if __name__ == '__main__' :
    
    test_line_builder()
    
# =============================================================================
# The END 
# =============================================================================

