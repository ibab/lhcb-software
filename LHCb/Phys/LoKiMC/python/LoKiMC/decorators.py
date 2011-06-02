#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiMC/decorators.py
#  The set of basic decorator for objects from LoKiMC library
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
The set of basic decorators for obejcts from LoKiMC library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "????-??-??"
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision$ "
# =============================================================================

from   LoKiMC.functions import *
from   LoKiMC.trees     import *   
import LoKiMC.MC

# =============================================================================
## decorate MC-finder object:
def _valid_(s)         :
    """
    Check the validity of the MC-finder object and
    the underlying pointer to IMCDecayFinder tool

    >>> finder = ...
    >>> if finder.valid() : 
    
    """
    return LoKi.Dicts.MCFinderDicts.valid      (s)
## decorate MC-finder object:
def _not_valid_(s)      :
    """
    Check the validity of the MC-finder object and
    the underlying pointer to IMCDecayFinder tool

    >>> finder = ...
    >>> if not finder : ...
    
    """
    return not s.valid()
# =============================================================================
## update the documentation 
_valid_.__doc__     += LoKi.Dicts.MCFinderDicts.valid.__doc__
# =============================================================================
## find the decays, see IMCDecayFinder::
def _findDecays_(s,decay,*a) :
    """
    Find the decay according t the decay descriptor
    [see IMCDecayFinder interface]

    >>> finder=...
    >>> decays = finder.findDecay( 'B0-> pi+ pi-')
    >>> 

    @param decay the decay descriptor
    @return the Range of found decay trees
    
    """
    return LoKi.Dicts.MCFinderDicts.findDecays ( s , decay , *a )
# =============================================================================
## update the documentation 
_findDecays_.__doc__ += LoKi.Dicts.MCFinderDicts.findDecays.__doc__
# =============================================================================
LoKi.MCFinder.find       = _findDecays_
LoKi.MCFinder.findDecay  = _findDecays_
LoKi.MCFinder.findDecays = _findDecays_
LoKi.MCFinder.valid      = _valid_ 
LoKi.MCFinder.__invert__ = _not_valid_ 


_name = __name__ 
# =============================================================================
## Make the decoration of all objects from this module
def _decorate ( name = _name ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    _mcp = 'const LHCb::MCParticle*'
    _mcv = 'const LHCb::MCVertex*'
    #_vp  = std.vector( _mcp )
    #_vv  = std.vector( _mcv )
    #_vd  = std.vector( 'double' )
    _vp  = 'std::vector<const LHCb::MCParticle*>' ## std.vector( _mcp )
    _vv  = 'std::vector<const LHCb::MCVertex*>'   ## std.vector( _mcv )
    _vd  = 'std::vector<double>'                  ## std.vector( 'double' )
    #
    
    # MCParticle -> double
    
    _decorated = _LoKiCore.getAndDecorateFunctions ( 
        name                                   , ## modulr name  
        LoKi.Functor   (_mcp,'double')         , ## the base
        LoKi.Dicts.FunCalls (LHCb.MCParticle)  , ## call-traits
        LoKi.Dicts.FuncOps  (_mcp,_mcp)        ) ## operators&operations
    
    # MCVertex -> double
    
    _decorated != _LoKiCore.getAndDecorateFunctions ( 
        name                                   , ## moduel name  
        LoKi.Functor   (_mcv,'double')         , ## the base
        LoKi.Dicts.FunCalls (LHCb.MCVertex)    , ## call-traits
        LoKi.Dicts.FuncOps  (_mcv,_mcv)        ) ## operators&operations
    
    # MCParticle -> bool
    
    _decorated != _LoKiCore.getAndDecoratePredicates  (
        name                                   , ## module name  
        LoKi.Functor   (_mcp,bool)             , ## the base
        LoKi.Dicts.CutCalls (LHCb.MCParticle)  , ## call-traits
        LoKi.Dicts.CutsOps  (_mcp,_mcp)        ) ## operators&operations
    
    # MCVertex -> bool
    
    _decorated != _LoKiCore.getAndDecoratePredicates  (
        name                                   , ## module name  
        LoKi.Functor   (_mcv,bool)             , ## the base
        LoKi.Dicts.CutCalls (LHCb.MCVertex)    , ## call-traits
        LoKi.Dicts.CutsOps  (_mcv,_mcv)        ) ## operators&operations

    ## functional part:
    
    # vector<T> -> vector<double>
    
    _decorated |= _LoKiCore.getAndDecorateMaps (
        name                                   , ## moduel name  
        LoKi.Functor   (_vp,_vd)               , ## the base
        LoKi.Dicts.MapsOps(_mcp)               ) ## call-traits
    _decorated |= _LoKiCore.getAndDecorateMaps (
        name                                   , ## moduel name  
        LoKi.Functor   (_vv,_vd)               , ## the base
        LoKi.Dicts.MapsOps(_mcv)               ) ## call-traits

    # vector<T> -> vector<T>

    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                   , ## module name  
        LoKi.Functor   (_vp,_vp)               , ## the base
        LoKi.Dicts.PipeOps(_mcp,_mcp)          ) ## call-traits
    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                   , ## module name  
        LoKi.Functor   (_vv,_vv)               , ## the base
        LoKi.Dicts.PipeOps(_mcv,_mcv)          ) ## call-traits

    # vector<T> -> double
    
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vp,'double')          , ## the base
        LoKi.Dicts.FunValOps(_mcp)             ) ## call-traits
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vv,'double')          , ## the base
        LoKi.Dicts.FunValOps(_mcv)             ) ## call-traits

    # vector<T> -> bool

    _decorated |= _LoKiCore.getAndDecorateCutVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vp,bool)              , ## the base
        LoKi.Dicts.CutValOps(_mcp)             ) ## call-traits
    _decorated |= _LoKiCore.getAndDecorateCutVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vv,bool)              , ## the base
        LoKi.Dicts.CutValOps(_mcv)             ) ## call-traits

     #sources : void -> vector<T>

    _decorated |= _LoKiCore.getAndDecorateSources  (  
        name                                   , ## module name  
        LoKi.Functor   ('void',_vp)            , ## the base
        LoKi.Dicts.SourceOps(_mcp,_mcp)        ) ## call-traits
    _decorated |= _LoKiCore.getAndDecorateSources  (  
        name                                   , ## module name  
        LoKi.Functor   ('void',_vv)            , ## the base
        LoKi.Dicts.SourceOps(_mcv,_mcv)        ) ## call-traits


    ## primitive voids:

    _decorated |= _LoKiCore.getAndDecoratePrimitiveVoids ( name ) 
    
    
    ## decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( MCID , MCABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )

        
    return _decorated
# =============================================================================


# =============================================================================
## Perform the actual decoration:
_decorated = _decorate() 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author    :  ' , __author__
    print ' Version   :  ' , __version__
    print ' Date      :  ' , __date__
    print ' Decorated : #' , len(_decorated)  
    print 80*'*'
    for i in dir() : print i 


# =============================================================================
# The END 
# =============================================================================
