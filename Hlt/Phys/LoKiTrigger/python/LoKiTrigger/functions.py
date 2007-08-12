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
## @see LoKi::Cuts::TrDOCA
TrDOCA        = LoKi.Tracks.DistanceOfClosestApproach ()
## @see LoKi::Cuts::TrDOWNSTREAM
TrDOWNSTREAM  = equal_to ( TrTYPE , LHCb.Track.Downstream ) 
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
# The END 
# =============================================================================
