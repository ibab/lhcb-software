#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic objects from LoKiTracklibrary
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
""" The set of basic objects from LoKiTrack library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 
from   LoKiCore.functions import equal_to 

## needed since there is no autoloading of HepMC dictionaries:

# Namespaces:
_global  = _LoKiCore._global 
std      = _global.std
LoKi     = _global.LoKi
LHCb     = _global.LHCb

_T = 'LHCb::Track'
_V = 'LHCb::RecVertex'

# =============================================================================
## "Ordinary" functions for Tracks: "Tr"
# =============================================================================

## @see LoKi::Types::TrFunc
TrFunc  = LoKi.Function               ( _T ) 
## @see LoKi::Types::TrCuts
TrCuts  = LoKi.Predicate              ( _T ) 
## @see LoKi::Types::TrFun
TrFun   = LoKi.FunctionFromFunction   ( _T ) 
## @see LoKi::Types::TrCut
TrCut   = LoKi.PredicateFromPredicate ( _T )

# =============================================================================
## "Ordinary" functions for Vertices: "RV"
# =============================================================================

## @see LoKi::Types::RVFunc
RVFunc = LoKi.Function               ( _V ) 
## @see LoKi::Types::RVCuts
RVCuts = LoKi.Predicate              ( _V ) 
## @see LoKi::Types::RVFun
RVFun  = LoKi.FunctionFromFunction   ( _V ) 
## @see LoKi::Types::RVCut
RVCut  = LoKi.PredicateFromPredicate ( _V ) 

_HTT = LoKi.Holder(_T,_T)
_HTV = LoKi.Holder(_T,_V)
_HVV = LoKi.Holder(_V,_V)

# =============================================================================
## Functors which deals with 2 tracks:   "TTr"
# =============================================================================

## @see LoKi::Types::TTrFunc
TTrFunc  = LoKi.Function               ( _HTT ) 
## @see LoKi::Types::TTrCuts
TTrCuts  = LoKi.Predicate              ( _HTT ) 
## @see LoKi::Types::TTrFun
TTrFun   = LoKi.FunctionFromFunction   ( _HTT ) 
## @see LoKi::Types::TTrCut
TTrCut   = LoKi.PredicateFromPredicate ( _HTT )

# =============================================================================
## Functors which deals with track and vertex: "TrV"
# =============================================================================

## @see LoKi::Types::TTrVFunc
TrVFunc  = LoKi.Function               ( _HTV ) 
## @see LoKi::Types::TTrVCuts
TrVCuts  = LoKi.Predicate              ( _HTV ) 
## @see LoKi::Types::TTrVFun
TrVFun   = LoKi.FunctionFromFunction   ( _HTV ) 
## @see LoKi::Types::TTrVCut
TrVCut   = LoKi.PredicateFromPredicate ( _HTV )

# =============================================================================
## Functors which deals with two vertices: "RVV"
# =============================================================================

## @see LoKi::Types::TTrVFunc
RVVFunc  = LoKi.Function               ( _HVV ) 
## @see LoKi::Types::TTrVCuts
RVVCuts  = LoKi.Predicate              ( _HVV ) 
## @see LoKi::Types::TTrVFun
RVVFun   = LoKi.FunctionFromFunction   ( _HVV ) 
## @see LoKi::Types::TTrVCut
RVVCut   = LoKi.PredicateFromPredicate ( _HVV )


# =============================================================================
## concrete functions:
# =============================================================================

# =============================================================================

## @see LoKi::Cuts::RVHASINFO
RVHASINFO     = LoKi.RecVertices.HasInfo  
## @see LoKi::Cuts::RVINFO
RVINFO        = LoKi.RecVertices.Info  
## @see LoKi::Cuts::RVMAXPT
RVMAXPT       = LoKi.RecVertices.MaxPt () 
## @see LoKi::Cuts::RVMAXPT
RVMINPT       = LoKi.RecVertices.MinPt () 
## @see LoKi::Cuts::RVSINFO
RVSINFO       = LoKi.RecVertices.SmartInfo

# =============================================================================

## @see LoKi::Cuts::RVVDZ
RVVDZ         = LoKi.RecVertices.DeltaZ () 
## @see LoKi::Cuts::RVVFC
RVVFC         = LoKi.RecVertices.FC     () 
## @see LoKi::Cuts::RVVFC2
RVVFC2        = LoKi.RecVertices.FC2    () 

# =============================================================================

## @see LoKi::Cuts::TrTYPE
TrTYPE        = LoKi.Tracks.Type  () 


## @see @see LoKi::Cuts::TrALL
TrALL         = LoKi.BooleanConstant ( _T ) ( True )
## @see LoKi::Cuts::TrBACKWARD
TrBACKWARD    = LoKi.Tracks.CheckFlag ( LHCb.Track.Backward ) 
## @see LoKi::Cuts::TrCHI2 
TrCHI2        = LoKi.Tracks.Chi2 () 
## @see LoKi::Cuts::TrCLONE
TrCLONE       = LoKi.Tracks.CheckFlag ( LHCb.Track.Clone ) 
## @see LoKi::Cuts::TrDOWNSTREAM
TrDOWNSTREAM  = equal_to ( TrTYPE , LHCb.Track.Downstream )
## @see LoKi::Cuts::TrDE
TrDE          = LoKi.Tracks.DeltaE     () 
## @see LoKi::Cuts::TrFALSE
TrFALSE       = LoKi.BooleanConstant( _T ) ( False )
## @see LoKi::Cuts::TrFILTER
TrFILTER      = LoKi.Tracks.Selector 
## @see LoKi::Cuts::TrHASINFO
TrHASINFO     = LoKi.Tracks.HasInfo
## @see LoKi::Cuts::TrHASSTATE
TrHASSTATE    = LoKi.Tracks.HasStateAt
## @see LoKi::Cuts::TrINTES
TrINTES       = LoKi.Tracks.InTES  () 
## @see LoKi::Cuts::TrINFO
TrINFO        = LoKi.Tracks.Info
## @see LoKi::Cuts::TrINVALID
TrINVALID     = LoKi.Tracks.CheckFlag ( LHCb.Track.Invalid )
## @see LoKi::Cuts::TrIPSELECTED
TrIPSELECTED  = LoKi.Tracks.CheckFlag ( LHCb.Track.IPSelected  )
## @see LoKi::Cuts::TrISFLAG
TrISFLAG      = LoKi.Tracks.CheckFlag
## @see LoKi::Cuts::TrKEY
TrKEY         = LoKi.Tracks.Key    () 
## @see LoKi::Cuts::TrL0CANDIDATE
TrL0CANDIDATE = LoKi.Tracks.CheckFlag ( LHCb.Track.L0Candidate ) 
## @see LoKi::Cuts::TrLONG
TrLONG        = equal_to ( TrTYPE , LHCb.Track.Long ) 
## @see LoKi::Cuts::TrMUON
TrMUON        = equal_to ( TrTYPE , LHCb.Track.Muon ) 
## @see LoKi::Cuts::TrNONE
TrNONE        = LoKi.BooleanConstant( _T ) ( False )
## @see LoKi::Cuts::TrONE
TrONE         = LoKi.Constant( _T ) ( 1.0 )
## @see LoKi::Cuts::TrP
TrP           = LoKi.Tracks.Momentum          () 
## @see LoKi::Cuts::TrPIDSELECTED
TrPIDSELECTED = LoKi.Tracks.CheckFlag ( LHCb.Track.PIDSelected ) 
## @see LoKi::Cuts::TrPROBCHI2 
TrPROBCHI2    = LoKi.Tracks.ProbChi2 () 
## @see LoKi::Cuts::TrPT
TrPT          = LoKi.Tracks.TransverseMomentum () 
## @see LoKi::Cuts::TrQ
TrQ           = LoKi.Tracks.Charge ()
## @see LoKi::Cuts::TrSELECTED
TrSELECTED    = LoKi.Tracks.CheckFlag ( LHCb.Track.Selected ) 
## @see LoKi::Cuts::TrSELECTOR
TrSELECTOR   = LoKi.Tracks.Selector 
## @see LoKi::Cuts::TrSINFO
TrSINFO       = LoKi.Tracks.SmartInfo
## @see LoKi::Cuts::TrTRUE
TrTRUE        = LoKi.BooleanConstant( _T ) ( True  )
## @see LoKi::Cuts::TrTTRACK
TrTTRACK      = equal_to ( TrTYPE , LHCb.Track.Ttrack ) 
## @see LoKi::Cuts::TrUNKNOWN
TrUNKNOWN     = equal_to ( TrTYPE , LHCb.Track.TypeUnknown ) 
## @see LoKi::Cuts::TrUPSTREAM
TrUPSTREAM    = equal_to ( TrTYPE , LHCb.Track.Upstream ) 
## @see LoKi::Cuts::TrUSED
TrUSED        = LoKi.Tracks.CheckFlag ( LHCb.Track.Used )
## @see LoKi::Cuts::TrVELO
TrVELO        = equal_to ( TrTYPE , LHCb.Track.Velo ) 
## @see LoKi::Cuts::TrVELOR
TrVELOR       = equal_to ( TrTYPE , LHCb.Track.VeloR ) 
## @see LoKi::Cuts::TrZERO
TrZERO        = LoKi.Constant( _T ) ( 0.0 )

# =============================================================================

## @see LoKi::Cuts::TTrDA
TTrDA         = LoKi.Tracks.DeltaAngle () 
## @see LoKi::Cuts::TTrDOCA
TTrDOCA       = LoKi.Tracks.DistanceOfClosestApproach ()
## @see LoKi::Cuts::TTrDP
TTrDP         = LoKi.Tracks.DeltaP     ()
## @see LoKi::Cuts::TTrMATCH
TTrMATCH      = LoKi.Tracks.TrackMatch 
## @see LoKi::Cuts::TTrMATCHIDS
TTrMATCHIDS   = LoKi.Tracks.MatchIDsFraction () 

# =============================================================================

## @see LoKi::Cuts::TrVIP 
TrVIP         = LoKi.Tracks.ImpactParameter () 
## @see LoKi::Cuts::TrVRIP 
TrVRIP        = LoKi.Tracks.RadialIP        () 





# =============================================================================
## Simple way to create the 'smart' function from the regular function.
def info ( key , fun , update = False ) :
    """
    Simple way to create the 'smart' function from the regular function.
    Smart function first checks the presence of the information with
    the given key in 'extra-info' data member, and int he case of missing
    information it used the supplied function to evaluate the information.
    optionally the evaluated information is added into  'extra-info'

    >>> fun =  ...
    >>> key =  ...
    >>> smartFun = info ( key , fun , true ) 
    
    """
    return LoKi.Dicts.Info.info ( key , fun , update )

info. __doc__ += "\n\n" + LoKi.Dicts.Info.info . __doc__ 

# =============================================================================


# =============================================================================
## Simple function to 'bind' the 2-argument function 
def bindMin ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some container
    
    The function comes in two flavours:

    1) for the given argument, return the minimal value of 2D-function against all
    objects frio the reference container:

    Find the minimal distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindMin ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !


    2) for the given argument, seek for the element from the reference container
    which minimizes 2D-function and return the value of the secodn function with
    this minimal element:
    
    Evaluate of the transverse momentum of th etarck, which
    has the the minimal dictance of the closest approach to the given track:
    
    >>> tracks = ...
    >>> pt = bindMin ( TTrDOCA , TrPT , tracks )   ## create the function
    
    >>> track = ...
    >>> value = pt ( track )                ## use it !
    
    """
    return LoKi.Dicts.HltBind.bindMin ( a , b , *c  ) 


