#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# $URL$
# =============================================================================
## @file functions.py LoKiPhys/functions.py
#  The set of basic decorator for objects from LoKiPhys library
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
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiPhys library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "????-??-??"
__version__ = "Version $Revision$"
# =============================================================================


import LoKiCore.decorators    as      _LoKiCore
from   LoKiCore.functions     import  equal_to 

# Namespaces:
from LoKiCore.basic import cpp, std, LoKi, LHCb, Gaudi 


_RCP = 'const LHCb::Particle*'
_RCV = 'const LHCb::VertexBase*'

_c1 = std.vector( _RCP )
_c2 = std.vector( _RCV )

## ranges
Range   = Gaudi.NamedRange_ ( _c1    )
VRange  = Gaudi.NamedRange_ ( _c2    )
## range-lists: 
RangeL  = LoKi.RangeList_  (  Range )
VRangeL = LoKi.RangeList_  ( VRange )


# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::Func
Func  = LoKi.Functor                ( _RCP , 'double' ) 
## @see LoKi::Types::Cuts
Cuts  = LoKi.Functor                ( _RCP ,  bool    ) 
## @see LoKi::Types::Fun
Fun   = LoKi.FunctorFromFunctor     ( _RCP , 'double' ) 
## @see LoKi::Types::Cut
Cut   = LoKi.FunctorFromFunctor     ( _RCP ,  bool    ) 
## @see LoKi::Types::Func
VFunc = LoKi.Functor                ( _RCV , 'double' ) 
## @see LoKi::Types::Cuts
VCuts = LoKi.Functor                ( _RCV ,  bool    ) 
## @see LoKi::Types::Fun
VFun  = LoKi.FunctorFromFunctor     ( _RCV , 'double' ) 
## @see LoKi::Types::Cut
VCut  = LoKi.FunctorFromFunctor     ( _RCV ,  bool    ) 


# =============================================================================
# Functional part: 
# =============================================================================

## functional part
#_vp       = std.vector ( _RCP    )
#_vv       = std.vector ( _RCV    )
#_vd       = std.vector ('double')

_vp       = 'std::vector<const LHCb::Particle*>'   ## std.vector ( _RCP    )
_vv       = 'std::vector<const LHCb::VertexBase*>' ## std.vector ( _RCV    )
_vd       = 'std::vector<double>'                  ## std.vector ('double')

#_vp       = std.vector ( _RCP    )
#_vv       = std.vector ( _RCV    )
#_vd       = std.vector ('double')
#

Maps      = LoKi.Functor             ( _vp    , _vd      )
Map       = LoKi.FunctorFromFunctor  ( _vp    , _vd      )
Pipes     = LoKi.Functor             ( _vp    , _vp      )
Pipe      = LoKi.FunctorFromFunctor  ( _vp    , _vp      )
FunVals   = LoKi.Functor             ( _vp    , 'double' )
FunVal    = LoKi.FunctorFromFunctor  ( _vp    , 'double' )
CutVals   = LoKi.Functor             ( _vp    , bool     )
CutVal    = LoKi.FunctorFromFunctor  ( _vp    , bool     )
Elements  = LoKi.Functor             ( _vp    , _RCP     ) 
Element   = LoKi.FunctorFromFunctor  ( _vp    , _RCP     ) 
Sources   = LoKi.Functor             ( 'void' , _vp      ) 
Source    = LoKi.FunctorFromFunctor  ( 'void' , _vp      ) 
#

VMaps     = LoKi.Functor             ( _vv , _vd      )
VMap      = LoKi.FunctorFromFunctor  ( _vv , _vd      )
VPipes    = LoKi.Functor             ( _vv , _vv      )
VPipe     = LoKi.FunctorFromFunctor  ( _vv , _vv      )
VFunVals  = LoKi.Functor             ( _vv , 'double' )
VFunVal   = LoKi.FunctorFromFunctor  ( _vv , 'double' )
VCutVals  = LoKi.Functor             ( _vv , bool     )
VCutVal   = LoKi.FunctorFromFunctor  ( _vv , bool     )
VElements = LoKi.Functor             ( _vv , _RCV     ) 
VElement  = LoKi.FunctorFromFunctor  ( _vv , _RCV     ) 
VSources  = LoKi.Functor             ( 'void' , _vv   ) 
VSource   = LoKi.FunctorFromFunctor  ( 'void' , _vv   ) 

EMPTY  = LoKi.Functors.Empty ( _RCP ) ()
VEMPTY = LoKi.Functors.Empty ( _RCV ) ()
SIZE   = LoKi.Functors.Size  ( _RCP ) ()
VSIZE  = LoKi.Functors.Size  ( _RCV ) ()



# =============================================================================
## All concrete types 
# =============================================================================

## @see LoKi::Cuts::ARMENTEROS 
ARMENTEROS = LoKi.Particles.ArmenterosPodolanski ()

