#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
## @file decorators.py LoKiAlgo/decorators.py
#  The set of basic decorator for objects from LoKiAlgo library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiAlgo library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators   as     _LoKiCore 
import LoKiPhys.decorators   as     _LoKiPhys
from   LoKiAlgo.functions    import * 

_name = _LoKiPhys.__name__

_decorated   = _LoKiCore.getAndDecorateFunctions (
    _name                                         , ## module name 
    LoKi.Function('const LHCb::Particle*')        , ## the base 
    LoKi.Dicts.ExtFunCalls(LHCb.Particle)         , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
_decorated |= _LoKiCore.getAndDecorateFunctions  (
    _name                                         , ## module name 
    LoKi.Function('const LHCb::VertexBase*')      , ## the base 
    LoKi.Dicts.ExtFunCalls(LHCb.VertexBase)       , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::VertexBase*') ) ## operators
_decorated |=  _LoKiCore.getAndDecoratePredicates (
    _name                                         , ## module name 
        LoKi.Predicate('const LHCb::Particle*')       , ## the base 
    LoKi.Dicts.ExtCutCalls(LHCb.Particle)         , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::Particle*')   ) ## operators 
_decorated |= _LoKiCore.getAndDecoratePredicates (
    _name                                         , ## module name 
    LoKi.Predicate('const LHCb::VertexBase*')     , ## the base 
    LoKi.Dicts.ExtCutCalls(LHCb.VertexBase)       , ## call-traits 
    LoKi.Dicts.FuncOps('const LHCb::VertexBase*') ) ## the operators 

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================
