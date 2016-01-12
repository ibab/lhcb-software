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
__version__ = "CVS tag $Name:$, version $Revision$ "
# =============================================================================


import LoKiCore.decorators as _LoKiCore 

# Namespaces:
from LoKiCore.basic import cpp, std, LoKi
LHCb     = cpp.LHCb

_T = 'const LHCb::Track*'

## FIXME: needed since there is no autoloading of Tracking dictionaries:
import cppyy
cppyy.loadDict('TrackEventDict')

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

 
## @see LoKi::Cuts::TrTYPE
TrTYPE        = LoKi.Tracks.Type  () 

## @see @see LoKi::Cuts::TrALL
TrALL         = LoKi.Constant ( _T , bool ) ( True )
## @see LoKi::Cuts::TrBACKWARD
TrBACKWARD    = LoKi.Tracks.CheckFlag ( LHCb.Track.Backward ) 
## @see LoKi::Cuts::TrCHI2 
TrCHI2        = LoKi.Tracks.Chi2       () 
## @see LoKi::Cuts::TrCHI2PDOF 
TrCHI2PDOF    = LoKi.Tracks.Chi2PerDoF () 
## @see LoKi::Cuts::TrGHOSTPROB 
TrGHOSTPROB    = LoKi.Tracks.GhostProb () 
## @see LoKi::Cuts::TrCLONE
TrCLONE       = LoKi.Tracks.CheckFlag ( LHCb.Track.Clone ) 
## ## @see LoKi::Cuts::TrDE
## TrDE          = LoKi.Tracks.DeltaE     () 
## @see LoKi::Cuts::TrFALSE
TrFALSE       = LoKi.Constant( _T , bool ) ( False )
## @see LoKi::Cuts::TrFILTER
TrFILTER      = LoKi.Tracks.Filter
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
## @see LoKi::Cuts::TrNONE
TrNONE        = LoKi.Constant( _T , bool ) ( False )
## @see LoKi::Cuts::TrONE
TrONE         = LoKi.Constant( _T , 'double' ) ( 1.0 )
## @see LoKi::Cuts::TrP
TrP           = LoKi.Tracks.Momentum          () 
## @see LoKi::Cuts::TrPX
TrPX          = LoKi.Tracks.MomentumX         () 
## @see LoKi::Cuts::TrPY
TrPY          = LoKi.Tracks.MomentumY         () 
## @see LoKi::Cuts::TrPZ
TrPZ          = LoKi.Tracks.MomentumZ         () 
## @see LoKi::Cuts::TrPHI
TrPHI         = LoKi.Tracks.Phi () 
## @see LoKi::Cuts::TrETA
TrETA         = LoKi.Tracks.Eta () 
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
TrSELECTOR    = LoKi.Tracks.Selector 
## @see LoKi::Cuts::TrSINFO
TrSINFO       = LoKi.Tracks.SmartInfo
## @see LoKi::Cuts::TrTRUE
TrTRUE        = LoKi.Constant( _T , bool ) ( True  )
## @see LoKi::Cuts::TrUSED
TrUSED        = LoKi.Tracks.CheckFlag ( LHCb.Track.Used )
## @see LoKi::Cuts::TrZERO
TrZERO        = LoKi.Constant( _T , 'double' ) ( 0.0 )

## @see LoKi::Cuts::TrIDC
TrIDC         = LoKi.Tracks.CountIDs
## @see LoKi::Cuts::TrOTIDC
TrOTIDC       = LoKi.Tracks.CountOTIDs
## @see LoKi::Cuts::TrSTIDC
TrSTIDC       = LoKi.Tracks.CountSTIDs
## @see LoKi::Cuts::TrVELOIDC
TrVELOIDC     = LoKi.Tracks.CountVeloIDs

## @see LoKi::Cuts::TrSTATEZ
TrSTATEZ      = LoKi.Tracks.StateZ
## @see LoKi::Cuts::TrFIRTSHITZ
TrFIRSTHITZ   = LoKi.Tracks.StateZ ( LHCb.State.FirstMeasurement )

## @see LoKi::Cuts::TrCOV2
TrCOV2        = LoKi.Tracks.Cov2

## @see LoKi::Cuts::TrHAST
TrHAST        = LoKi.Tracks.HasT    () 
## @see LoKi::Cuts::TrHAST
TrHASVELO     = LoKi.Tracks.HasVelo () 
## @see LoKi::Cuts::TrHASTT
TrHASTT       = LoKi.Tracks.HasTT   () 

