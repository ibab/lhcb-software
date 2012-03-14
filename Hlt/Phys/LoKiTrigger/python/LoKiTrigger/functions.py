#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic objects from LoKiTrigger library
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
#  with the smear campaign of Dr.O.Callot et al.:
#   ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-06-09
# =============================================================================
"""
The set of basic objects from LoKiTrigger library


      This file is a part of LoKi project -
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
contributions and advices from G.Raven, J.van Tilburg,
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement
with the smear campaign of Dr.O.Callot et al.:
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__date__    = "2007-06-09"
__version__ = "$Revision$ "
# =============================================================================

import LoKiCore.decorators as _LoKiCore
import LoKiTrigger.Hlt

# Namespaces:
from LoKiCore.basic import cpp, std, LoKi
LHCb     = cpp.LHCb
Hlt      = cpp.Hlt

# =============================================================================
## new stuff ##
# =============================================================================

_TC = 'const Hlt::Candidate*'
_TS = 'const Hlt::Stage*'
_vC = std.vector( _TC      ) ## std::vector<const Hlt::Candidate*>
_vD = std.vector( 'double' ) ## std::vector<double>


## @see LoKi::Types::TC_Func
TC_Func      = LoKi.Functor                 ( _TC    , 'double' )
## @see LoKi::Types::TC_Cuts
TC_Cuts      = LoKi.Functor                 ( _TC    ,  bool    )
## @see LoKi::Types::TC_Fun
TC_Fun       = LoKi.FunctorFromFunctor      ( _TC    , 'double' )
## @see LoKi::Types::TC_Cut
TC_Cut       = LoKi.FunctorFromFunctor      ( _TC    ,  bool    )

## @see LoKi::Types::TS_Func
TS_Func      = LoKi.Functor                 ( _TS    , 'double' )
## @see LoKi::Types::TC_Cuts
TS_Cuts      = LoKi.Functor                 ( _TS    ,  bool    )
## @see LoKi::Types::TC_Fun
TS_Fun       = LoKi.FunctorFromFunctor      ( _TS    , 'double' )
## @see LoKi::Types::TC_Cut
TS_Cut       = LoKi.FunctorFromFunctor      ( _TS    ,  bool    )

## @see LoKi::Types::TC_Maps
TC_Maps      = LoKi.Functor                 ( _vC    , _vD      )
## @see LoKi::Types::TC_Map
TC_Map       = LoKi.FunctorFromFunctor      ( _vC    , _vD      )
## @see LoKi::Types::TC_Pipes
TC_Pipes     = LoKi.Functor                 ( _vC    , _vC      )
## @see LoKi::Types::TC_Pipe
TC_Pipe      = LoKi.FunctorFromFunctor      ( _vC    , _vC      )
## @see LoKi::Types::TC_FunVals
TC_FunVals   = LoKi.Functor                 ( _vC    , 'double' )
## @see LoKi::Types::TC_FunVal
TC_FunVal    = LoKi.FunctorFromFunctor      ( _vC    , 'double' )
## @see LoKi::Types::TC_CutVals
TC_CutVals   = LoKi.Functor                 ( _vC    , bool     )
## @see LoKi::Types::TC_CutVal
TC_CutVal    = LoKi.FunctorFromFunctor      ( _vC    , bool     )
## @see LoKi::Types::TC_Sources
TC_Sources   = LoKi.Functor                 ( 'void' , _vC      )
## @see LoKi::Types::TC_Source
TC_Source    = LoKi.FunctorFromFunctor      ( 'void' , _vC      )

TC_ALL       = LoKi.Constant ( _TC , bool ) ( True  )
TC_NONE      = LoKi.Constant ( _TC , bool ) ( False )

TS_ALL       = LoKi.Constant ( _TS , bool ) ( True  )
TS_NONE      = LoKi.Constant ( _TS , bool ) ( False )


## @see LoKi::Cuts::TC_TOTSTAGES
TC_TOTSTAGES = LoKi.Candidates.TotalStages ()
## @see LoKi::Cuts::TC_NSTAGES
TC_NSTAGES   = LoKi.Candidates.NStages
## @see LoKi::Cuts::TC_StFUN
TC_StFUN     = LoKi.Candidates.StageFun
## @see LoKi::Cuts::TC_StCUT
TC_StCUT     = LoKi.Candidates.StageCut
## @see LoKi::Cuts::TC_BRANCH
TC_BRANCH    = LoKi.Candidates.Branch       ()

## @see LoKi::Cuts::TC_FUN
TC_FUN       = LoKi.Candidates.SlotFun
## @see LoKi::Cuts::TC_FUN
TC_CUT       = LoKi.Candidates.SlotCut

## @see LoKi::Cuts::TC_FILTER
TC_FILTER    = LoKi.Candidates.SlotFilter
## @see LoKi::Cuts::TC_MAP
TC_MAP       = LoKi.Candidates.SlotMap


## @see LoKi::Cuts::TS_ISTRACK
TS_ISTRACK      = LoKi.Stages.IsTrack       ()
## @see LoKi::Cuts::TS_ISL0MUON
TS_ISL0MUON     = LoKi.Stages.IsL0Muon      ()
## @see LoKi::Cuts::TS_ISL0DIMUON
TS_ISL0DIMUON   = LoKi.Stages.IsL0DiMuon    ()
## @see LoKi::Cuts::TS_ISL0CALO
TS_ISL0CALO     = LoKi.Stages.IsL0Calo      ()
## @see LoKi::Cuts::TS_ISVERTEX
TS_ISVERTEX     = LoKi.Stages.IsVertex      ()
## @see LoKi::Cuts::TS_ISMULTITRACK
TS_ISMULTITRACK = LoKi.Stages.IsMultiTrack  ()
## @see LoKi::Cuts::TS_ISSTAGE
TS_ISSTAGE      = LoKi.Stages.IsStage       ()

## @see LoKi::Cuts::TS_TYPE
TS_TYPE         = LoKi.Stages.Type          ()

## @see LoKi::Cuts::TS_TrFUN
TS_TrFUN        = LoKi.Stages.TrFun
## @see LoKi::Cuts::TS_TrCUT
TS_TrCUT        = LoKi.Stages.TrCut

## @see LoKi::Cuts::TS_LOCKED
TS_LOCKED       = LoKi.Stages.Locked        ()

## @see LoKi::Cuts::TS_HISTORY
TS_HISTORY      = LoKi.Stages.History

## @see LoKi::Cuts::TS_HISTORY_SUB
TS_HISTORY_SUB  = LoKi.Stages.HistorySub

## @see LoKi::Cuts::TS_HISTORY_RE
TS_HISTORY_RE   = LoKi.Stages.HistoryRegex

## @see LoKi::Cuts::TS_HASCACHE
TS_HASCACHE     = LoKi.Stages.HasCache 

## @see LoKi::Cuts::TS_CACHE_DOUBLE
TS_CACHE_DOUBLE = LoKi.Stages.Cache1

## @see LoKi::Cuts::TS_CACHE_BOOL
TS_CACHE_BOOL   = LoKi.Stages.Cache2

## @see LoKi::Cuts::TC_SELECTION
TC_SELECTION    = LoKi.Hlt1.Selection
## @see LoKi::Cuts::SELECTION
SELECTION       = LoKi.Hlt1.Selection
## @see LoKi::Cuts::TV_SELECTION
VX_SELECTION    = LoKi.Hlt1.VxSelection

## @see LoKi::Cuts::TC_SINK
TC_SINK         = LoKi.Hlt1.Sink
## @see LoKi::Cuts::SINK
SINK            = LoKi.Hlt1.Sink
## @see LoKi::Cuts::VX_SINK
VX_SINK         = LoKi.Hlt1.VxSink

## @see LoKi::Cuts::TC_SIZE
TC_SIZE         = LoKi.Functors.Size  ( _TC ) ()
## @see LoKi::Cuts::TC_EMPTY
TC_EMPTY        = LoKi.Functors.Empty ( _TC ) ()


# =============================================================================
## Upgrade:
# =============================================================================

## @see LoKi::Cuts::TC_UPGRADE_ALL
TC_UPGRADE_ALL  = LoKi.Hlt1.Upgrade
## @see LoKi::Cuts::TC_UPGRADE_TR
TC_UPGRADE_TR   = LoKi.Hlt1.UpgradeTracks
## @see LoKi::Cuts::TC_UPGRADE_MT
TC_UPGRADE_MT   = LoKi.Hlt1.UpgradeMultiTracks
## @see LoKi::Cuts::TC_UPGRADE_VX
TC_UPGRADE_VX   = LoKi.Hlt1.UpgradeVertices

# =============================================================================
## Filter:
# =============================================================================
TC_FILTER_TR   = LoKi.Hlt1.FilterTracks

# =============================================================================
## Match
# =============================================================================

## @see LoKi::Cuts::TC_MATCH
TC_MATCH      = LoKi.Hlt1.Match
## @see LoKi::Cuts::TC_MATCH2
TC_MATCH2     = LoKi.Hlt1.Match2
## @see LoKi::Cuts::TC_MATCH
TC_MATCHFLTR  = LoKi.Hlt1.FilterMatch
## @see LoKi::Cuts::TC_MATCH2
TC_MATCHFLTR2 = LoKi.Hlt1.FilterMatch2

# =============================================================================
## vertex making 
# =============================================================================

## @see LoKi::Cuts::TC_VXMAKE 
TC_VXMAKE      = LoKi.Hlt1.VxMaker
## @see LoKi::Cuts::TC_VXMAKE2 
TC_VXMAKE2     = LoKi.Hlt1.VxMaker2
## @see LoKi::Cuts::TC_VXMAKE3 
TC_VXMAKE3     = LoKi.Hlt1.VxMaker3
## @see LoKi::Cuts::TC_VXMAKE4 
TC_VXMAKE4     = LoKi.Hlt1.VxMaker4

# =============================================================================
## di-tracks makers
# =============================================================================
## @see LoKi::Cuts::TC_DITRACKS
TC_DITRACKS    = LoKi.Hlt1.DiTrackMaker
## @see LoKi::Cuts::TC_DITRACKS2
TC_DITRACKS2   = LoKi.Hlt1.DiTrackMaker2

# =============================================================================
## end of new stuff ##
# =============================================================================

## @see LoKi::Cuts::VX_BEAMSPOTRHO
BEAMSPOT       = LoKi.Vertices.BeamSpot
VX_BEAMSPOTRHO = LoKi.Vertices.BeamSpotRho

# ============================================================================
## Hlt1Wrappers
# ============================================================================

TrackFunction       = LoKi.Hlt1.TrackFunction
TrFUN               = LoKi.Tracks.TrFunction

# ============================================================================
## Track functions 
# ============================================================================

## @see LoKi::Cuts::TrDPMU_ 
TrDPMU_             = LoKi.Tracks.MuonDeltaP
## @see LoKi::Cuts::TrDPMU
TrDPMU              = LoKi.Tracks.MuonDeltaP() 
## @see LoKi::Cuts::Tr_HLTMIP 
Tr_HLTMIP           = LoKi.Tracks.Hlt1TrackMinIp
## @see LoKi::Cuts::Tr_HLTMIPCHI2  
Tr_HLTMIPCHI2       = LoKi.Tracks.Hlt1TrackMinIpChi2


# ============================================================================
## Vertex functions 
# ============================================================================
## @see LoKi::Cuts::RV_MAXDOCA 
RV_MAXDOCA       = LoKi.RecVertices.MaxDOCA
## @see LoKi::Cuts::RV_MAXDOCACHI2 
RV_MAXDOCACHI2   = LoKi.RecVertices.MaxDOCAChi2
## @see LoKi::Cuts::RV_DOCACUT
RV_DOCACUT       = LoKi.RecVertices.DOCACut
## @see LoKi::Cuts::HLT_TCASRVC_ANY
HLT_TCASRVC_ANY      = LoKi.RecVertices.Hlt_TrackCutAsRecVertexCut_Any
## @see LoKi::Cuts::HLT_TCASRVC_ALL
HLT_TCASRVC_ALL      = LoKi.RecVertices.Hlt_TrackCutAsRecVertexCut_All


##
LoKi.Hlt1.UpgradeConf      . __str__  =  LoKi.Hlt1.UpgradeConf     . toString
LoKi.Hlt1.UpgradeConf      . __repr__ =  LoKi.Hlt1.UpgradeConf     . toString
LoKi.Hlt1.MatchConf        . __str__  =  LoKi.Hlt1.MatchConf       . toString
LoKi.Hlt1.MatchConf        . __repr__ =  LoKi.Hlt1.MatchConf       . toString
LoKi.Hlt1.TrackFunction    . __str__  =  LoKi.Hlt1.TrackFunction   . toString
LoKi.Hlt1.TrackFunction    . __repr__ =  LoKi.Hlt1.TrackFunction   . toString
LoKi.Hlt1.VxMakerConf      . __str__  =  LoKi.Hlt1.VxMakerConf     . toString
LoKi.Hlt1.VxMakerConf      . __repr__ =  LoKi.Hlt1.VxMakerConf     . toString

##LoKi.Hlt1.TrackBiFunction  . __str__  =  LoKi.Hlt1.TrackBiFunction . toString
##LoKi.Hlt1.TrackBiFunction  . __repr__ =  LoKi.Hlt1.TrackBiFunction . toString


## TrackBiFunction     = LoKi.Hlt1.TrackBiFunction
## MatchTVelo          = LoKi.Hlt1.MatchTVelo
## TrackVertexFunction = LoKi.Hlt1.TrackVertexFunction

# functional stuff


## functional part
## _vt       = 'std::vector<LHCb::Track*>'     ## std.vector ( 'LHCb::Track*'     )
## _vv       = 'std::vector<LHCb::RecVertex*>' ## std.vector ( 'LHCb::RecVertex*' )
## _vd       = 'std::vector<double>'           ## std.vector ( 'double')
#

# =============================================================================
if '__main__' == __name__ :
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__
    print 80*'*'

# =============================================================================
# The END
# =============================================================================