## @see LoKi::Cuts::ALL
ALL      = LoKi.Constant( _RCP , bool ) ( True  )
## @see LoKi::Cuts::NONE
NONE     = LoKi.Constant( _RCP , bool ) ( False )
## @see LoKi::Cuts::PALL
PALL     = LoKi.Constant( _RCP , bool ) ( True  )
## @see LoKi::Cuts::PNONE
PNONE    = LoKi.Constant( _RCP , bool ) ( False )
## @see LoKi::Cuts::ONE
ONE      = LoKi.Constant     ( _RCP , 'double' ) ( 1.0 )
## @see LoKi::Cuts::ZERO
ZERO     = LoKi.Constant     ( _RCP , 'double' ) ( 0.0 )
## @see LoKi::Cuts::VALID
VALID    = LoKi.Valid        ( _RCP )()
## @see LoKi::Cuts::SAME
SAME     = LoKi.TheSame      ( _RCP )
## @see LoKi::Cuts::HASKEY
HASKEY   = LoKi.Particles.HasKey   () 
## @see LoKi::Cuts::KEY
KEY      = LoKi.Particles.Key      () 
## @see LoKi::Cuts::PKEY
PKEY     =  KEY
## @see LoKi::Cuts::INTES
INTES    = LoKi.Particles.InTES       
## @see LoKi::Cuts::PINTES
PINTES   = INTES
## @see LoKi::Cuts::ID
ID       = LoKi.Particles.Identifier ()
## @see LoKi::Cuts::ABSID
ABSID    = LoKi.Particles.AbsIdentifier ()
## @see LoKi::Cuts::Q
Q        = LoKi.Particles.Charge        ()
## @see LoKi::Cuts::SUMQ
SUMQ     = LoKi.Particles.SumCharge     ()
## @see LoKi::Cuts::P   
P        = LoKi.Particles.Momentum      ()
## @see LoKi::Cuts::P2  
P2       = LoKi.Particles.Momentum2     ()
## @see LoKi::Cuts::E   
E        = LoKi.Particles.Energy        ()
## @see LoKi::Cuts::P2  
PT       = LoKi.Particles.TransverseMomentum () 
## @see LoKi::Cuts::PX  
PX       = LoKi.Particles.MomentumX     ()
## @see LoKi::Cuts::PY  
PY       = LoKi.Particles.MomentumY     ()
## @see LoKi::Cuts::PZ  
PZ       = LoKi.Particles.MomentumZ     ()
## @see LoKi::Cuts::ETA 
ETA      = LoKi.Particles.PseudoRapidity () 
## @see LoKi::Cuts::PHI 
PHI      = LoKi.Particles.Phi            () 
## @see LoKi::Cuts::M   
M        = LoKi.Particles.Mass           () 
## @see LoKi::Cuts::MM  
MM       = LoKi.Particles.MeasuredMass   () 
## @see LoKi::Cuts::MASS 
MASS     = LoKi.Particles.InvariantMass     
## @see LoKi::Cuts::M1
M1       = MASS(1)
## @see LoKi::Cuts::M12  
M12      = MASS(1,2)
## @see LoKi::Cuts::M13  
M13      = MASS(1,3)
## @see LoKi::Cuts::M14  
M14      = MASS(1,4)
## @see LoKi::Cuts::M2  
M2       = MASS(2)
## @see LoKi::Cuts::M23  
M23      = MASS(2,3)
## @see LoKi::Cuts::M24  
M124     = MASS(2,4)
## @see LoKi::Cuts::M3  
M3       = MASS(3)
## @see LoKi::Cuts::M34  
M34      = MASS(3,4)
## @see LoKi::Cuts::M4  
M4       = MASS(4)
## @see LoKi::Cuts::NMASS
NMASS    = LoKi.Particles.NominalMass ()
## @see LoKi::Cuts::DMASS 
DMASS    = LoKi.Particles.DeltaMass         
## @see LoKi::Cuts::ADMASS 
ADMASS   = LoKi.Particles.AbsDeltaMass         
## @see LoKi::Cuts::DMMASS 
DMMASS   = LoKi.Particles.DeltaMeasuredMass         
## @see LoKi::Cuts::ADMMASS 
ADMMASS  = LoKi.Particles.AbsDeltaMeasuredMass         
## @see LoKi::Cuts::CHI2M
CHI2M    = LoKi.Particles.DeltaMassChi2
## @see LoKi::Cuts::CHI2MASS
CHI2MASS = CHI2M
## @see LoKi::Cuts::CHI2MM
CHI2MM    = LoKi.Particles.DeltaMeasuredMassChi2
## @see LoKi::Cuts::CHI2MMASS
CHI2MMASS = CHI2MM
## @see LoKi::Cuts::CL  
CL       = LoKi.Particles.ConfidenceLevel   () 
## @see LoKi::Cuts::WEIGHT
WEIGHT   = LoKi.Particles.Weight            () 
## @see LoKi::Cuts::ISBASIC 
ISBASIC  = LoKi.Particles.IsBasic           () 
## @see LoKi::Cuts::ISMUON
ISMUON   = LoKi.Particles.IsMuon            ()
## @see LoKi::Cuts::ISMUONLOOSE
ISMUONLOOSE = LoKi.Particles.IsMuonLoose    ()
## @see LoKi::Cuts::ISLOOSEMUON
ISLOOSEMUON = LoKi.Particles.IsMuonLoose    ()
## @see LoKi::Cuts::INMUON
INMUON   = LoKi.Particles.InMuonAcceptance  ()
## @see LoKi::Cuts::NDAUGS  
NDAUGS   = LoKi.Particles.NumberOfDaughters () 
## @see LoKi::Cuts::HASINFO
HASINFO  = LoKi.Particles.HasInfo
## @see LoKi::Cuts::INFO
INFO     = LoKi.Particles.Info
## @see LoKi::Cuts::HASPROTO
HASPROTO = LoKi.Particles.HasProto         () 
## @see LoKi::Cuts::HASVERTEX
HASVERTEX= LoKi.Particles.HasVertex        () 
## @see LoKi::Cuts::VD
VD       = LoKi.Particles.VertexDistance   
## @see LoKi::Cuts::VDSIGN
VDSIGN   = LoKi.Particles.VertexSignedDistance   
## @see LoKi::Cuts::VDDOT 
VDDOT    = LoKi.Particles.VertexDotDistance   
## @see LoKi::Cuts::VDCHI2 
VDCHI2   = LoKi.Particles.VertexChi2Distance   
## @see LoKi::Cuts::VDMIN
VDMIN    = LoKi.Particles.MinVertexDistance   
## @see LoKi::Cuts::MINVD
MINVD    = VDMIN
## @see LoKi::Cuts::VDMINCHI2
VDMINCHI2= LoKi.Particles.MinVertexChi2Distance   
## @see LoKi::Cuts::TIMEDIST
TIMEDIST = LoKi.Particles.TimeDistance
## @see LoKi::Cuts::CTAU
CTAU     = TIMEDIST
## @see LoKi::Cuts::TDIST
TDIST    = TIMEDIST
## @see LoKi::Cuts::TD
TD       = TIMEDIST
## @see LoKi::Cuts::TSIGND   
TSIGND   = LoKi.Particles.TimeSignedDistance
## @see LoKi::Cuts::TDSIGN    
TDSIGN   =  TSIGND 
## @see LoKi::Cuts::CTAUSIGN    
CTAUSIGN =  TSIGND 
## @see LoKi::Cuts::TDOT
TDOT     = LoKi.Particles.TimeDotDistance
## @see LoKi::Cuts::CTAUDOT
CTAUDOT  = TDOT 
## @see LoKi::Cuts::CLAPP
CLAPP    = LoKi.Particles.ClosestApproach 
## @see LoKi::Cuts::CLAPPCHI2
CLAPPCHI2= LoKi.Particles.ClosestApproachChi2
## @see LoKi::Cuts::MINCLAPP
MINCLAPP = LoKi.Particles.MinClosestApproach 
## @see LoKi::Cuts::MINCLAPPCHI2 
MINCLAPPCHI2 = LoKi.Particles.MinClosestApproachChi2 
## @see LoKi::Cuts::IP
IP       = LoKi.Particles.ImpPar
## @see LoKi::Cuts::IPCHI2 
IPCHI2   = LoKi.Particles.ImpParChi2 
## @see LoKi::Cuts::IPMIN
IPMIN    = LoKi.Particles.MinImpPar 
## @see LoKi::Cuts::MIP
MIP      =  IPMIN
## @see LoKi::Cuts::MINIP
MINIP    =  IPMIN
## @see LoKi::Cuts::MIPCHI2 
MIPCHI2  = LoKi.Particles.MinImpParChi2 
## @see LoKi::Cuts::CHI2MIP 
CHI2MIP  = MIPCHI2
## @see LoKi::Cuts::INTREE
INTREE   = LoKi.Particles.InTree
## @see LoKi::Cuts::NINTREE
NINTREE  = LoKi.Particles.NinTree
## @see LoKi::Cuts::SUMTREE
SUMTREE  = LoKi.Particles.SumTree
## @see LoKi::Cuts::MULTTREE
MULTTREE = LoKi.Particles.MultTree
## @see LoKi::Cuts::MINTREE
MINTREE  = LoKi.Particles.MinTree
## @see LoKi::Cuts::MAXTREE
MAXTREE  = LoKi.Particles.MaxTree
## @see LoKi::Cuts::CHILD 
CHILD    = LoKi.Particles.ChildFunction
## @see LoKi::Cuts::CHILDFUN
CHILDFUN = CHILD 
## @see LoKi::Cuts::CHILDCUT 
CHILDCUT = LoKi.Particles.ChildPredicate
## @see LoKi::Cuts::VFASPF 
VFASPF   = LoKi.Particles.VFunAsPFun
## @see LoKi::Cuts::HASPROTOS 
HASPROTOS= LoKi.Particles.HasProtos 
## @see LoKi::Cuts::PROTOS
PROTOS   = HASPROTOS 
## @see LoKi::Cuts::HASPROTOSINTREE 
HASPROTOSINTREE = LoKi.Particles.HasProtosInTree
## @see LoKi::Cuts::PROTOSINTREE
PROTOSINTREE = HASPROTOSINTREE 
## @see LoKi::Cuts::HASTRACK
HASTRACK  = LoKi.Particles.HasTrack() 
## @see LoKi::Cuts::HASTRACKS
HASTRACKS = LoKi.Particles.HasTracks 
## @see LoKi::Cuts::TRACKS
TRACKS    = HASTRACKS  
## @see LoKi::Cuts::HASTRACKSINTREE
HASTRACKSINTREE = LoKi.Particles.HasTracksInTree
## @see LoKi::Cuts::TRACKSINTREE
TRACKSINTREE = HASTRACKSINTREE  
## @see LoKi::Cuts::PVTRACK
PVTRACK  = LoKi.Particles.HasTracksFromPV
## @see LoKi::Cuts::TRACKFROMPV 
TRACKFROMPV = PVTRACK
## @see LoKi::Cuts::PVTRACKINTREE 
PVTRACKINTREE = LoKi.Particles.HasTracksInTreeFromPV
## @see LoKi::Cuts::TRACKFROMPVINTREE 
TRACKFROMPVINTREE = PVTRACKINTREE 
## @see LoKi::Cuts::IS 
IS       = LoKi.Particles.IsAParticle
## @see LoKi::Cuts::IN
IS       = IS
## @see LoKi::Cuts::ISINTREE 
ISINTREE = LoKi.Particles.IsAParticleInTree 
## @see LoKi::Cuts::FROMTREE 
FROMTREE = LoKi.Particles.IsAParticleInTree 
## @see LoKi::Cuts::FROM
FROM     = FROMTREE
## @see LoKi::Cuts::PDGMASS
PDGMASS  = LoKi.Particles.NominalMass ()
## @see LoKi::Cuts::PPHASINFO
PPHASINFO= LoKi.Particles.ProtoHasInfo
## @see LoKi::Cuts::PPINFO
PPINFO   = LoKi.Particles.ProtoInfo
## @see LoKi::Cuts::PIDe
PIDe     = PPINFO( LHCb.ProtoParticle.CombDLLe  , 0 , -1000 )
## @see LoKi::Cuts::DLLe 
DLLe     = PIDe
## @see LoKi::Cuts::PIDmu
PIDmu    = PPINFO( LHCb.ProtoParticle.CombDLLmu , 0 , -1000 )
## @see LoKi::Cuts::DLLmu
DLLmu    = PIDmu
## @see LoKi::Cuts::PIDpi
PIDpi    = PPINFO( LHCb.ProtoParticle.CombDLLpi , 0 , -1000 )
## @see LoKi::Cuts::DLLmu
DLLpi    = PIDpi
## @see LoKi::Cuts::PIDK
PIDK     = PPINFO( LHCb.ProtoParticle.CombDLLk  , 0 , -1000 )
## @see LoKi::Cuts::DLLK  
DLLK     = PIDK  
## @see LoKi::Cuts::PIDp
PIDp     = PPINFO( LHCb.ProtoParticle.CombDLLp  , 0 , -1000 )
## @see LoKi::Cuts::DLLp  
DLLp     = PIDp 
## @see LoKi::Cuts::PPHASRICH
PPHASRICH= LoKi.Particles.ProtoHasRichPID     ()
## @see LoKi::Cuts::HASRICH
HASRICH  = PPHASRICH 
## @see LoKi::Cuts::PPHASMUON
PPHASMUON= LoKi.Particles.ProtoHasMuonPID     () 
## @see LoKi::Cuts::HASMUON
HASMUON  = PPHASMUON
## @see LoKi::Cuts::PPHASCALOS
PPHASCALOS = LoKi.Particles.ProtoHasCaloHypos () 
## @see LoKi::Cuts::HASCALOS
HASCALOS = PPHASCALOS
## @see LoKi::Cuts::THASINFO 
THASINFO = LoKi.Particles.TrackHasInfo
## @see LoKi::Cuts::TINFO
TINFO    = LoKi.Particles.TrackInfo 
## @see LoKi::Cuts::TRACKINFO
TRACKINFO= LoKi.Particles.TrackInfo 
## @see LoKi::Cuts::TRCHI2 
TRCHI2   = LoKi.Particles.TrackChi2           ()
## @see LoKi::Cuts::TRCHI2DOF 
TRCHI2DOF= LoKi.Particles.TrackChi2PerDoF     () 
## @see LoKi::Cuts::HASSTATE  
HASSTATE = LoKi.Particles.TrackHasState 
## @see LoKi::Cuts::TRTYPE 
TRTYPE   = LoKi.Particles.TrackType           () 
## @see LoKi::Cuts::TRHISTORY
TRHISTORY= LoKi.Particles.TrackHistory            () 
## @see LoKi::Cuts::TRHISTFIT 
TRHISTFIT= LoKi.Particles.TrackHistoryFit         () 
## @see LoKi::Cuts::TRSTATUS  
TRSTATUS = LoKi.Particles.TrackStatus             () 
## @see LoKi::Cuts::FILTER    
FILTER   = LoKi.Particles.Filter
## @see LoKi::Cuts::DANG
DANG     = LoKi.Particles.CosineDirectionAngle 
## @see LoKi::Cuts::DIRA
DIRA     = LoKi.Particles.CosineDirectionAngle 
## @see LoKi::Cuts::DETA
DETA     = LoKi.Particles.DeltaEta
## @see LoKi::Cuts::DPHI
DPHI     = LoKi.Particles.DeltaPhi
## @see LoKi::Cuts::DELTAR2
DELTAR2  = LoKi.Particles.DeltaR2
## @see LoKi::Cuts::DR2
DR2      = LoKi.Particles.DeltaR2
## @see LoKi::Cuts::TRPCHI2 
TRPCHI2  = LoKi.Particles.TrackChi2Prob ()