## @see LoKi::Cuts::TrPATQUALITY 
TrPATQUALITY    = LoKi.Tracks.Info  ( LHCb.Track.PatQuality       , -1000 ) 
## @see LoKi::Cuts::TrMATCHCHI2
TrMATCHCHI2     = LoKi.Tracks.Info  ( LHCb.Track.MatchChi2        , -1000 ) 
## @see LoKi::Cuts::TrFITVELOCHI2
TrFITVELOCHI2   = LoKi.Tracks.Info  ( LHCb.Track.FitVeloChi2      , -1000 )
## @see LoKi::Cuts::TrFITVELONDOF
TrFITVELONDOF   = LoKi.Tracks.Info  ( LHCb.Track.FitVeloNDoF      , -1000 )
## @see LoKi::Cuts::TrFITTCHI2
TrFITTCHI2      = LoKi.Tracks.Info  ( LHCb.Track.FitTChi2         , -1000 )
## @see LoKi::Cuts::TrFITTNDOF
TrFITTNDOF      = LoKi.Tracks.Info  ( LHCb.Track.FitTNDoF         , -1000 )
## @see LoKi::Cuts::TrFITMATCHCHI2
TrFITMATCHCHI2  = LoKi.Tracks.Info  ( LHCb.Track.FitMatchChi2     , -1000 )
## @see LoKi::Cuts::TrTSALIKELIHOOD
TrTSALIKELIHOOD = LoKi.Tracks.Info  ( LHCb.Track.TsaLikelihood    , -1000 )
## @see LoKi::Cuts::TrCLONEDIST
TrCLONEDIST     = LoKi.Tracks.Info  ( LHCb.Track.CloneDist        , 1.e+9 ) 

## @see LoKi::Cuts::TrNVELOMISS
TrNVELOMISS     = LoKi.Tracks.NVeloMissed ()
## @see LoKi::Cuts::TrNTHITS    (for Johannes)
TrNTHITS        = LoKi.Tracks.NTHits      ()
## @see LoKi::Cuts::TrTNORMIDC  (for Vava) 
TrTNORMIDC      = LoKi.Tracks.NTHits      ()

## @see LoKi::Cuts::TrNCONSECVELOSPACEPOINTS
TrNCONSECVELOSPACEPOINTS = LoKi.Tracks.MaxNumConsecutiveVeloSpacePoints ()
## @see LoKi::Tracks::NumVeloSpacePoints
TrNVELOSPACEPOINTS       = LoKi.Tracks.NumVeloSpacePoints               ()
## @see LoKi::Tracks::NumVeloACOverlapHits
TrNVELOOVERLAPS          = LoKi.Tracks.NumVeloACOverlapHits             ()
## @see LoKi::Tracks::NumVeloACOverlapHitsR
TrNVELOOVERLAPSR         = LoKi.Tracks.NumVeloACOverlapHitsR            ()
## @see LoKi::Tracks::NumVeloACOverlapHitsPhi
TrNVELOOVERLAPSPHI       = LoKi.Tracks.NumVeloACOverlapHitsPhi          ()

# =============================================================================

## @see LoKi::Tracks::FastDOCAToBeamLine
Tr_FASTDOCATOBEAMLINE = LoKi.Tracks.FastDOCAToBeamLine



## functional part
_vt       = 'std::vector<const LHCb::Track*>'
_vd       = 'std::vector<double>'
#
TrMaps         = LoKi.Functor             ( _vt  , _vd       )
TrMap          = LoKi.FunctorFromFunctor  ( _vt  , _vd       )
TrPipes        = LoKi.Functor             ( _vt  , _vt       )
TrPipe         = LoKi.FunctorFromFunctor  ( _vt  , _vt       )
TrFunVals      = LoKi.Functor             ( _vt  , 'double'  )
TrFunVal       = LoKi.FunctorFromFunctor  ( _vt  , 'double'  )
TrSources      = LoKi.Functor             ('void', _vt  )
TrSource       = LoKi.FunctorFromFunctor  ('void', _vt  )
TrCutVals      = LoKi.Functor             ( _vt  , bool )
TrCutVal       = LoKi.FunctorFromFunctor  ( _vt  , bool )

TrSOURCE       = LoKi.Tracks.SourceTES 
TrTES          = LoKi.Tracks.SourceTES 

TrEMPTY        = LoKi.Functors.Empty ( _T ) ()
TrSIZE         = LoKi.Functors.Size  ( _T ) ()

TrNUM          = LoKi.Tracks.TESCounter


TrFAKESOURCE   = LoKi.Functors.FakeSource( _T )
TrVALID        = LoKi.Valid              ( _T )()

# =============================================================================
## helper function to create ``smart'' RecSumary functor 
#  @see HASRECSUMMARY
#  @see    RECSUMMARY
#  @see      TrNUM
#  @author rlambert
#  @date  26.11.2011
def  recSummaryTrack ( index , trtype, location='Rec/Track/Best' ) :
    """
    Helper function to create ``smart'' RecSummary-fucntor
    
    result = has summary  ? summary : contains
    
    """
    #
    from LoKiCore.functions import switch
    #
    from LoKiNumbers.functions import (HASRECSUMMARY, RECSUMMARY)
    return switch ( HASRECSUMMARY ( index      ) ,
                    RECSUMMARY    ( index , -1 ) ,
                    TrNUM      ( location , trtype  ) )  

# 
# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
