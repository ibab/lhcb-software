#!/usr/bin/env python
# =============================================================================
# $Id: tests.py,v 1.4 2010-05-30 17:06:43 ibelyaev Exp $
# =============================================================================
## @file   LoKiCore/tests.py
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
The basic tests for LoKiCore package

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__   = "Vanya BELYAEV  Ivan.Belyaev@nikhef.nl"
__date__     = "2007-05-29"
__version__  = "CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.4 $ "
# =============================================================================
## The most trivial test function
def test() :
    """
    The most trivial test function
    """
    import LoKiCore.decorators
    import LoKiCore.math
    from LoKiCore.functions import LoKi 
    
    LoKi.Welcome.instance().welcome() 
    
    pass
# =============================================================================
## Perform all known tests
def testAll() :
    """ Perform all known tests """
    test()
    
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    
    testAll ()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
