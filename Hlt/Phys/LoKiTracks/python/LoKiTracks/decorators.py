#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic objects from LoKiTracks library
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
# =============================================================================
"""
The set of basic objects from LoKiTracks library

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
 ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 

"""
# =============================================================================
__author__  = "Vanya BELYAEV  Ivan.Belyaev@nikhef.nl "
__date__    = "2010-07-17"
__version__ = "CVS tag $Name:$, version $Revision:$ "
# =============================================================================

from   LoKiTracks.functions import *
from   LoKiCore.functions   import equal_to

_name = __name__


# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    
    ## regular functors which accept the track
    _t  =  LHCb.Track
    _pt = 'const LHCb::Track*'
    
    # "function" : Track -> double 
    
    _decorated  = _LoKiCore.getAndDecorateFunctions ( 
        name                                   , ## module name  
        TrFunc                                 , ## the base
        LoKi.Dicts.FunCalls (_t)               , ## call-traits
        LoKi.Dicts.FuncOps  (_pt,_pt)          ) ## operators&operations
    
    # "predicate/cut" :  Track -> bool
    
    _decorated |= _LoKiCore.getAndDecoratePredicates ( 
        name                                   , ## module name  
        TrCuts                                 , ## the base
        LoKi.Dicts.CutCalls (_t)               , ## call-traits
        LoKi.Dicts.CutsOps  (_pt,_pt)          ) ## operators&operations

    ## functional stuff
    _vt = 'std::vector<const LHCb::Track*>'      ## std.vector ( )    
    _vd = 'std::vector<double>'                  ## std.vector ( 'double' )
    
    # "map" :  vector<T> -> vector<double>
    _decorated |= _LoKiCore.getAndDecorateMaps ( 
        name                                   , ## module name  
        LoKi.Functor (_vt,_vd)                 , ## the base
        LoKi.Dicts.MapsOps(_pt)                ) ## call-traits
    
    # "pipe" : vector<T> -> vector<T>    
    _decorated |= _LoKiCore.getAndDecoratePipes ( 
        name                                   , ## module name  
        LoKi.Functor   (_vt,_vt)               , ## the base
        LoKi.Dicts.PipeOps(_pt,_pt)            ) ## call-traits
  
    # "funval" : vector<T> -> double    
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vt,'double')          , ## the base
        LoKi.Dicts.FunValOps(_pt)              ) ## call-traits

    # "cutval" : vector<T> -> bool    
    _decorated |= _LoKiCore.getAndDecorateFunVals ( 
        name                                   , ## module name  
        LoKi.Functor   (_vt,bool)              , ## the base
        LoKi.Dicts.CutValOps(_pt)              ) ## call-traits

     # "element": vector<T> -> T    
    _decorated |= _LoKiCore.getAndDecorateElements ( 
        name                                   , ## module name  
        LoKi.Functor   (_vt,_pt)               , ## the base
        LoKi.Dicts.ElementOps(_pt,_pt)         ) ## call-traits
    
    # "source" : void -> vector<T>    
    _decorated |= _LoKiCore.getAndDecorateSources ( 
        name                                   , ## module name  
        LoKi.Functor   ('void',_vt)            , ## the base
        LoKi.Dicts.SourceOps(_pt,_pt)          ) ## call-traits

    ## smart info:    
    _decorated |= _LoKiCore.getAndDecorateInfos      (
        name                                            , ## module name
        TrFunc                                          , ## the base 
        LoKi.Dicts.InfoOps (_pt)                        ) ## methods
    ## 
    return _decorated                            ## RETURN



# =============================================================================
## perform the decoration 
_decorated = _decorate ()                         ## ATTENTION 
# =============================================================================

## @see LoKi::Cuts::TrDOWNSTREAM
TrDOWNSTREAM  = equal_to ( TrTYPE , LHCb.Track.Downstream  )
## @see LoKi::Cuts::TrLONG
TrLONG        = equal_to ( TrTYPE , LHCb.Track.Long        ) 
## @see LoKi::Cuts::TrMUON
TrMUON        = equal_to ( TrTYPE , LHCb.Track.Muon        ) 
## @see LoKi::Cuts::TrTTRACK
TrTTRACK      = equal_to ( TrTYPE , LHCb.Track.Ttrack      ) 
## @see LoKi::Cuts::TrUNKNOWN
TrUNKNOWN     = equal_to ( TrTYPE , LHCb.Track.TypeUnknown ) 
## @see LoKi::Cuts::TrUPSTREAM
TrUPSTREAM    = equal_to ( TrTYPE , LHCb.Track.Upstream    ) 
## @see LoKi::Cuts::TrVELO
TrVELO        = equal_to ( TrTYPE , LHCb.Track.Velo        ) 
## @see LoKi::Cuts::TrVELOR
TrVELOR       = equal_to ( TrTYPE , LHCb.Track.VeloR       ) 


# =============================================================================
if '__main__' == __name__ :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' Number of properly decorated types: %s'%len(_decorated)
    print '*'*120
    


# =============================================================================
# The END 
# =============================================================================
