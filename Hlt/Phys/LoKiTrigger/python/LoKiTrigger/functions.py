#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic objects from LoKiTrigger library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
"""
The set of basic objects from LoKiTrack library
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 
from   LoKiCore.functions import equal_to 

## needed since there is no autoloading of Tracking dictionaries:
import sys,PyCintex

if sys.platform == 'win32' : PyCintex.loadDict (    "TrackEventDict" )  
else                       : PyCintex.loadDict ( "libTrackEventDict" )  
    

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
TrFunc  = LoKi.Functor                ( _T , 'double' ) 
## @see LoKi::Types::TrCuts
TrCuts  = LoKi.Functor                ( _T ,  bool    ) 
## @see LoKi::Types::TrFun
TrFun   = LoKi.FunctorFromFunctor     ( _T , 'double' ) 
## @see LoKi::Types::TrCut
TrCut   = LoKi.FunctorFromFunctor     ( _T ,  bool    )

# ## "Ordinary" functions for Vertices: "RV"
# 
## @see LoKi::Types::RVFunc
RVFunc = LoKi.Functor                ( _V , 'double' ) 
## @see LoKi::Types::RVCuts
RVCuts = LoKi.Functor                ( _V ,  bool    ) 
## @see LoKi::Types::RVFun
RVFun  = LoKi.FunctorFromFunctor     ( _V , 'double' ) 
## @see LoKi::Types::RVCut
RVCut  = LoKi.FunctorFromFunctor     ( _V ,  bool    ) 

_HTT = LoKi.Holder(_T,_T)
_HTV = LoKi.Holder(_T,_V)
_HVV = LoKi.Holder(_V,_V)

#
## Functors which deals with 2 tracks:   "TTr"
# 
## @see LoKi::Types::TTrFunc
TTrFunc  = LoKi.Functor                ( _HTT , 'double' ) 
## @see LoKi::Types::TTrCuts
TTrCuts  = LoKi.Functor                ( _HTT ,  bool    ) 
## @see LoKi::Types::TTrFun
TTrFun   = LoKi.FunctorFromFunctor     ( _HTT , 'double' ) 
## @see LoKi::Types::TTrCut
TTrCut   = LoKi.FunctorFromFunctor     ( _HTT ,  bool    )

#
## Functors which deals with track and vertex: "TrV"
#

## @see LoKi::Types::TTrVFunc
TrVFunc  = LoKi.Functor                ( _HTV , 'double' ) 
## @see LoKi::Types::TTrVCuts
TrVCuts  = LoKi.Functor                ( _HTV ,  bool    ) 
## @see LoKi::Types::TTrVFun
TrVFun   = LoKi.FunctorFromFunctor     ( _HTV , 'double' ) 
## @see LoKi::Types::TTrVCut
TrVCut   = LoKi.FunctorFromFunctor     ( _HTV ,  bool    )

#
# ## Functors which deals with two vertices: "RVV"
#

## @see LoKi::Types::TTrVFunc
RVVFunc  = LoKi.Functor                ( _HVV , 'double' ) 
## @see LoKi::Types::TTrVCuts
RVVCuts  = LoKi.Functor                ( _HVV ,  bool    ) 
## @see LoKi::Types::TTrVFun
RVVFun   = LoKi.FunctorFromFunctor     ( _HVV , 'double' ) 
## @see LoKi::Types::TTrVCut
RVVCut   = LoKi.FunctorFromFunctor     ( _HVV ,  bool    )


# =============================================================================
# ## concrete functions:
# =============================================================================

## @see LoKi::Cuts::RVHASINFO
RVHASINFO     = LoKi.RecVertices.HasInfo  
## @see LoKi::Cuts::RVINFO
RVINFO        = LoKi.RecVertices.Info  
## @see LoKi::Cuts::RVMAXPT
RVMAXPT       = LoKi.RecVertices.VertexMaxPt () 
## @see LoKi::Cuts::RVMAXPT
RVMINPT       = LoKi.RecVertices.VertexMinPt () 
## @see LoKi::Cuts::RVSINFO
RVSINFO       = LoKi.RecVertices.SmartInfo

# 
## @see LoKi::Cuts::RVVDZ
RVVDZ         = LoKi.RecVertices.DeltaZ () 
## @see LoKi::Cuts::RVVFC
RVVFC         = LoKi.RecVertices.FC     () 
## @see LoKi::Cuts::RVVFC2
RVVFC2        = LoKi.RecVertices.FC2    () 

# 
## @see LoKi::Cuts::TrTYPE
TrTYPE        = LoKi.Tracks.Type  () 


## @see @see LoKi::Cuts::TrALL
TrALL         = LoKi.Constant ( _T , bool ) ( True )
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
TrFALSE       = LoKi.Constant( _T , bool ) ( False )
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
TrNONE        = LoKi.Constant( _T , bool ) ( False )
## @see LoKi::Cuts::TrONE
TrONE         = LoKi.Constant( _T , 'double' ) ( 1.0 )
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
TrTRUE        = LoKi.Constant( _T , bool ) ( True  )
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
TrZERO        = LoKi.Constant( _T , 'double' ) ( 0.0 )

# 
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

# 
## @see LoKi::Cuts::TrVIP 
TrVIP         = LoKi.Tracks.ImpactParameter () 
## @see LoKi::Cuts::TrVRIP 
TrVRIP        = LoKi.Tracks.RadialIP        () 

# functional stuff


## functional part
_vt       = 'std::vector<LHCb::Track*>'     ## std.vector ( 'LHCb::Track*'     )
_vv       = 'std::vector<LHCb::RecVertex*>' ## std.vector ( 'LHCb::RecVertex*' )
_vd       = 'std::vector<double>'           ## std.vector ( 'double')
#
TrMaps      = LoKi.Functor             ( _vt  , _vd       )
TrMap       = LoKi.FunctorFromFunctor  ( _vt  , _vd       )
TrPipes     = LoKi.Functor             ( _vt  , _vt      )
TrPipe      = LoKi.FunctorFromFunctor  ( _vt  , _vt      )
TrFunVals   = LoKi.Functor             ( _vt  , 'double'  )
TrFunVal    = LoKi.FunctorFromFunctor  ( _vt  , 'double'  )
TrElements  = LoKi.Functor             ( _vt  , 'LHCb::Track*' ) 
TrElement   = LoKi.FunctorFromFunctor  ( _vt  , 'LHCb::Track*' )
TrSources   = LoKi.Functor             ('void', _vt  )
TrSource    = LoKi.FunctorFromFunctor  ('void', _vt  )
TrCutVals   = LoKi.Functor             ( _vt  , bool )
TrCutVal    = LoKi.FunctorFromFunctor  ( _vt  , bool )

RVMaps      = LoKi.Functor             ( _vv  , _vd       )
RVMap       = LoKi.FunctorFromFunctor  ( _vv  , _vd       )
RVPipes     = LoKi.Functor             ( _vv  , _vv       )
RVPipe      = LoKi.FunctorFromFunctor  ( _vv  , _vv       )
RVFunVals   = LoKi.Functor             ( _vv  , 'double'  )
RVFunVal    = LoKi.FunctorFromFunctor  ( _vv  , 'double'  )
RVElements  = LoKi.Functor             ( _vv  , 'LHCb::RecVertex*' ) 
RVElement   = LoKi.FunctorFromFunctor  ( _vv  , 'LHCb::RecVertex*' )
RVSources   = LoKi.Functor             ('void', _vv      )
RVSource    = LoKi.FunctorFromFunctor  ('void', _vv      )
RVFunVals   = LoKi.Functor             ( _vv  , bool )
RVFunVal    = LoKi.FunctorFromFunctor  ( _vv  , bool )


TrSOURCE       = LoKi.Tracks.SourceTES 
TrSOURCEDIRECT = LoKi.Tracks.SourceDirect 

TrEMPTY        = LoKi.Functors.Empty ('LHCb::Track*') ()
TrSIZE         = LoKi.Functors.Size  ('LHCb::Track*') ()

RVSOURCE       = LoKi.RecVertices.SourceTES 
RVSOURCEDIRECT = LoKi.RecVertices.SourceDirect 

RVEMPTY        = LoKi.Functors.Empty ('LHCb::RecVertex*') ()
RVSIZE         = LoKi.Functors.Size  ('LHCb::RecVertex*') ()


## Hlt1 stuff 
TrSELECTION = LoKi.Hlt1.TrSelection 
TrREGISTER  = LoKi.Hlt1.TrRegister
RvSELECTION = LoKi.Hlt1.RvSelection 
RvREGISTER  = LoKi.Hlt1.RvRegister
TrUPGRADE   = LoKi.Hlt1.TrUpgrade
TrMATCH     = LoKi.Hlt1.TrMatch
VxMAKER     = LoKi.Hlt1.VxMaker 
VxUPGRADE   = LoKi.Hlt1.VxUpgrade



# ## Simple way to create the 'smart' function from the regular function.
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
    return LoKi.Dicts.TrInfo.info ( key , fun , update )

info. __doc__ += "\n\n" + LoKi.Dicts.TrInfo.info . __doc__ 
 

# ## ## Simple function to 'bind' the 2-argument function 
def bindMin ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some container
    
    The function comes in two flavours:

    1) for the given argument, return the minimal value of 2D-function against all
    objects from the reference container:

    Find the minimal distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindMin ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !


    2) for the given argument, seek for the element from the reference container
    which minimizes 2D-function and return the value of the second function with
    this minimal element:
    
    Evaluate of the transverse momentum of the track, which
    has the the minimal distance of the closest approach to the given track:
    
    >>> tracks = ...
    >>> pt = bindMin ( TTrDOCA , TrPT , tracks )   ## create the function
    
    >>> track = ...
    >>> value = pt ( track )                ## use it !
    
    """
    return LoKi.Dicts.HltBind.bindMin ( a , b , *c  ) 


