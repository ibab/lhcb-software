#!/usr/bin/env python
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2007/06/01 12:07:03  ibelyaev
#  prepare for version v4r0
#
# =============================================================================
## @file functions.py LoKiPhys/functions.py
#  The set of basic decorator for objects from LoKiPhys library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic decorators for obejcts from LoKiPhys library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore

# Namespaces:
_global  = _LoKiCore._global 
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _LoKiCore.LHCb

_RCP = 'const LHCb::Particle*'
_RCV = 'const LHCb::VertexBase*'

_c1 = std.vector( _RCP )
_c2 = std.vector( _RCV )

## ranges
Range   = LoKi.NamedRange_ ( _c1    )
VRange  = LoKi.NamedRange_ ( _c2    )
## range-lists: 
RangeL  = LoKi.RangeList_  (  Range )
VRangeL = LoKi.RangeList_  ( VRange )


# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::Func
Func  = LoKi.Function               ( _RCP ) 
## @see LoKi::Types::Cuts
Cuts  = LoKi.Predicate              ( _RCP ) 
## @see LoKi::Types::Fun
Fun   = LoKi.FunctionFromFunction   ( _RCP ) 
## @see LoKi::Types::Cut
Cut   = LoKi.PredicateFromPredicate ( _RCP ) 
## @see LoKi::Types::Func
VFunc = LoKi.Function               ( _RCV ) 
## @see LoKi::Types::Cuts
VCuts = LoKi.Predicate              ( _RCV ) 
## @see LoKi::Types::Fun
VFun  = LoKi.FunctionFromFunction   ( _RCV ) 
## @see LoKi::Types::Cut
VCut  = LoKi.PredicateFromPredicate ( _RCV ) 

# =============================================================================
## All concrete types 
# =============================================================================

## @see LoKi::Cuts::TRUE
TRUE     = LoKi.BooleanConstant( _RCP )(True)
## @see LoKi::Cuts::FALSE
FALSE    = LoKi.BooleanConstant( _RCP )(False)
## @see LoKi::Cuts::ALL
ALL      = TRUE
## @see LoKi::Cuts::NONE
NONE     = FALSE
## @see LoKi::Cuts::ONE
ONE      = LoKi.Constant     ( _RCP )(1.0)
## @see LoKi::Cuts::ZERO
ZERO     = LoKi.Constant     ( _RCP )(0.0)
## @see LoKi::Cuts::MIN
MIN      = LoKi.Min          ( _RCP )
## @see LoKi::Cuts::MIN
MAX      = LoKi.Max          ( _RCP )
## @see LoKi::Cuts::SWITCH
SWITCH   = LoKi.Switch       ( _RCP )
## @see LoKi::Cuts::SSWITCH
SSWITCH  = LoKi.SimpleSwitch ( _RCP )
## @see LoKi::Cuts::VALID
VALID    = LoKi.Valid        ( _RCP )()
## @see LoKi::Cuts::SAME
SAME     = LoKi.TheSame      ( _RCP )
## @see LoKi::Cuts::EQUALTO
## EQUALTO  = LoKi.EqualToValue ( _RCP )
## @see LoKi::Cuts::EQ
## EQ       = EQUALTO
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
## @see LoKi::Cuts::M12  
M12      = MASS(1,2)
## @see LoKi::Cuts::M13  
M13      = MASS(1,3)
## @see LoKi::Cuts::M14  
M14      = MASS(1,4)
## @see LoKi::Cuts::M23  
M23      = MASS(2,3)
## @see LoKi::Cuts::M24  
M124     = MASS(2,4)
## @see LoKi::Cuts::M34  
M34      = MASS(3,4)
## @see LoKi::Cuts::DMASS 
DMASS    = LoKi.Particles.DeltaMass         
## @see LoKi::Cuts::ADMASS 
ADMASS   = LoKi.Particles.AbsDeltaMass         
## @see LoKi::Cuts::DMMASS 
DMMASS   = LoKi.Particles.DeltaMeasuredMass         
## @see LoKi::Cuts::ADMMASS 
ADMMASS  = LoKi.Particles.AbsDeltaMeasuredMass         
## @see LoKi::Cuts::CHI2M
CHI2M    = LoKi.Particles.DeltaMeasuredMassChi2
## @see LoKi::Cuts::CHI2MASS
CHI2MASS = CHI2M
## @see LoKi::Cuts::CL  
CL       = LoKi.Particles.ConfidenceLevel  () 
## @see LoKi::Cuts::WEIGHT
WEIGHT   = LoKi.Particles.Weight           () 
## @see LoKi::Cuts::ISBASIC 
ISBASIC  = LoKi.Particles.IsBasic          () 
## @see LoKi::Cuts::NDAUGS  
NDAUGS   = LoKi.Particles.NumberOfDaughters  () 
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
## @see LoKi::Cuts::TRCHI2 
TRCHI2   = LoKi.Particles.TrackChi2           ()
## @see LoKi::Cuts::TRCHI2DOF 
TRCHI2DOF= LoKi.Particles.TrackChi2PerDoF     () 
## @see LoKi::Cuts::HASSTATE  
HASSTATE = LoKi.Particles.TrackHasState 
## @see LoKi::Cuts::TRTYPE 
TRTYPE   = LoKi.Particles.TrackType           () 
## @see LoKi::Cuts::ISLONG
##ISLONG   = EQUALTO( TRTYPE , float(LHCb.Track.Long)       )
## @see LoKi::Cuts::ISUP   
##ISUP     = EQUALTO( TRTYPE , float(LHCb.Track.Upstream)   ) 
## @see LoKi::Cuts::ISDOWN
##ISDOWN   = EQUALTO( TRTYPE , float(LHCb.Track.Downstream) ) 
## @see LoKi::Cuts::ISTTRACK
##ISTTRACK = EQUALTO( TRTYPE , float(LHCb.Track.Ttrack)     ) 
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

## @see LoKi::Cuts::VTRUE
VTRUE     = LoKi.BooleanConstant( _RCV )(True)
## @see LoKi::Cuts::VFALSE
VFALSE    = LoKi.BooleanConstant( _RCV )(False)
## @see LoKi::Cuts::VALL
VALL      = VTRUE
## @see LoKi::Cuts::VNONE
VNONE     = VFALSE
## @see LoKi::Cuts::VONE
VONE      = LoKi.Constant     ( _RCV )(1.0)
## @see LoKi::Cuts::VZERO
VZERO     = LoKi.Constant     ( _RCV )(0.0)
## @see LoKi::Cuts::VMIN
VMIN      = LoKi.Min          ( _RCV )
## @see LoKi::Cuts::VMIN
VMAX      = LoKi.Max          ( _RCV )
## @see LoKi::Cuts::VSWITCH
VSWITCH   = LoKi.Switch       ( _RCV )
## @see LoKi::Cuts::VSSWITCH
VSSWITCH  = LoKi.SimpleSwitch ( _RCV )
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
VCHI2     = LoKi.Vertices.VertexChi2  ()
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

_PhysChild = LoKi.Child.PhysChild 

# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        

# =============================================================================
# The END 
# =============================================================================

