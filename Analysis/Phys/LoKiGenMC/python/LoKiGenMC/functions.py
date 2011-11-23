#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiGenMC/functions.py
#  The set of basic functions from LoKiGenMC library
#  The set of basic decorator for objects from LoKiGenMC library
#  The file is a part of LoKi and Bender projects
#
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
# =============================================================================
"""
The set of basic functions from LoKiGenMC library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
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