## @see LoKi::Cuts::TRGHP
TRGHP        = LoKi.Particles.GhostProbability ()
## @see LoKi::Cuts::TRGHOSTPROB
TRGHOSTPROB  = TRGHP 

## @see LoKi::Cuts::TRLH
TRLH         = LoKi.Particles.TrackLikelihood ()
## @see LoKi::Cuts::TRGHOSTPROB
TRLIKELIHOOD = TRLH 


## @see LoKi::Cuts::TRGPOINTING
TRGPOINTING     = LoKi.Particles.TrgPointingScore 

## @see LoKi::Cuts::QPT 
QPT      = LoKi.Particles.TransverseMomentumQ 
## @see LoKi::Cuts::QPT1 
QPT1     = LoKi.Particles.TransverseMomentumQ ( 1 ) 
## @see LoKi::Cuts::QPT2
QPT2     = LoKi.Particles.TransverseMomentumQ ( 2 ) 


## @see LoKi::Cuts::Y
Y        = LoKi.Particles.Rapidity  ()
## @see LoKi::Cuts::Y0
Y0       = LoKi.Particles.Rapidity0 ()


# cosine of the decay  angle
LV0    = LoKi.Particles.DecayAngle
# cosine of the decay  angle for the first  daughter 
LV01   = LoKi.Particles.DecayAngle ( 1 )
# cosine of the decay  angle for the second daughter 
LV02   = LoKi.Particles.DecayAngle ( 2 )
# cosine of the decay  angle for the third  daughter 
LV03   = LoKi.Particles.DecayAngle ( 3 )
# cosine of the decay  angle for the fourth daughter 
LV04   = LoKi.Particles.DecayAngle ( 4 )


