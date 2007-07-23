# $Id: tests.py,v 1.2 2007-07-23 17:07:42 ibelyaev Exp $
# =============================================================================
## @file   LoKiCore/tests.py
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
""" The basic tests for LoKiCore package """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================
## The most trivial test function
def test() :
    """ The most trivial test function """
    import LoKiCore.decorators
    import LoKiCore.math
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
