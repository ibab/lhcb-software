#!/usr/bin/env python
# =============================================================================
## @file   LoKiHlt/tests.py
#  The simple tets script for Phys/LoKiHlt package
#
#    This file is a part of LoKi project - 
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
The basic tests for Phys/LoKiHlt package

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg 
A.Golutvin, P.Koppenburg have been used in the design.


"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $" 
# =============================================================================

# temporary:
import GaudiPython.Pythonizations

from LoKiHlt.decorators  import *
from LoKiHlt.algorithms  import *
from LoKiCore.functions  import *
from LoKiCore.math       import *
# =============================================================================
## The most trivial test function
def test0() :
    """
    The most trivial test function
    """
    pass

# =============================================================================
## Perform all known tests
def testAll() :
    """
    Perform all known tests
    """
    test0()
    
    
# =============================================================================
if '__main__' == __name__ :
    testAll ()
    print dir()
# =============================================================================
    
# =============================================================================
# The END 
# =============================================================================