# meta-functions requested by Patrick KOPPENBURG:
## @see LoKi::Cuts::INGENERATION 
INGENERATION  = LoKi.Particles.InGeneration
## @see LoKi::Cuts::NINGENERATION 
NINGENERATION = LoKi.Particles.NinGeneration

## @see LoKi::Cuts::WM 
WM      = LoKi.Particles.WrongMass
## @see LoKi::Cuts::WMASS 
WMASS   = LoKi.Particles.WrongMass
## @see LoKi::Cuts::DWMASS 
DWMASS  = LoKi.Particles.DeltaWrongMass 
## @see LoKi::Cuts::DWM
DWM     = LoKi.Particles.DeltaWrongMass 
## @see LoKi::Cuts::ADWMASS 
ADWMASS = LoKi.Particles.AbsDeltaWrongMass 
## @see LoKi::Cuts::ADWM
ADWM    = LoKi.Particles.AbsDeltaWrongMass 


## @see LoKi::Cuts::LIFETIME
LIFETIME         = LoKi.Particles.LifeTime
## @see LoKi::Cuts::LIFETIMECHI2
LIFETIMECHI2     = LoKi.Particles.LifeTimeChi2
## @see LoKi::Cuts::LIFETIMESIGNCHI2
LIFETIMESIGNCHI2 = LoKi.Particles.LifeTimeSignedChi2
## @see LoKi::Cuts::LIFETIMEFITCHI2
LIFETIMEFITCHI2  = LoKi.Particles.LifeTimeFitChi2

