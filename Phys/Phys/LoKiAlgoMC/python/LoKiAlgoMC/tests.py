#!/usr/bin/env python
# =============================================================================
# $Id: tests.py,v 1.1 2007-06-01 12:30:51 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
## @file   LoKiAlgoMC/tests.py
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
""" The basic tests for LoKiAlgoMC package """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================
## The most trivial test function
def test() :
    """ The most trivial test function """
    from LoKiAlgoMC.decorators import _decorated
    print 'LoKiAlgoMCTest: decorated objects %s'%len(_decorated)
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
