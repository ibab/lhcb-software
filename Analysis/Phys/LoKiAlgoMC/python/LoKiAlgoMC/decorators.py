#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiAlgoMC/decorators.py
#  The set of basic decorator for objects from LoKiAlgoMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for objects from LoKiAlgoMC library
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators   as _LoKiCore
import LoKiMC.decorators     as _LoKiMC
import LoKiGen.decorators    as _LoKiGen
import LoKiGenMC.decorators  as _LoKiGenMC
import LoKiPhys.decorators   as _LoKiPhys 
import LoKiPhysMC.decorators as _LoKiPhysMC
import LoKiAlgo.decorators   as _LoKiAlgo
from   LoKiAlgoMC.functions  import *

LoKi = _LoKiCore.LoKi
LHCb = _LoKiCore.LHCb

##_alg = LoKi.AlgoMC

import LoKiAlgoMC.fArrayMCP
import LoKiAlgoMC.fArrayGP

# =============================================================================
## decorate MC-algorithm: 
AlgoMC = _LoKiAlgo._decorateAlg( 'LoKi::AlgoMC' )
# =============================================================================

_name = _LoKiPhysMC.__name__

# =============================================================================
## make the decoration
_decorate = _LoKiAlgo._decorate 

# =============================================================================

# =============================================================================
## perform the actual decoration
_decorated = _decorate( _name ) 
# =============================================================================
    
# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o


# =============================================================================
# The END 
# =============================================================================