## @see LoKi::Cuts::LIFETIME
LTIME         = LoKi.Particles.LifeTime
## @see LoKi::Cuts::LIFETIMECHI2
LTIMECHI2     = LoKi.Particles.LifeTimeChi2
## @see LoKi::Cuts::LIFETIMESIGNCHI2
LTIMESIGNCHI2 = LoKi.Particles.LifeTimeSignedChi2
## @see LoKi::Cuts::LIFETIMEFITCHI2
LTIMEFITCHI2  = LoKi.Particles.LifeTimeFitChi2

## @see LoKi::Cuts::LIFETIMECHI2
LTCHI2     = LoKi.Particles.LifeTimeChi2
## @see LoKi::Cuts::LIFETIMESIGNCHI2
LTSIGNCHI2 = LoKi.Particles.LifeTimeSignedChi2
## @see LoKi::Cuts::LIFETIMEFITCHI2
LTFITCHI2  = LoKi.Particles.LifeTimeFitChi2
## @see LoKi::Cuts::LTIMEERR
LTIMEERR      = LoKi.Particles.LifeTimeError

## @see LoKi::Cuts::REFIT_
REFIT_        = LoKi.Particles.ReFit
## @see LoKi::Cuts::MFIT_
MFIT_         = LoKi.Particles.MassFit 


## @see LoKi::Cuts::VTRUE
VTRUE     = LoKi.Constant( _RCV , bool )(True)
## @see LoKi::Cuts::VFALSE
VFALSE    = LoKi.Constant( _RCV , bool )(False)
## @see LoKi::Cuts::VALL
VALL      = VTRUE
## @see LoKi::Cuts::VNONE
VNONE     = VFALSE
## @see LoKi::Cuts::VONE
VONE      = LoKi.Constant     ( _RCV , 'double' )(1.0)
## @see LoKi::Cuts::VZERO
VZERO     = LoKi.Constant     ( _RCV , 'double' )(0.0)
## @see LoKi::Cuts::VVALID
VVALID    = LoKi.Valid        ( _RCV )()
## @see LoKi::Cuts::VSAME
VSAME     = LoKi.TheSame      ( _RCV )
## @see LoKi::Cuts::PRIMARY
PRIMARY   = LoKi.Vertices.IsPrimary   ()
## @see LoKi::Cuts::ISPRIMARY
ISPRIMARY =  PRIMARY
## @see LoKi::Cuts::TECHNIQUE
TECHNIQUE = LoKi.Vertices.Technique   ()
## @see LoKi::Cuts::VTECHNIQUE
VTECHNIQUE = TECHNIQUE
## @see LoKi::Cuts::VCHI2
VCHI2     = LoKi.Vertices.VertexChi2        ()
## @see LoKi::Cuts::VCHI2PDOF
VCHI2PDOF = LoKi.Vertices.VertexChi2PerDoF  ()
## @see LoKi::Cuts::VDOF
VDOF      = LoKi.Vertices.VertexDoF  ()
## @see LoKi::Cuts::VXNDOF
VXNDOF    =  VDOF 
## @see LoKi::Cuts::VX 
VX        = LoKi.Vertices.PositionX  ()
## @see LoKi::Cuts::VY
VY        = LoKi.Vertices.PositionY  ()
## @see LoKi::Cuts::VZ 
VZ        = LoKi.Vertices.PositionZ  ()
## @see LoKi::Cuts::VHASINFO
VHASINFO  = LoKi.Vertices.HasInfo
## @see LoKi::Cuts::VINFO
VINFO     = LoKi.Vertices.Info
## @see LoKi::Cuts::NTRACKS
NTRACKS   = LoKi.Vertices.NumberOfTracks   ()
## @see LoKi::Cuts::NPRONGS 
NPRONGS   = LoKi.Vertices.NumberOfOutgoing ()
## @see LoKi::Cuts::VPCHI2
VPCHI2    = LoKi.Vertices.Chi2Prob()
## @see LoKi::Cuts::VVDIST
VVDIST    = LoKi.Vertices.VertexDistance 
## @see LoKi::Cuts::VVD
VVD       = VVDIST
## @see LoKi::Cuts::VVDSIGN
VVDSIGN   = LoKi.Vertices.VertexSignedDistance 
## @see LoKi::Cuts::VVDCHI2 
VVDCHI2   = LoKi.Vertices.VertexChi2Distance 
## @see LoKi::Cuts::MINVVD
MINVVD    = LoKi.Vertices.MinVertexDistance 
## @see LoKi::Cuts::MINVVDCHI2
MINVVDCHI2 = LoKi.Vertices.MinVertexChi2Distance 
## @see LoKi::Vertices::ImpPar
VIP        = LoKi.Vertices.ImpPar
## @see LoKi::Vertices::ImpParChi2
VIPCHI2    = LoKi.Vertices.ImpParChi2
## @see LoKi::Cuts::VDZ
VDZ        = LoKi.Vertices.VertexDeltaZ 
## @see LoKi::Cuts::VABSDZ
VABSDZ     = LoKi.Vertices.VertexAbsDeltaZ 

# =============================================================================
# Aliases from Vava GLIGOROV
# =============================================================================
CLOSESTAPPROACH         = CLAPP
CLOSESTAPPROACHCHI2     = CLAPPCHI2 
COSPF                   = DIRA
DPHI2DETA2              = DR2
PDGID                   = ID
MEASUREDMASS            = MM
MINCLOSESTAPPROACH      = MINCLAPP
MINCLOSESTAPPROACHCHI2  = MINCLAPPCHI2 
MINIPCHI2               = MIPCHI2
NDAUGHTERS              = NDAUGS
VERTEXSEP               = VD
VERTEXSEPCHI2           = VDCHI2
VERTEXSEPSIGN           = VDSIGN
APPLYVERTEXFUNTOENDVERT = VFASPF 

