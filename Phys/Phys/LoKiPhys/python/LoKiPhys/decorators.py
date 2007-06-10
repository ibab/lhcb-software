#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiPhys/decorators.py
#  The set of basic decorator for objects from LoKiPhys library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiPhys library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiPhys.functions  import *

_name = __name__ 
# =============================================================================
## make the decoration of all objects from this module
def _decorate ( name = _name ) :
    """ Make the decoration of all objects from this module """
    import LoKiCore.decorators as     _LoKiCore
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                          , ## module name 
        LoKi.Function('const LHCb::Particle*')        , ## the base 
        LoKi.Dicts.FunCalls(LHCb.Particle)            , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                          , ## module name 
        LoKi.Function('const LHCb::VertexBase*')      , ## the base 
        LoKi.Dicts.FunCalls(LHCb.VertexBase)          , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::VertexBase*') ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Predicate('const LHCb::Particle*')       , ## the base 
        LoKi.Dicts.CutCalls(LHCb.Particle)            , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Predicate('const LHCb::VertexBase*')     , ## the base 
        LoKi.Dicts.CutCalls(LHCb.VertexBase)          , ## call-traits 
        LoKi.Dicts.FuncOps('const LHCb::VertexBase*') ) ## the operators 
    # decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( ID , ABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )
    ##
    return  _decorated 
# =============================================================================

# =============================================================================
## perform the ctual decoration
_decorated = _decorate()                                     ## ATTENTION
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================

