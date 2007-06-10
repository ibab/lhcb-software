#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2007/06/01 12:24:50  ibelyaev
#  prepare for v5r0
#
# =============================================================================
## @file decorators.py LoKiPhysMC/decorators.py
#  The set of basic decorator for objects from LoKiPhysMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiPhysMC library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from LoKiCore.decorators import LoKi 
from LoKiCore.decorators import LHCb

MCTRUTH  = LoKi.PhysMCParticles.MCTruth 
GMCTRUTH = LoKi.PhysMCParticles.GenMCTruth
RCTRUTH  = LoKi.PhysMCParticles.RCTruth

 
# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        

# =============================================================================
# The END 
# =============================================================================

