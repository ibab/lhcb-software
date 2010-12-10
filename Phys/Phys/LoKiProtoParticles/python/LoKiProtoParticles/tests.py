#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file   LoKiProtoParticles/tests.py
#  The simple test script for Phys/LoKiProtoParticles package
#
#        This file is a part of LoKi project - 
#  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-27
# =============================================================================
"""
The simple test script for Phys/LoKiProtoParticles package

        This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

  The package has been designed with the kind help from
  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
  contributions and advices from G.Raven, J.van Tilburg, 
  A.Golutvin, P.Koppenburg have been used in the design.
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2010-05-27"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision$"
# =============================================================================

# temporary:
import GaudiPython.Pythonizations

from LoKiProtoParticles.decorators import *
from LoKiCore.functions            import *
from LoKiCore.math                 import *
# =============================================================================
## The most trivial test function
def test0() :
    """
    The most trivial test function
    """
    from LoKiProtoParticles.decorators import _decorated
    print 'LoKiProtoParticlesTest: decorated objects %s'%len(_decorated)
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

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__ 
    print ' Date    : ' , __date__ 
    print ' Version : ' , __version__ 
    print 80*'*'
    
    testAll ()
    print 80*'*'
# =============================================================================
    
# =============================================================================
# The END 
# =============================================================================
