#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiProtoParticles/decorators.py
#  The set of basic decorator for objects from Phys/LoKiProtoParticles library
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
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#    ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#    
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date  2010-05-26
#
#                  $Revision$
# LastModification $Date$
#               by $Author$ 
# =============================================================================
"""
The set of basic decorators for obejcts from Phys/LoKiProtoParticles library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                 $Revision$
LastModification $Date$
              by $Author$ 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2010-05-26"
__version__ = "Version $Revision$"
# =============================================================================

from   LoKiProtoParticles.functions import *

_name = __name__ 
# =============================================================================
## Make the decoration of all objects from this module
def _decorate ( name = _name ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    _pp  = 'const LHCb::ProtoParticle*'
    _ppv = 'std::vector<const LHCb::ProtoParticle*>' ## std.vector( _mcp )
    _vd  = 'std::vector<double>'                     ## std.vector( 'double' )
    #
    
    # ProtoParticle -> double
    
    _decorated = _LoKiCore.getAndDecorateFunctions ( 
        name                                       , ## module name  
        LoKi.Functor  ( _pp , 'double')              , ## the base
        LoKi.Dicts.FunCalls ( LHCb.ProtoParticle)  , ## call-traits
        LoKi.Dicts.FuncOps  ( _pp , _pp )          ) ## operators&operations

    # ProtoParticle -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates  (
        name                                        , ## module name  
        LoKi.Functor   ( _pp , bool )               , ## the base
        LoKi.Dicts.CutCalls ( LHCb.ProtoParticle )  , ## call-traits
        LoKi.Dicts.CutsOps  ( _pp , _pp )           ) ## operators&operations
    
    ## functional part:
    
    # vector<T> -> vector<double>
    
    _decorated |= _LoKiCore.getAndDecorateMaps (
        name                                     , ## module name  
        LoKi.Functor       ( _ppv , _vd )        , ## the base
        LoKi.Dicts.MapsOps ( _pp )               ) ## call-traits
    
    # vector<T> -> vector<T>
    
    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                    , ## module name  
        LoKi.Functor       ( _ppv , _ppv )      , ## the base
        LoKi.Dicts.PipeOps ( _pp  , _pp  )      ) ## call-traits
    
    # vector<T> -> double
    
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                     , ## module name  
        LoKi.Functor         ( _ppv , 'double' ) , ## the base
        LoKi.Dicts.FunValOps ( _pp )             ) ## call-traits
    
    # vector<T> -> bool
    
    _decorated |= _LoKiCore.getAndDecorateCutVals ( 
        name                                    , ## module name  
        LoKi.Functor         ( _ppv , bool )    , ## the base
        LoKi.Dicts.CutValOps ( _pp )            ) ## call-traits
    
    # vector<T> -> T
    
    _decorated |= _LoKiCore.getAndDecorateElements (  
        name                                    , ## module name  
        LoKi.Functor          ( _ppv , _pp )    , ## the base
        LoKi.Dicts.ElementOps ( _pp  , _pp )    ) ## call-traits
    
    #sources : void -> vector<T>
    
    _decorated |= _LoKiCore.getAndDecorateSources  (  
        name                                   , ## module name  
        LoKi.Functor         ('void', _ppv )   , ## the base
        LoKi.Dicts.SourceOps ( _pp  , _pp  )   ) ## call-traits

    # 'infos'
    
    _decorated |= _LoKiCore.getAndDecorateInfos    (
        name                                   , ## module name
        LoKi.Functor       ( _pp ,'double')    , ## the base 
        LoKi.Dicts.InfoOps ( _pp )             ) ## methods


    # primitive voids :
    
    _decorated |= _LoKiCore.getAndDecoratePrimitiveVoids ( name )
    return _decorated

# =============================================================================
## Perform the actual decoration:
_decorated = _decorate() 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author     : ' , __author__
    print ' Version    : ' , __version__
    print ' Date       : ' , __date__
    print ' #decorated : ' , len(_decorated)
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
