#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
## @file decorators.py LoKiPhysMC/decorators.py
#  The set of basic decorator for objects from LoKiPhysMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiPhysMC library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore
import LoKiPhys.decorators as _LoKiPhys
import LoKiMC.decorators   as _LoKiMC 
import LoKiGen.decorators  as _LoKiGen

LoKi = _LoKiPhys.LoKi
LHCb = _LoKiPhys.LHCb


MCTRUTH  = LoKi.Particles.MCTruth 
GMCTRUTH = LoKi.Particles.GenMCTruth
RCTRUTH  = LoKi.MCParticles.RCTruth

 
# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        

# =============================================================================
# The END 
# =============================================================================

