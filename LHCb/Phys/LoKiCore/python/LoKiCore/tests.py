#!/usr/bin/env python
# =============================================================================
# $Id: tests.py,v 1.3 2008-11-22 16:19:58 ibelyaev Exp $
# =============================================================================
## @file   LoKiCore/tests.py
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
"""
The basic tests for LoKiCore package
"""
# =============================================================================
__author__   = "Vanya BELYAEV  Ivan.Belyaev@nikhef.nl"
__version__  = "CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.3 $ "
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
    testAll ()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
