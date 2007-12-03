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
import LoKiPhysMC.decorators as _LoKiPhysMC
from   LoKiAlgoMC.functions  import *
import LoKiAlgo.decorators   as _LoKiAlgo

LoKi = _LoKiCore.LoKi
LHCb = _LoKiCore.LHCb

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
