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

from   LoKiPhysMC.functions import *
import LoKiCore.decorators as _LoKiCore


_decorated  = _LoKiCore.getAndDecoratePredicates (
    __name__                                      , ## module name
    LoKi.Function('const LHCb::Particle*')        , ## the base 
    LoKi.Dicts.CutCalls(LHCb.Particle)            , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
_decorated |= _LoKiCore.getAndDecoratePredicates (
    __name__                                      , ## module name
    LoKi.Function('const LHCb::MCParticle*')      , ## the base 
    LoKi.Dicts.CutCalls(LHCb.MCParticle)          , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::MCParticle*') ) ## operators 

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
 
# =============================================================================
# The END 
# =============================================================================

