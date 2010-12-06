#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file 
#  The set of basic decorations for objects from LoKiTrigger library
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
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-06-09
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
The set of basic decorations for objects from LoKiTrigger library

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
 ``No Vanya's lines are allowed in LHCb/Gaudi software.''


                   $Revision$
 Last modification $Date$
                by $Author$
 """
# =============================================================================
__date__     = "2007-05-29"
__version__  = "SVN version $Revision$"
__author__   = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================
from   LoKiTrigger.functions   import *

_name = __name__

# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    
    tC = 'const Hlt::Candidate*'
    tS = 'const Hlt::Stage*' 
    vC = 'std::vector<const Hlt::Candidate*>'
    vD = 'std::vector<double>'
    #
    ## "function" : Hlt::Candidate -> double 
    
    _decorated  = _LoKiCore.getAndDecorateFunctions (  
        name                                   , ## module name  
        LoKi.Functor        ( tC ,'double'  )  , ## the base
        LoKi.Dicts.FunCalls ( Hlt.Candidate )  , ## call-traits
        LoKi.Dicts.FuncOps  ( tC , tC       )  ) ## operators&operations
    
    ## "predicate/cut" :  Hlt::Candidate -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates ( 
        name                                    , ## module name  
        LoKi.Functor        ( tC , bool     )   , ## the base
        LoKi.Dicts.CutCalls ( Hlt.Candidate )   , ## call-traits
        LoKi.Dicts.CutsOps  ( tC , tC       )   ) ## operators&operations

    ## "function" : Hlt::Stage -> double 
    
    _decorated  = _LoKiCore.getAndDecorateFunctions (  
        name                                   , ## module name  
        LoKi.Functor        ( tS ,'double'  )  , ## the base
        LoKi.Dicts.FunCalls ( Hlt.Stage     )  , ## call-traits
        LoKi.Dicts.FuncOps  ( tS , tS       )  ) ## operators&operations
    
    ## "predicate/cut" :  Hlt::Stage -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates ( 
        name                                    , ## module name  
        LoKi.Functor        ( tS , bool     )   , ## the base
        LoKi.Dicts.CutCalls ( Hlt.Stage     )   , ## call-traits
        LoKi.Dicts.CutsOps  ( tS , tS       )   ) ## operators&operations

    ## functional part:
    
    # "map" : vector<T> -> vector<double>
    
    _decorated |= _LoKiCore.getAndDecorateMaps (
        name                                   , ## module name  
        LoKi.Functor       ( vC , vD )         , ## the base
        LoKi.Dicts.MapsOps ( tC      )         ) ## call-traits
    
    # "pipe" : vector<T> -> vector<T>
    
    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                   , ## module name  
        LoKi.Functor       ( vC , vC )         , ## the base
        LoKi.Dicts.PipeOps ( tC , tC )         ) ## call-traits
    
     # "funval" : vector<T> -> double 
    
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor         ( vC , 'double' ) , ## the base
        LoKi.Dicts.FunValOps ( tC )            ) ## call-traits

    # "cutval" : vector<T> -> bool

    _decorated |= _LoKiCore.getAndDecorateCutVals (
        name                                   , ## module name  
        LoKi.Functor         ( vC , bool )     , ## the base
        LoKi.Dicts.CutValOps ( tC        )     ) ## call-traits


    # "element": vector<T> -> T
    
    _decorated |= _LoKiCore.getAndDecorateElements (  
        name                                   , ## module name  
        LoKi.Functor          ( vC , tC )      , ## the base
        LoKi.Dicts.ElementOps ( tC , tC )      ) ## call-traits

    # 'source' : void -> vector<T>
    
    _decorated |= _LoKiCore.getAndDecorateSources  (  
        name                                   , ## module name  
        LoKi.Functor         ( 'void' , vC )   , ## the base
        LoKi.Dicts.SourceOps ( tC     , tC )   ) ## call-traits

    
    ## primitive voids 
    _decorated |= _LoKiCore.getAndDecoratePrimitiveVoids ( name )


    # =========================================================================
    ## LHCb::Track remnants ATTENTION !!! 
    # =========================================================================
    
    tT = 'const LHCb::Track*'
    vT = 'std::vector<const LHCb::Track*>'
    
    # function 
    _decorated |= _LoKiCore.getAndDecorateFunctions (  
        name                                   , ## module name  
        LoKi.Functor        ( tT ,'double'  )  , ## the base
        LoKi.Dicts.FunCalls ( LHCb.Track    )  , ## call-traits
        LoKi.Dicts.FuncOps  ( tT , tT       )  ) ## operators&operations
    
    # "pipe" : vector<T> -> vector<T>    
    _decorated |= _LoKiCore.getAndDecoratePipes (
        name                                   , ## module name  
        LoKi.Functor       ( vT , vT )         , ## the base
        LoKi.Dicts.PipeOps ( tT , tT )         ) ## call-traits
    

    # 'source' : void -> vector<T>    
    _decorated |= _LoKiCore.getAndDecorateSources  (  
        name                                   , ## module name  
        LoKi.Functor         ( 'void' , vT )   , ## the base
        LoKi.Dicts.SourceOps ( tT     , tT )   ) ## call-traits


    ## various "info" operations
    
    ## decorate HltCandidates:
    _decorated |= _LoKiCore.getAndDecorateInfos (
        name                                   ,
        LoKi.Functor        ( tC ,'double'  )  , ## the base
        LoKi.Dicts.InfoOps  ( tC               ) ## info-opeartion
        )
    ## decorate HltCandidates:
    _decorated |= _LoKiCore.getAndDecorateInfos (
        name                                   ,
        LoKi.Functor        ( tC , bool     )  , ## the base
        LoKi.Dicts.InfoOps  ( tC               ) ## info-opeartion
        )
    ## decorate HltStages:
    _decorated |= _LoKiCore.getAndDecorateInfos (
        name                                   , 
        LoKi.Functor        ( tS ,'double'     )  , ## the base
        LoKi.Dicts.InfoOps  ( tS               ) ## info-opeartion
        )
    ## decorate HltStages:
    _decorated |= _LoKiCore.getAndDecorateInfos (
        name                                   , 
        LoKi.Functor        ( tS , bool        )  , ## the base
        LoKi.Dicts.InfoOps  ( tS               ) ## info-opeartion
        )
    
    ## 
    return _decorated                            ## RETURN

# =============================================================================
## perform the decoration 
_decorated = _decorate ()                         ## ATTENTION 
# =============================================================================

from LoKiTracks.decorators import *


# =============================================================================
## Create the function/tool from the descriptor
def __tr_func__ ( self ) :
    """
    Create the function/tool from the descriptor
    
    >>> fun  = ..                  ## get the function configuration
    >>> fltr = ( fun() < 1 * GeV ) ## create the streamer
    
    """
    return TrFUN ( self )

LoKi.Hlt1.TrackFunction  . __call__  =  __tr_func__


# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                               __doc__
    print ' Author    : %s '        %   __author__    
    print ' Version   : %s '        %   __version__
    print ' Date      : %s '        %   __date__
    print ' Decorated : %s symbols' %   len ( _decorated )
    for t in _decorated  : print t 
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================

