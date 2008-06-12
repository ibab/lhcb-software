#!/usr/bin/env python
# =============================================================================
# $Id: decorators.py,v 1.11 2008-06-12 08:28:54 ibelyaev Exp $ 
# =============================================================================
## @file decorators.py LoKiAlgo/decorators.py
#  The set of basic decorator for objects from LoKiAlgo library
#  The file is a part of LoKi and Bender projects
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
The set of basic decorators for objects from LoKiAlgo library

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

import LoKiPhys.decorators   as     _LoKiPhys
#import LoKiPhysMC.decorators as     _LoKiPhysMC 
from   LoKiAlgo.functions    import * 


# =============================================================================
##get the effective formula of the loop, @see LoKi::LoopObj::formula
def _formula_  ( s )      :
    """
    Get the effective 'formula' of the loop-object:

    >>> l= self.loop(...)
    >>> l.formula()

    """
    return LoKi.Dicts.DictsLoop.formula  ( s )
# =============================================================================
## get the effective dimension of the loop (== number of components)
def _dim_      ( s ) :
    """
    Get the effective dimension of the loop (== number of components)

    >>> l.dim() 
    
    """
    return LoKi.Dicts.DictsLoop.dim  ( s )
# =============================================================================
## get the estimate of number of combinations 
def _size_     ( s )      :
    """
    Get the estimate of number of combinations

    >>> l.dim() 
    
    """    
    return LoKi.Dicts.DictsLoop.size  ( s )
# =============================================================================
## get the status of loop object
def _status_   ( s )      :
    """
    Get the status of the loop object

    >>> l.status() 
    
    """    
    return LoKi.Dicts.DictsLoop.status  ( s )
# =============================================================================
##  advance the loop to the next combination
def _next_     ( s )      :
    """
    Advance the loop to the next combination
    
    >>> l.next() 
    
    """    
    return LoKi.Dicts.DictsLoop.next  ( s )
# =============================================================================
## conversion operator to LHCb::Vertex and access to the children:
def _vertex_ ( s ) :
    """
    Conversion operator to LHCb::Vertex and access to the children:
    
    >>> l.vertex() 
    
    """
    return LoKi.Dicts.DictsLoop.vertex ( s )
# =============================================================================
## conversion operator to LHCb::Particle and access to the children:
def _particle_ ( s , *a ) :
    """
    Conversion operator to LHCb::Particle and access to the children:
    
    >>> l.particle() 
    >>> l.particle(3)
    >>> l(2)
    >>> l()
    
    """
    return LoKi.Dicts.DictsLoop.particle ( s , *a )
# =============================================================================
## get the momentum of the particle or daughter or combination
def _momentum_ ( s , *a ) :
    """
    Get the momentum of the particle or daughter or combination
    
    >>> l.momentum() 
    >>> l.momentum(3) 
    >>> l.momentum(1,2,3) 
    
    """
    return LoKi.Dicts.DictsLoop.momentum ( s , *a ) 
# =============================================================================
## get the invarinat mass of the particle or daughter or combination
def _mass_ ( s , *a ) :
    """
    Get the momentum of the particle or daughter or combination
    
    >>> l.mass() 
    >>> l.mass(2) 
    >>> l.mass(1,4) 
    >>> l.m() 
    >>> l.m(2) 
    >>> l.m(1,4,2) 
    
    """
    return LoKi.Dicts.DictsLoop.mass ( s , *a ) 
# =============================================================================
## get the effective PID
def _pid_      ( s ) :
    """
    Get the effective PID, associated with Loop objects
    
    >>> l.pid() 
    
    """
    return LoKi.Dicts.DictsLoop.pid ( s ) 
# =============================================================================
## get the effective PID
def _pp_      ( s ) :
    """
    Get the effective PID, associated with Loop objects
    
    >>> l.pp() 
    
    """
    return LoKi.Dicts.DictsLoop.pp ( s ) 
