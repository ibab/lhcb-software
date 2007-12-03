#!/usr/bin/env python
# =============================================================================
# $Id: tests.py,v 1.2 2007-12-03 11:14:51 ibelyaev Exp $
# =============================================================================
## @file   LoKiAlgo/tests.py
#  The file is a part of LoKi and Bender projects
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
"""
The basic tests for LoKiAlgo package

         This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================
from LoKiAlgo.decorators import *

# =============================================================================
## The most trivial test function
def test() :
    """
    The most trivial test function
    """
    from LoKiAlgo.decorators import _decorated
    print 'LoKiAlgoTest: decorated objects %s'%len(_decorated)
    pass
# =============================================================================
## Perform all known tests
def testAll() :
    """ Perform all known tests """
    test()

# =============================================================================
if '__main__' == __name__ : testAll ()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
