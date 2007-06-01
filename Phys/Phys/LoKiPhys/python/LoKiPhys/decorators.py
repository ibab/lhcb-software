#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
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
import LoKiCore.decorators as     _LoKiCore

# =============================================================================
# make the decoration of all objects from this module
# =============================================================================

_decorated  = _LoKiCore.getAndDecorateFunctions (
    __name__                                      , ## module name 
    Func                                          , ## the base 
    LoKi.Dicts.FunCalls(LHCb.Particle)            , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
_decorated |= _LoKiCore.getAndDecorateFunctions  (
    __name__                                      , ## module name 
    VFunc                                         , ## the base 
    LoKi.Dicts.FunCalls(LHCb.VertexBase)          , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::VertexBase*') ) ## operators
_decorated |=  _LoKiCore.getAndDecoratePredicates (
    __name__                                      , ## module name 
    Cuts                                          , ## the base 
    LoKi.Dicts.CutCalls(LHCb.Particle)            , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
_decorated |= _LoKiCore.getAndDecoratePredicates (
    __name__                                      , ## module name 
    VCuts                                         , ## the base 
    LoKi.Dicts.CutCalls(LHCb.VertexBase)          , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::VertexBase*') ) ## the operators 

# decorate pids (Comparison with strings, integers and ParticleID objects:
for t in ( ID , ABSID ) :
    t = type ( t ) 
    _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================