# =============================================================================
## get the effective PID
def _pidName_  ( s ) :
    """
    Get the effective PID, associated with Loop objects
    
    >>> l.pidName() 
    
    """
    return LoKi.Dicts.DictsLoop.pidName ( s ) 
# =============================================================================
## set the particle ID for the effective particle of the loop
def _setPID_   ( s , pid ) :
    """
    Set the particle ID for the effective particle of the loop

    >>> l.setPID ('B0')
    
    """
    return LoKi.Dicts.DictsLoop.setPID ( s , pid )
# =============================================================================
## set the default IParticleCombiner tool
def _setCom_   ( s , *a ) :
    """
    Set the default IParticleCombiner tool

    >>> l.setCombiner ( combiner )
    
    """
    return LoKi.Dicts.DictsLoop.setCombiner ( s , pid )
# =============================================================================
## set the default IParticleReFitter tool
def _setFit_   ( s , *a ) :
    """
    Set the default IParticleReFitter tool
    
    >>> l.setCombiner ( combiner )
    
    """
    return LoKi.Dicts.DictsLoop.setReFitter ( s , pid )
# =============================================================================
## get the associated vertex
def _pv_       ( s )      :
    """
    Get the associated vertex
    
    >>> primVertex = l.pv() 
    
    """
    return LoKi.Dicts.DictsLoop.pv       ( s )  
# =============================================================================
## set the associated vertex
def _setPV_    ( s , *a ) :
    """
    Set the associated vertex
    
    >>> l.setPV( primVertex )
    
    """
    return LoKi.Dicts.DictsLoop.setPV    ( s , *a )
# =============================================================================
## make 'effective' particle using IParticleCombiner tool
def _make_     ( s , *a ) :
    """
    Make 'effective' particle using IParticleCombiner tool
    
    >>> l.make( combiner )
    >>> l.make()
    
    """
    return LoKi.Dicts.DictsLoop.make     ( s , *a )
# =============================================================================
## refit the particle using IParticleReFitter tool
def _reFit_    ( s , *a ) :
    """
    Refit the particle using IParticleReFitter tool
    
    >>> l.reFit ( reFitter )
    >>> l.reFit ()
    
    """
    return LoKi.Dicts.DictsLoop.reFit    ( s , *a ) 
# =============================================================================
## save the particle into LoKi storage
def _save_     ( s ,name  ) :
    """
    Save the particle into LoKi storage
    
    >>> l.save ('good')
    
    """    
    return LoKi.Dicts.DictsLoop.save     ( s , name ) 
# =============================================================================
## backup the current state of the loop
def _backup_   ( s )      :
    """
    Backup the current state of the loop
    
    >>> l.backup()
    
    """
    return LoKi.Dicts.DictsLoop.backup   ( s )
# =============================================================================
## restore the loop from the last backup state
def _restore_  ( s )      :
    """
    Restore the loop state from the last backup state
    
    >>> l.restore()
    
    """
    return LoKi.Dicts.DictsLoop.restore  ( s )      
# =============================================================================
## define the iteration over the loop content
def _iter_ ( s ) :
    """
    Iteration over the loop content:
    
    >>> l = self.loop(...)
    >>> for B in l :
    
    """
    loop = s
    while loop.valid() :
        yield loop
        loop.next()
# =============================================================================

        

