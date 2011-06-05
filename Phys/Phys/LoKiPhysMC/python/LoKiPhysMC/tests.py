#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision$
# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
## @file   LoKiPhysMC/tests.py
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-05-29
# =============================================================================
""" The basic tests for LoKiGenMC package """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================
## The most trivial test function
def test() :
    """ The most trivial test function """
    from LoKiPhysMC.decorators import _decorated
    print 'LoKiPhysMCTest: decorated objects %s'%len(_decorated)
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