# =============================================================================
## Simple function to 'bind' the 2-argument function 
def bindAbsMin ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some container

    The function comes in two flavours:
    
    1) for the given argument, return the minimal (absolute) value
    of 2D-function against all objects for the reference container:
    
    Find the minimal (absolute) distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindAbsMin ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !

    
    2) for the given argument, seek for the element from the reference container
    which minimizes th eabsoluet value for 2D-function and return the value
    of the second function with this minimal element:
    
    Evaluate of the transverse momentum of th etarck, which
    has the the minimal dictance of the closest approach to the given track:
    
    >>> tracks = ...
    >>> pt = bindAbsMin ( TTrDOCA , TrPT , tracks )   ## create the function
    
    >>> track = ...
    >>> value = pt ( track )                ## use it !

    
    Essenntially the following expression are equivalent :

    >>> fun = binAbsMin (       f   , a , ... )
    >>> fun = bindMin   ( abs ( f ) , a , ... )

    However the first one a little bit more efficient (less virtual calls) 
    
    """
    return LoKi.Dicts.HltBind.bindAbsMin ( a , b , *c  ) 

# =============================================================================
## Simple function to 'bind' the 2-argument function 
def bindMax ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some containers
    
    The function comes in two flavours:

    1) for the given argument, return the maximal value of 2D-function against all
    objects frio the reference container:

    find the maximal distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindMax ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !


    2) for the given argument, seek for the element from the reference container
    which maximizes 2D-function and return the value of the secodn function with
    this maximal element:
    
    Evaluate of the transverse momentum of th etarck, which
    has the the maximal distance of the closest approach to the given track:
    
    >>> tracks = ...
    >>> pt = bindMax ( TTrDOCA , TrPT , tracks )   ## create the function
    
    >>> track = ...
    >>> value = pt ( track )                ## use it !
    
    """
    return LoKi.Dicts.HltBind.bindMin ( a , b , *c  ) 