# =============================================================================
## update the docuemntation
_formula_   . __doc__  +=  LoKi.Dicts.DictsLoop.formula     . __doc__ 
_dim_       . __doc__  +=  LoKi.Dicts.DictsLoop.dim         . __doc__ 
_size_      . __doc__  +=  LoKi.Dicts.DictsLoop.size        . __doc__ 
_status_    . __doc__  +=  LoKi.Dicts.DictsLoop.status      . __doc__ 
_next_      . __doc__  +=  LoKi.Dicts.DictsLoop.next        . __doc__ 
_vertex_    . __doc__  +=  LoKi.Dicts.DictsLoop.vertex      . __doc__ 
_particle_  . __doc__  +=  LoKi.Dicts.DictsLoop.particle    . __doc__ 
_momentum_  . __doc__  +=  LoKi.Dicts.DictsLoop.momentum    . __doc__ 
_mass_      . __doc__  +=  LoKi.Dicts.DictsLoop.mass        . __doc__ 
_pid_       . __doc__  +=  LoKi.Dicts.DictsLoop.pid         . __doc__ 
_pp_        . __doc__  +=  LoKi.Dicts.DictsLoop.pp          . __doc__ 
_pidName_   . __doc__  +=  LoKi.Dicts.DictsLoop.pidName     . __doc__ 
_setPID_    . __doc__  +=  LoKi.Dicts.DictsLoop.setPID      . __doc__ 
_setFit_    . __doc__  +=  LoKi.Dicts.DictsLoop.setReFitter . __doc__ 
_pv_        . __doc__  +=  LoKi.Dicts.DictsLoop.pv          . __doc__ 
_setPV_     . __doc__  +=  LoKi.Dicts.DictsLoop.setPV       . __doc__ 
_make_      . __doc__  +=  LoKi.Dicts.DictsLoop.make        . __doc__ 
_reFit_     . __doc__  +=  LoKi.Dicts.DictsLoop.reFit       . __doc__ 
_save_      . __doc__  +=  LoKi.Dicts.DictsLoop.save        . __doc__ 
_backup_    . __doc__  +=  LoKi.Dicts.DictsLoop.backup      . __doc__ 
_restore_   . __doc__  +=  LoKi.Dicts.DictsLoop.restore     . __doc__ 
# =============================================================================


LoKi.Loop.formula     = _formula_
LoKi.Loop.dim         = _dim_
LoKi.Loop.size        = _size_
LoKi.Loop.status      = _status_
LoKi.Loop.next        = _next_
LoKi.Loop.vertex      = _vertex_
LoKi.Loop.particle    = _particle_
LoKi.Loop.momentum    = _momentum_
LoKi.Loop.mass        = _mass_
LoKi.Loop.pid         = _pid_
LoKi.Loop.pp          = _pp_
LoKi.Loop.pidName     = _pidName_
LoKi.Loop.setPID      = _setPID_
LoKi.Loop.setFit      = _setFit_
LoKi.Loop.pv          = _pv_
LoKi.Loop.setPV       = _setPV_
LoKi.Loop.make        = _make_
LoKi.Loop.reFit       = _reFit_
LoKi.Loop.save        = _save_
LoKi.Loop.backup      =  _backup_
LoKi.Loop.restore     = _restore_
LoKi.Loop.__iter__    =  _iter_
LoKi.Loop.__call__    = _particle_
LoKi.Loop.__invert__  = lambda s: not s.valid() 

LoKi.Loop.child       = _particle_
LoKi.Loop.daughter    = _particle_
LoKi.Loop.son         = _particle_
LoKi.Loop.m           = _mass_ 
LoKi.Loop.p           = _momentum_



# =============================================================================
## Print the decay tree for LoKi::Loop object:
def printDecay ( s , *a ) :
    """
    Print the decay tree for LoKi::Loop object:

    >>> l = ...
    >>> printDecay ( l )

    """
    return LoKi.PrintLoop.printDecay ( s , *a )

printDecay . __doc__ += "\n\n" + LoKi.PrintLoop.printDecay . __doc__


if not hasattr ( LoKi.Loop ,   "printDecay"   ) :
    LoKi.Loop.   printDecay   = printDecay
    LoKi.Loop.        decay   = printDecay
if not hasattr ( LoKi.Loop , "__printDecay__" ) :
    LoKi.Loop. __printDecay__ = printDecay
    LoKi.Loop.      __decay__ = printDecay
    
import LoKiCore.functions  as _LCF 
_LCF.printDecay    . __doc__ += "\n" + LoKi.PrintLoop.printDecay    . __doc__ 