# =============================================================================
# CONTEXT-DEPENDENT FUNCTORS
# =============================================================================
## @see LoKi::Cuts::BPV 
BPV    = LoKi.Particles.BestPrimaryVertexAdaptor 
# =============================================================================
BPVDIRA               = LoKi.Particles.CosineDirectionAngleWithTheBestPV()
BESTPVCOSPF           = LoKi.Particles.CosineDirectionAngleWithTheBestPV() ## by Vava GLIGOROV

BPVIP                 = LoKi.Particles.ImpParWithTheBestPV
BESTPVIP              = LoKi.Particles.ImpParWithTheBestPV                ## by Vava GLIGOROV

BPVIPCHI2             = LoKi.Particles.ImpParChi2WithTheBestPV
BESTPVIPCHI2          = LoKi.Particles.ImpParChi2WithTheBestPV            ## by Vava GLIGOROV

MIPSOURCE             = LoKi.Particles.MinImpParWithSource
MINIPWITHSOURCE       = LoKi.Particles.MinImpParWithSource                ## by Vava GLIGOROV 

MIPDV                 = LoKi.Particles.MinImpParDV
MINIPWITHDV           = LoKi.Particles.MinImpParDV                        ## by Vava GLIGOROV 

MIPTES                = LoKi.Particles.MinImpParTES
MINIPWITHTES          = LoKi.Particles.MinImpParTES                       ## by Vava GLIGOROV 

MIPCHI2SOURSE         = LoKi.Particles.MinImpParChi2WithSource
MINIPCHI2WITHSOURSE   = LoKi.Particles.MinImpParChi2WithSource            ## by Vava GLIGOROV

MIPCHI2DV             = LoKi.Particles.MinImpParChi2DV
MINIPCHI2WITHDV       = LoKi.Particles.MinImpParChi2DV                    ## by Vava GLIGOROV

MIPCHI2TES            = LoKi.Particles.MinImpParChi2TES
MINIPCHI2WITHTES      = LoKi.Particles.MinImpParChi2TES                   ## by Vava GLIGOROV 

BPVVD                 = LoKi.Particles.VertexDistanceDV ()
BESTPVVERTSEP         = LoKi.Particles.VertexDistanceDV ()                ## by Vava GLIGOROV 

BPVVDSIGN             = LoKi.Particles.VertexSignedDistanceDV ()
BESTPVVERTSEPSIGN     = LoKi.Particles.VertexSignedDistanceDV ()          ## by Vava GLIGOROV 

BPVVDDOT              = LoKi.Particles.VertexDotDistanceDV  ()
BESTPVVERTSEPALONGMOM = LoKi.Particles.VertexDotDistanceDV  ()            ## by Vava GLIGOROV 


BPVVDCHI2             = LoKi.Particles.VertexChi2DistanceDV ()
BPVLTIME              = LoKi.Particles.LifeTimeDV           
BPVLT                 = LoKi.Particles.LifeTimeDV           
BPVLTCHI2             = LoKi.Particles.LifeTimeChi2DV
BPVLTSIGNCHI2         = LoKi.Particles.LifeTimeSignedChi2DV
BPVLTFITCHI2          = LoKi.Particles.LifeTimeFitChi2DV

# =============================================================================
# Functions for Lisa Dwyer 
# =============================================================================
BPVLTERR              = LoKi.Particles.LifeTimeErrorDV           


# =============================================================================
# Functions for Olivier Leroy
# =============================================================================
## @see LoKi::Cuts::REFIT
REFIT         = LoKi.Particles.ReFitter 
## @see LoKi::Cuts::REFITTER
REFITTER      = LoKi.Particles.ReFitter 
## @see LoKi::Cuts::MFIT
MFIT          = LoKi.Particles.MassFitter () 
## @see LoKi::Cuts::MFITTER
MFITTER       = LoKi.Particles.MassFitter
## @see LoKi::Cuts::MASSFIT
MASSFIT       = LoKi.Particles.MassFitter

# =============================================================================
# Functions for Jaap Panman 
# =============================================================================
## @see LoKi::Cuts::BPVDVDOCA
BPVDVDOCA     = LoKi.Particles.DHCoplanarity


# =============================================================================
# Functions for Kazu Karvalho Akiba
# =============================================================================
## @see LoKi::Cuts::DPCTAU
DPCTAU             = LoKi.Particles.DaughterParticleCTau 
## @see LoKi::Cuts::DPCTAUERR
DPCTAUERR          = LoKi.Particles.DaughterParticleCTauErr
## @see LoKi::Cuts::DPCTAUFITCHI2 
DPCTAUFITCHI2      = LoKi.Particles.DaughterParticleCTauFitChi2 
## @see LoKi::Cuts::DPCTAUSIGNIFICANCE
DPCTAUSIGNIFICANCE = LoKi.Particles.DaughterParticleCTauSignificance


# =============================================================================
## nodes & trees 
# =============================================================================

## @see LoKi::Cuts::DECNODE 
DECNODE = LoKi.Particles.DecNode 
## @see LoKi::Cuts::DECTREE
DECTREE = LoKi.Particles.DecTree

## 
## charged lepton:
import PartProp.Nodes 
PELL    = DECNODE ( PartProp.Nodes.Ell    , True )
PLEPTON = DECNODE ( PartProp.Nodes.Lepton , True )
PHADRON = DECNODE ( PartProp.Nodes.Hadron , True )
PMESON  = DECNODE ( PartProp.Nodes.Meson  , True )
PBARYON = DECNODE ( PartProp.Nodes.Baryon , True )
PBEAUTY = DECNODE ( PartProp.Nodes.Beauty , True )
PCHARM  = DECNODE ( PartProp.Nodes.Charm  , True )

