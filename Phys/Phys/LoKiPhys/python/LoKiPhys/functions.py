#!/usr/bin/env python
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
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiPhys library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators    as      _LoKiCore
from   LoKiCore.functions     import  equal_to 

# Namespaces:
_global  = _LoKiCore._global
cpp      = _global 
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
## All concrete types 
# =============================================================================

## @see LoKi::Cuts::TRUE
TRUE     = LoKi.Constant( _RCP , bool )(True)
## @see LoKi::Cuts::FALSE
FALSE    = LoKi.Constant( _RCP , bool )(False)
## @see LoKi::Cuts::ALL
ALL      = TRUE
## @see LoKi::Cuts::NONE
NONE     = FALSE
## @see LoKi::Cuts::ONE
ONE      = LoKi.Constant     ( _RCP , 'double' )(1.0)
## @see LoKi::Cuts::ZERO
ZERO     = LoKi.Constant     ( _RCP , 'double' )(0.0)
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
## @see LoKi::Cuts::ISMUON
ISMUON   = LoKi.Particles.IsMuon           ()
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
## @see LoKi::Cust::TRPCHI2 
TRPCHI2  = LoKi.Particles.TrackChi2Prob ()

# meta-functions requested by Patrick KOPPENBURG:
## @see LoKi::Cuts::INGENERATION 
INGENERATION  = LoKi.Particles.InGeneration
## @see LoKi::Cuts::NINGENERATION 
NINGENERATION = LoKi.Particles.NinGeneration



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


# the functors, required by Sean BRISBANE :

## @see LoKi::Cuts::BPVVDZ 
BPVVDZ                = LoKi.Particles.VertexZDistanceWithTheBestPV   ()
## @see LoKi::Cuts::BPVVDRHO 
BPVVDRHO              = LoKi.Particles.VertexRhoDistanceWithTheBestPV ()
## @see LoKi::Cuts::BPVVDR
BPVVDR                = LoKi.Particles.VertexRhoDistanceWithTheBestPV ()


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
#
Maps      = LoKi.Functor             ( _vp    , _vd      )
Map       = LoKi.FunctorFromFunctor  ( _vp    , _vd      )
Pipes     = LoKi.Functor             ( _vp    , _vp      )
Pipe      = LoKi.FunctorFromFunctor  ( _vp    , _vp      )
FunVals   = LoKi.Functor             ( _vp    , 'double' )
FunVal    = LoKi.FunctorFromFunctor  ( _vp    , 'double' )
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
VElements = LoKi.Functor             ( _vv , _RCV     ) 
VElement  = LoKi.FunctorFromFunctor  ( _vv , _RCV     ) 
VSources  = LoKi.Functor             ( 'void' , _vv   ) 
VSource   = LoKi.FunctorFromFunctor  ( 'void' , _vv   ) 


SOURCE         = LoKi.Particles.SourceTES
SOURCEDESKTOP  = LoKi.Particles.SourceDesktop
VSOURCE        = LoKi.Vertices.SourceTES
VSOURCEDESKTOP = LoKi.Vertices.SourceDesktop


# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        

# =============================================================================
# The END 
# =============================================================================

