#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiCore/decorators.py
#  The set of basic decorator for objects from LoKiGenMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiGenMC library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import PyCintex 
PyCintex.loadDict("libLoKiGenDict")
PyCintex.loadDict("libLoKiMCDict")
#PyCintex.loadDict("libLoKiGenMCDict")


import LoKiCore.decorators as _LoKiCore
from   LoKiGenMC.functions import * 


_name = __name__

# =============================================================================
## make a decoration of the actual functions 
def decorate ( name = _name ) :
    
    _decorated  = _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name
        LoKi.Predicate('const HepMC::GenParticle*')     , ## the base 
        LoKi.Dicts.CutCalls(HepMC.GenParticle)          , ## call-traits 
        LoKi.Dicts.FuncOps('const HepMC::GenParticle*') ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name
        LoKi.Predicate('const LHCb::MCParticle*')       , ## the base 
        LoKi.Dicts.CutCalls(LHCb.MCParticle)            , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::MCParticle*')   ) ## operators
    ## 
    return _decorated                                      ## RETURN 
# =============================================================================

# =============================================================================
## perform the actual decoration 
_decorated = decorate() 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
    
# =============================================================================
# The END
# =============================================================================
