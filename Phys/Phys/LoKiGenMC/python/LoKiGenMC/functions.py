#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiGenMC/functions.py
#  The set of basic functions from LoKiGenMC library
#  The set of basic decorator for objects from LoKiGenMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic functions from LoKiGenMC library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore
import LoKiGen.decorators  as _LoKiGen
import LoKiMC.decorators   as _LoKiMC


LoKi   = _LoKiGen.LoKi
HepMC  = _LoKiGen.HepMC
LHCb   = _LoKiMC.LHCb

FROMGTREE = LoKi.GenMCParticles.IsFromHepMC
GMCMOTH   = LoKi.GenMCParticles.IsAMotherForMC


# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        
        
# =============================================================================
# The END
# =============================================================================
