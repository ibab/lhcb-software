#!/usr/bin/env python
# =============================================================================
# $Id: decorators.py,v 1.14 2010-05-30 17:09:55 ibelyaev Exp $ 
# =============================================================================
## @file LoKiGen/decorators.py
#  The set of basic decorator for objects from LoKiGen library
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiGen library

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
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision: 1.14 $ "
# =============================================================================

from   LoKiGen.functions   import *
from   LoKiGen.trees       import *
import LoKiGen.HepMC

_name = __name__ 
# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """ Make the decoration of all objects fomr this module """
    import LoKiCore.decorators as _LoKiCore
    _gp  = 'const HepMC::GenParticle*'
    _gv  = 'const HepMC::GenVertex*'
    
    ## _vgp = std.vector ( _gp      )
    ## _vgv = std.vector ( _gv      )
    ## _vd  = std.vector ( 'double' )

    _vgp = 'std::vector<const HepMC::GenParticle*>'
    _vgv = 'std::vector<const HepMC::GenVertex*>'
    _vd  = 'std::vector<double>'

    # T -> double
    
    _decorated = _LoKiCore.getAndDecorateFunctions ( 
        name                                    , ## moduel name  
        LoKi.Functor   (_gp,'double')           , ## the base
        LoKi.Dicts.FunCalls (HepMC.GenParticle) , ## call-traits
        LoKi.Dicts.FuncOps  (_gp,_gp)           ) ## operators&operations
    
    _decorated |= _LoKiCore.getAndDecorateFunctions ( 
        name                                    , ## module name  
        LoKi.Functor   (_gv,'double')           , ## the base
        LoKi.Dicts.FunCalls (HepMC.GenVertex)   , ## call-traits
        LoKi.Dicts.FuncOps  (_gv,_gv)           ) ## operators&operations
    
    # T -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates ( 
        name                                    , ## module name  
        LoKi.Functor   (_gp,bool)               , ## the base
        LoKi.Dicts.CutCalls (HepMC.GenParticle) , ## call-traits
        LoKi.Dicts.CutsOps  (_gp,_gp)           ) ## operators&operations
    
    _decorated |= _LoKiCore.getAndDecoratePredicates ( 
        name                                    , ## module name  
        LoKi.Functor   (_gv,bool)               , ## the base
        LoKi.Dicts.CutCalls (HepMC.GenVertex)   , ## call-traits
        LoKi.Dicts.CutsOps  (_gv,_gv)           ) ## operators&operations

    # functional part

    # map: vector<T> -> vector<double>

    _decorated |= _LoKiCore.getAndDecorateMaps (
        name                                   , ## moduel name  
        LoKi.Functor   (_vgp,_vd)              , ## the base
        LoKi.Dicts.MapsOps(_gp)                ) ## call-traits    
    _decorated |= _LoKiCore.getAndDecorateMaps (
        name                                   , ## moduel name  
        LoKi.Functor   (_vgv,_vd)              , ## the base
        LoKi.Dicts.MapsOps(_gv)                ) ## call-traits

    # pipe: vector<T> -> vector<T>

    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                   , ## module name  
        LoKi.Functor   (_vgp,_vgp)             , ## the base
        LoKi.Dicts.PipeOps(_gp,_gp)            ) ## call-traits    
    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                   , ## module name  
        LoKi.Functor   (_vgv,_vgv)             , ## the base
        LoKi.Dicts.PipeOps(_gv,_gv)            ) ## call-traits
    
    # vector<T> -> double
    
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vgp,'double')         , ## the base
        LoKi.Dicts.FunValOps(_gp)              ) ## call-traits
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vgv,'double')         , ## the base
        LoKi.Dicts.FunValOps(_gv)              ) ## call-traits

    # vector<T> -> bool

    _decorated |= _LoKiCore.getAndDecorateCutVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vgp,bool)             , ## the base
        LoKi.Dicts.CutValOps(_gp)              ) ## call-traits

    _decorated |= _LoKiCore.getAndDecorateCutVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vgv,bool)             , ## the base
        LoKi.Dicts.CutValOps(_gv)              ) ## call-traits

    #sources : void -> vector<T>

    _decorated |= _LoKiCore.getAndDecorateSources ( 
        name                                   , ## module name  
        LoKi.Functor   ('void',_vgp)           , ## the base
        LoKi.Dicts.SourceOps(_gp,_gp)          ) ## call-traits
    
    _decorated |= _LoKiCore.getAndDecorateSources ( 
        name                                   , ## module name  
        LoKi.Functor   ('void',_vgv)           , ## the base
        LoKi.Dicts.SourceOps(_gv,_gv)          ) ## call-traits
    
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
