#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiPhysMC/decorators.py
#  The set of basic decorator for objects from LoKiPhysMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiPhysMC library
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
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