# =============================================================================
# P->VV angles 
# =============================================================================
## @see LoKi::Cuts::COSPOL
COSPOL     = LoKi.Particles.PolarizationAngle
## @see LoKi::Cuts::SINCHI
SINCHI     = LoKi.Particles.SinChi
## @see LoKi::Cuts::COSCHI
COSCHI     = LoKi.Particles.CosChi
## @see LoKi::Cuts::ANGLECHI
ANGLECHI   = LoKi.Particles.AngleChi
## @see LoKi::Cuts::COSTHETATR
COSTHETATR = LoKi.Particles.CosThetaTr
## @see LoKi::Cuts::SINPHITR
SINPHITR   = LoKi.Particles.SinPhiTr 
## @see LoKi::Cuts::COSPHITR
COSPHITR   = LoKi.Particles.CosPhiTr 
## @see LoKi::Cuts::ANGLEPHITR
ANGLEPHITR = LoKi.Particles.AnglePhiTr 


# vertex separation (for particles) with respect to *all* vertices

MINVDSOURCE           = LoKi.Particles.MinVertexDistanceWithSource
MINVDDV               = LoKi.Particles.MinVertexDistanceDV
MINVDTES              = LoKi.Particles.MinVertexDistanceTES

MINVDCHI2SOURCE       = LoKi.Particles.MinVertexChi2DistanceWithSource
MINVDCHI2DV           = LoKi.Particles.MinVertexChi2DistanceDV
MINVDCHI2TES          = LoKi.Particles.MinVertexChi2DistanceTES

# vertex separation (for vertices) with respect to *all* vertices

VMINVDSOURCE          = LoKi.Vertices.MinVertexDistanceWithSource
VMINVDDV              = LoKi.Vertices.MinVertexDistanceDV
VMINVDTES             = LoKi.Vertices.MinVertexDistanceTES

VMINVDCHI2SOURCE      = LoKi.Vertices.MinVertexChi2DistanceWithSource
VMINVDCHI2DV          = LoKi.Vertices.MinVertexChi2DistanceDV
VMINVDCHI2TES         = LoKi.Vertices.MinVertexChi2DistanceTES

# functors rerquired by Rob Lambert:

## @see LoKi::Cuts::MAXDIRASOURCE 
MAXDIRASOURCE         = LoKi.Particles.MaxCosineDirectionAngleWithSource
## @see LoKi::Cuts::MAXDIRADV 
MAXDIRADV             = LoKi.Particles.MaxCosineDirectionAngleDV 
## @see LoKi::Cuts::MAXDIRATES
MAXDIRATES            = LoKi.Particles.MaxCosineDirectionAngleTES
## @see LoKi::Cuts::DOCA 
DOCA                  = LoKi.Particles.DOCA
## @see LoKi::Cuts::DOCACHI2  
DOCACHI2              = LoKi.Particles.DOCAChi2 
## @see LoKi::Cuts::DOCAMAX_
DOCAMAX_              = LoKi.Particles.DOCAMax
## @see LoKi::Cuts::DOCACHI2MAX_
DOCACHI2MAX_          = LoKi.Particles.DOCAChi2Max 
## @see LoKi::Cuts::DOCAMAX
DOCAMAX               = LoKi.Particles.DOCAMax     () 
## @see LoKi::Cuts::DOCACHI2MAX
DOCACHI2MAX           = LoKi.Particles.DOCAChi2Max ()


# the functors, required by Sean BRISBANE :

## @see LoKi::Cuts::BPVVDZ 
BPVVDZ                = LoKi.Particles.VertexZDistanceWithTheBestPV   ()
## @see LoKi::Cuts::BPVVDRHO 
BPVVDRHO              = LoKi.Particles.VertexRhoDistanceWithTheBestPV ()
## @see LoKi::Cuts::BPVVDR
BPVVDR                = LoKi.Particles.VertexRhoDistanceWithTheBestPV ()


ADPDGM       = LoKi.Particles.AbsDeltaNominalMass  ()
ADPDGMASS    = LoKi.Particles.AbsDeltaNominalMass  ()
DPDGM        = LoKi.Particles.DeltaNominalMass     ()
DPDGMASS     = LoKi.Particles.DeltaNominalMass     ()
CHI2PDGM     = LoKi.Particles.DeltaNominalMassChi2 ()
CHI2PDGMASS  = LoKi.Particles.DeltaNominalMassChi2 ()


## @see LoKi::Cuts::CHILDIP
CHILDIP     = LoKi.Particles.ChildIP
## @see LoKi::Cuts::CHILDIPCHI2
CHILDIPCHI2 = LoKi.Particles.ChildIPChi2


## @see LoKi::Cuts::D2DVVD 
D2DVVD          = LoKi.Particles.DaughterVertexDistance 
## @see LoKi::Cuts::D2DVVDCHI2  
D2DVVDCHI2      = LoKi.Particles.DaughterVertexDistanceChi2 
## @see LoKi::Cuts::D2DVVDCHI2SIGN  
D2DVVDCHI2SIGN  = LoKi.Particles.DaughterVertexDistanceSignedChi2 
## @see LoKi::Cuts::D2DVVDDOT
D2DVVDDOT       = LoKi.Particles.DaughterVertexDistanceDot 
## @see LoKi::Cuts::D2DVVDSIGN
D2DVVDSIGN      = LoKi.Particles.DaughterVertexSignedDistance

## @see LoKi::Cuts::BPVTRGPOINTING
BPVTRGPOINTING     = LoKi.Particles.TrgPointingScoreWithBestPV()


# DecayTreeFitter:

## @see LoKi::Cuts::DTF_FUN 
DTF_FUN              = LoKi.Particles.DecayTreeFitterFun
## @see LoKi::Cuts::DTF_CUT
DTF_CUT              = LoKi.Particles.DecayTreeFitterCut

## @see LoKi::Cuts::DTF_CHI2 
DTF_CHI2             = LoKi.Particles.DecayTreeFitChi2
## @see LoKi::Cuts::DTF_NDOF
DTF_NDOF             = LoKi.Particles.DecayTreeFitNDoF
## @see LoKi::Cuts::DTF_CHI2NDOF
DTF_CHI2NDOF         = LoKi.Particles.DecayTreeFitChi2NDoF
## @see LoKi::Cuts::DTF_PROB
DTF_PROB             = LoKi.Particles.DecayTreeFitProb