## ============================================================================
## decorate LoKi::Algo class
def _decorateAlg ( alg ) :
    """
    Decorate LoKi::Algo class
    """
    import GaudiPython.GaudiAlgs
    from   GaudiPython import iAlgorithm 
    _Alg = LoKi.Dicts.Alg( alg )
    ## the major LoKi object - algorithm 
    class Algo( _Alg , iAlgorithm ) :
        """
        The major LoKi object - algorithm
        """
        pass
    ## setup the base 
    Algo .  Base      = _Alg
    Algo . _Base      = _Alg
    ## initialze with N-tuples 
    Algo . initialize = GaudiPython.GaudiAlgs._initialize_tuple_
    ## the default execute 
    Algo . execute    = Algo   .execute_
    ## the default analyse
    Algo . analyse    = Algo   .analyse_
    ## some accessors 
    Algo . ntupleSvc  = GaudiPython.GaudiAlgs._ntupleSvc
    Algo . tupleSvc   = GaudiPython.GaudiAlgs._ntupleSvc
    Algo . ntupSvc    = GaudiPython.GaudiAlgs._ntupleSvc
    Algo . tupSvc     = GaudiPython.GaudiAlgs._ntupleSvc
    Algo . evtColSvc  = GaudiPython.GaudiAlgs._evtcolSvc
    Algo . evtcolSvc  = GaudiPython.GaudiAlgs._evtcolSvc
    GaudiPython.GaudiAlgs._decorate_algs_    ( [ Algo ] )
    GaudiPython.GaudiAlgs._decorate_plots_   ( [ Algo ] )
    GaudiPython.GaudiAlgs._decorate_tuples_  ( [ Algo ] )
    ##
    return Algo
## ============================================================================
Algo = _decorateAlg ( 'LoKi::Algo' ) 


## decorate N-tuples
import LoKiAlgo.fArrayP

_name = _LoKiPhys.__name__

## ============================================================================
## make a decoration fo functions and predicates """
## ============================================================================
def _decorate( name = _name ) :
    """
    Make a decoration fo functions and predicates
    """
    import LoKiCore.decorators   as     _LoKiCore
    _p='const LHCb::Particle*'
    _v='const LHCb::VertexBase*'
    _decorated   = _LoKiCore.getAndDecorateFunctions (
        name                                          , ## module name 
        LoKi.Functor          (_p,'double')           , ## the base 
        LoKi.Dicts.ExtFunCalls(LHCb.Particle)         , ## call-traits 
        LoKi.Dicts.FuncOps    (_p,_p)                 ) ## operators 
    _decorated |= _LoKiCore.getAndDecorateFunctions  (
        name                                          , ## module name 
        LoKi.Functor          (_v,'double'), ## the base 
        LoKi.Dicts.ExtFunCalls(LHCb.VertexBase)       , ## call-traits 
        LoKi.Dicts.FuncOps    (_v,_v)                 ) ## operators
    _decorated |=  _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Functor          (_p,bool)               , ## the base 
        LoKi.Dicts.ExtCutCalls(LHCb.Particle)         , ## call-traits 
        LoKi.Dicts.CutsOps    (_p,_p)                 ) ## operators 
    _decorated |= _LoKiCore.getAndDecoratePredicates (
        name                                          , ## module name 
        LoKi.Functor          (_v,bool)               , ## the base 
        LoKi.Dicts.ExtCutCalls(LHCb.VertexBase)       , ## call-traits 
        LoKi.Dicts.CutsOps    (_v,_v)                 ) ## the operators
    # (re)decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( _LoKiPhys.ID , _LoKiPhys.ABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )
    ## 
    return _decorated                                   ## RETURN  

# =============================================================================
## make a decoration of the module 
_decorated  = _decorate ( _LoKiPhys   . __name__ ) 
#_decorated |= _decorate ( _LoKiPhysMC . __name__ ) 
# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s'%len(_decorated)
    for o in _decorated : print o
    
# =============================================================================
# The END 
# =============================================================================