# =============================================================================
## Simple function to 'bind' the 2-argument function 
def bindAbsMin ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some container

    The function comes in two flavours:

    1) for the given argument, return the maximal (absolute) value
    of 2D-function against all objects from the reference container:
    
    Find the maximal (absoluet) distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindAbsMax ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !


    2) for the given argument, seek for the element from the reference container
    which maximizes the absoluet value for 2D-function and return the value of
    the second function with this maximal element:
    
    Evaluate of the transverse momentum of the tarck, which
    has the the maximal (absoluet) distance of the closest approach to the given track:
    
    >>> tracks = ...
    >>> pt = bindAbsMax ( TTrDOCA , TrPT tracks )   ## create the function
    
    >>> track = ...
    >>> value = pt ( track )                ## use it !

    
    Essenntially the following expression are equivalent :

    >>> fun = binAbsMax (       f   , a , ... )
    >>> fun = bindMax   ( abs ( f ) , a , ... )

    However the first one a little bit more efficient (less virtual calls) 
    
    """
    return LoKi.Dicts.HltBind.bindAbsMax ( a , b , *c  ) 



bindMin    . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindMin    . __doc__ 
bindMax    . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindMax    . __doc__ 
bindAbsMin . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindAbsMin . __doc__ 
bindAbsMax . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindAbsMax . __doc__ 


if '__main__' == __name__ :
    for i in dir() : print i 

# =============================================================================
# The END 
# =============================================================================
