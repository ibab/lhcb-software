#!/usr/bin/env python
# =============================================================================
## @file   LoKiGenMC/tests.py
#
#  The set of basic functions for objects from LoKiGenMC library
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
The basic tests for LoKiGenMC package

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
import LoKiGen.HepMC
import LoKiMC.MC
import LoKiMC.decorators
import LoKiGen.decorators
import LoKiGenMC.decorators

## The most trivial test function
def test():
    """
    The most trivial test function
    """
    from LoKiGenMC.decorators import _decorated
    print 'LoKiGenMCTest: decorated objects %s'%len(_decorated)
    for o in _decorated : print o
# =============================================================================
## Perform all known tests
def testAll() :
    """
    Perform all known tests
    """
    test()


# =============================================================================
if '__main__' == __name__ : testAll ()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