# ## Simple function to 'bind' the 2-argument function 
def bindAbsMin ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some container

    The function comes in two flavours:
    
    1) for the given argument, return the minimal (absolute) value
    of 2D-function against all objects from the reference container:
    
    Find the minimal (absolute) distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindAbsMin ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !

    
    2) for the given argument, seek for the element from the reference container
    which minimizes the absolute value for 2D-function and return the value
    of the second function with this minimal element:
    
    Evaluate of the transverse momentum of the track, which
    has the the minimal distance of the closest approach to the given track:
    
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

# ## Simple function to 'bind' the 2-argument function 
def bindMax ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some containers
    
    The function comes in two flavours:

    1) for the given argument, return the maximal value of 2D-function against all
    objects from the reference container:

    find the maximal distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindMax ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !


    2) for the given argument, seek for the element from the reference container
    which maximizes 2D-function and return the value of the secodn function with
    this maximal element:
    
    Evaluate of the transverse momentum of the track, which
    has the the maximal distance of the closest approach to the given track:
    
    >>> tracks = ...
    >>> pt = bindMax ( TTrDOCA , TrPT , tracks )   ## create the function
    
    >>> track = ...
    >>> value = pt ( track )                ## use it !
    
    """
    return LoKi.Dicts.HltBind.bindMin ( a , b , *c  ) 


# ## Simple function to 'bind' the 2-argument function 
def bindAbsMax ( a , b , *c ) :
    """
    Simple function to 'bind' the 2-argument function to some container

    The function comes in two flavours:

    1) for the given argument, return the maximal (absolute) value
    of 2D-function against all objects from the reference container:
    
    Find the maximal (absolute) distance of the closest approach:
    
    >>> tracks = ...
    >>> doca = bindAbsMax ( TTrDOCA , tracks )   ## create the function
    
    >>> track = ...
    >>> value = doca ( track )                ## use it !


    2) for the given argument, seek for the element from the reference container
    which maximizes the absolute value for 2D-function and return the value of
    the second function with this maximal element:
    
    Evaluate of the transverse momentum of the tarck, which
    has the the maximal (absolute) distance of the closest approach to the given track:
    
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