## @see LoKi::Cuts::DTF_CTAU
DTF_CTAU             = LoKi.Particles.ChildCTau
## @see LoKi::Cuts::DTF_CTAUERR
DTF_CTAUERR          = LoKi.Particles.ChildCTauErr
## @see LoKi::Cuts::DTF_CTAUSIGNIFICANCE 
DTF_CTAUSIGNIFICANCE = LoKi.Particles.ChildCTauSignificance 


# =============================================================================
## Decay length significance functors & related
# @thanks Wouter Hulsbergen
# @thanks Roel Aaij
# =============================================================================


## @see LoKi::Cuts::DLS
DLS                  = LoKi.Particles.DecayLengthSignificance
## @see LoKi::Cuts::BPVDLS
BPVDLS               = LoKi.Particles.DecayLengthSignificanceDV ()

## @see LoKi::Cuts::PATHDIST
PATHDIST             = LoKi.Particles.PathDistance 
## @see LoKi::Cuts::PATHDISTCHI2 
PATHDISTCHI2         = LoKi.Particles.PathDistanceChi2 
## @see LoKi::Cuts::PATHDISTSIGNIFICANCE
PATHDISTSIGNIFICANCE = LoKi.Particles.PathDistanceSignificance 
## @see LoKi::Cuts::PDS
PDS                  = LoKi.Particles.PathDistanceSignificance

## @see LoKi::Cuts::BPVPATHDIST
BPVPATHDIST          = LoKi.Particles.PathDistanceWithBestPV()
## @see LoKi::Cuts::BPVPATHDIST_
BPVPATHDIST_         = LoKi.Particles.PathDistanceWithBestPV
## @see LoKi::Cuts::BPVPATHDISTCHI2
BPVPATHDISTCHI2      = LoKi.Particles.PathDistanceChi2WithBestPV()
## @see LoKi::Cuts::BPVPATHDISTCHI2_
BPVPATHDISTCHI2_     = LoKi.Particles.PathDistanceChi2WithBestPV
## @see LoKi::Cuts::BPVPDS
BPVPDS               = LoKi.Particles.PathDistanceSignificanceWithBestPV()
## @see LoKi::Cuts::BPVPATHDISTCHI2_
BPVPDS_              = LoKi.Particles.PathDistanceSignificanceWithBestPV

## @see LoKi::Cuts::PROJDIST 
PROJDIST             = LoKi.Particles.ProjectedDistance
## @see LoKi::Cuts::PROJDS  
PROJDS               = LoKi.Particles.ProjectedDistanceSignificance

## @see LoKi::Cuts::BPVPROJDIST 
BPVPROJDIST          = LoKi.Particles.ProjectedDistanceWithBestPV ()
## @see LoKi::Cuts::BPVPROJDIST_ 
BPVPROJDIST_         = LoKi.Particles.ProjectedDistanceWithBestPV

## @see LoKi::Cuts::BPVPROJDS 
BPVPROJDS            = LoKi.Particles.ProjectedDistanceSignificanceWithBestPV ()
## @see LoKi::Cuts::BPVPROJDS_ 
BPVPROJDS_           = LoKi.Particles.ProjectedDistanceSignificanceWithBestPV


# =============================================================================
## Collection of functions for 'corrected mass' variable by Mike Williams, 
#  see <a href="http://indico.cern.ch/conferenceDisplay.py?confId=104542">
#  Mike's slides at Trigger meeting 28 September 2k+10 </a> and 
#   <a href="http://savannah.cern.ch/task/?17418">Savannah task #17418</a>
#  @thanks Mike Williams
#  @see LoKi::Cuts::PTFLIGHT 
#  @see LoKi::Cuts::BPVPTFLIGHT 
#  @see LoKi::Cuts::CORRM
#  @see LoKi::Cuts::BPVCORRM
# =============================================================================

## @see LoKi::Cuts::PTFLIGHT 
PTFLIGHT    = LoKi.Particles.PtFlight
## @see LoKi::Cuts::BPVPTFLIGHT 
BPVPTFLIGHT = LoKi.Particles.PtFlightWithBestVertex () 
## @see LoKi::Cuts::CORRM
CORRM       = LoKi.Particles.MCorrected 
## @see LoKi::Cuts::BPVCORRM
BPVCORRM    = LoKi.Particles.MCorrectedWithBestVertex ()  




SOURCE         = LoKi.Particles.SourceTES
SOURCEDESKTOP  = LoKi.Particles.SourceDesktop
VSOURCE        = LoKi.Vertices.SourceTES
VSOURCEDESKTOP = LoKi.Vertices.SourceDesktop


NUMBER         = LoKi.Particles.TESCounter
VNUMBER        = LoKi.Vertices.TESCounter


## adapters
## @see LoKi::Cuts::PCUTA 
PCUTA   = LoKi.Particles.PCutA
## @see LoKi::Cuts::PCUTV
PCUTV   = LoKi.Particles.PCutV
## @see LoKi::Cuts::PFUNA 
PFUNA   = LoKi.Particles.PFunA
## @see LoKi::Cuts::PFUNV
PFUNV   = LoKi.Particles.PFunV
## @see LoKi::Cuts::PPFUN
PPFUN   = LoKi.Particles.ProtoPFun
## @see LoKi::Cuts::PPCUT
PPCUT   = LoKi.Particles.ProtoPCut
## @see LoKi::Cuts::TRFUN
TRFUN   = LoKi.Particles.TrackFun
## @see LoKi::Cuts::TRCUT
TRCUT   = LoKi.Particles.TrackCut


## Legacy stuff
BPVVDS_LEGACY = LoKi.Legacy.DistanceSignificanceWithBestPV () 
TRGPOINTINGWPT = LoKi.Legacy.TrgPointingScoreWithPt
BPVTRGPOINTINGWPT = LoKi.Legacy.TrgPointingScoreWithPtWithBestPV ()


# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 

# =============================================================================
# The END 
# =============================================================================

