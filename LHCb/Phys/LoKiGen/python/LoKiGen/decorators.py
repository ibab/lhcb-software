#!/usr/bin/env python
# =============================================================================
## @file decorators.py LoKiCore/decorators.py
#  The set of basic decorator for objects from LoKiGen library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiGen library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiGen.functions   import *

_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """ Make the decoration of all objects fomr this module """
    import LoKiCore.decorators as _LoKiCore 
    _decorated  = _LoKiCore.getAndDecorateFunctions (
        name                                            , ## module name 
        LoKi.Function('const HepMC::GenParticle*')      , ## the base 
        LoKi.Dicts.FunCalls(HepMC.GenParticle)          , ## call-traits 
        LoKi.Dicts.FuncOps('const HepMC::GenParticle*') ) ## operators 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                            , ## module name 
        LoKi.Function('const HepMC::GenVertex*')        , ## the base 
        LoKi.Dicts.FunCalls(HepMC.GenVertex)            , ## call-traits 
        LoKi.Dicts.FuncOps('const HepMC::GenVertex*')   ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        LoKi.Predicate('const HepMC::GenParticle*')     , ## the base 
        LoKi.Dicts.CutCalls(HepMC.GenParticle)          , ## call-traits 
        LoKi.Dicts.FuncOps('const HepMC::GenParticle*') ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                            , ## module name 
        LoKi.Predicate('const HepMC::GenVertex*')       , ## the base 
        LoKi.Dicts.FunCalls(HepMC.GenVertex)            , ## call-traits 
        LoKi.Dicts.FuncOps('const HepMC::GenVertex*')   ) ## the operators
    # decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( GID , GABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add( t ) 
    ## 
    return _decorated                            ## RETURN

# =============================================================================
## perform the decoration 
_decorated = _decorate ()                         ## ATTENTION 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
        
# =============================================================================
# The END
# =============================================================================
