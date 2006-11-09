# =============================================================================
# $Id: benderpcuts.py,v 1.2 2006-11-09 14:10:38 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/10/11 14:45:11  ibelyaev
#  few steps towards v6r0
#
# =============================================================================
""" This is helper module for decoration of LoKi particle functions/cuts  """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
## @file
#
#  This is helper module for decoration of LoKi particle functions/cuts 
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb
_P      = _LoKi.Particles 

# =============================================================================
_RCP = 'const LHCb::Particle*'
# =============================================================================

## @see LoKi::Cuts::TRUE
TRUE     = _LoKi.BooleanConstant( _RCP )(True)
## @see LoKi::Cuts::FALSE
FALSE    = _LoKi.BooleanConstant( _RCP )(False)
## @see LoKi::Cuts::ALL
ALL      = TRUE
## @see LoKi::Cuts::NONE
NONE     = FALSE
## @see LoKi::Cuts::ONE
ONE      = _LoKi.Constant     ( _RCP )(1.0)
## @see LoKi::Cuts::ZERO
ZERO     = _LoKi.Constant     ( _RCP )(0.0)
## @see LoKi::Cuts::MIN
MIN      = _LoKi.Min          ( _RCP )
## @see LoKi::Cuts::MIN
MAX      = _LoKi.Max          ( _RCP )
## @see LoKi::Cuts::SWITCH
SWITCH   = _LoKi.Switch       ( _RCP )
## @see LoKi::Cuts::SSWITCH
SSWITCH  = _LoKi.SimpleSwitch ( _RCP )
## @see LoKi::Cuts::VALID
VALID    = _LoKi.Valid        ( _RCP )()
## @see LoKi::Cuts::SAME
SAME     = _LoKi.TheSame      ( _RCP )
## @see LoKi::Cuts::EQUALTO
EQUALTO  = _LoKi.EqualToValue ( _RCP )
## @see LoKi::Cuts::EQ
EQ       = EQUALTO
## @see LoKi::Cuts::HASKEY
HASKEY   = _LoKi.Particles.HasKey   () 
## @see LoKi::Cuts::KEY
KEY      = _LoKi.Particles.Key      () 
## @see LoKi::Cuts::PKEY
PKEY     =  KEY
## @see LoKi::Cuts::INTES
INTES    = _LoKi.Particles.InTES       
## @see LoKi::Cuts::PINTES
PINTES   = INTES
## @see LoKi::Cuts::ID
ID       = _LoKi.Particles.Identifier ()
## @see LoKi::Cuts::ABSID
ABSID    = _LoKi.Particles.AbsIdentifier ()
## @see LoKi::Cuts::Q
Q        = _LoKi.Particles.Charge        ()
## @see LoKi::Cuts::SUMQ
SUMQ     = _LoKi.Particles.SumCharge     ()
## @see LoKi::Cuts::P   
P        = _LoKi.Particles.Momentum      ()
## @see LoKi::Cuts::P2  
P2       = _LoKi.Particles.Momentum2     ()
## @see LoKi::Cuts::E   
E        = _LoKi.Particles.Energy        ()
## @see LoKi::Cuts::P2  
PT       = _LoKi.Particles.TransverseMomentum () 
## @see LoKi::Cuts::PX  
PX       = _LoKi.Particles.MomentumX     ()
## @see LoKi::Cuts::PY  
PY       = _LoKi.Particles.MomentumY     ()
## @see LoKi::Cuts::PZ  
PZ       = _LoKi.Particles.MomentumZ     ()
## @see LoKi::Cuts::ETA 
ETA      = _LoKi.Particles.PseudoRapidity () 
## @see LoKi::Cuts::PHI 
PHI      = _LoKi.Particles.Phi            () 
## @see LoKi::Cuts::M   
M        = _LoKi.Particles.Mass           () 
## @see LoKi::Cuts::MM  
MM       = _LoKi.Particles.MeasuredMass   () 
## @see LoKi::Cuts::MASS 
MASS     = _LoKi.Particles.InvariantMass     
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
DMASS    = _LoKi.Particles.DeltaMass         
## @see LoKi::Cuts::ADMASS 
ADMASS   = _LoKi.Particles.AbsDeltaMass         
## @see LoKi::Cuts::DMMASS 
DMMASS   = _LoKi.Particles.DeltaMeasuredMass         
## @see LoKi::Cuts::ADMMASS 
ADMMASS  = _LoKi.Particles.AbsDeltaMeasuredMass         
## @see LoKi::Cuts::CHI2M
CHI2M    = _LoKi.Particles.DeltaMeasuredMassChi2     
## @see LoKi::Cuts::CHI2MASS
CHI2MASS = CHI2M
## @see LoKi::Cuts::CL  
CL       = _LoKi.Particles.ConfidenceLevel  () 
## @see LoKi::Cuts::WEIGHT
WEIGHT   = _LoKi.Particles.Weight           () 
## @see LoKi::Cuts::ISBASIC 
ISBASIC  = _LoKi.Particles.IsBasic          () 
## @see LoKi::Cuts::NDAUGS  
NDAUGS   = _LoKi.Particles.NumberOfDaughters  () 
## @see LoKi::Cuts::HASINFO
HASINFO  = _LoKi.Particles.HasInfo
## @see LoKi::Cuts::INFO
INFO     = _LoKi.Particles.Info
## @see LoKi::Cuts::HASPROTO
HASPROTO = _LoKi.Particles.HasProto         () 
## @see LoKi::Cuts::HASVERTEX
HASVERTEX= _LoKi.Particles.HasVertex        () 
## @see LoKi::Cuts::VD
VD       = _LoKi.Particles.VertexDistance   
## @see LoKi::Cuts::VDSIGN
VDSIGN   = _LoKi.Particles.VertexSignedDistance   
## @see LoKi::Cuts::VDDOT 
VDDOT    = _LoKi.Particles.VertexDotDistance   
## @see LoKi::Cuts::VDCHI2 
VDCHI2   = _LoKi.Particles.VertexChi2Distance   
## @see LoKi::Cuts::VDMIN
VDMIN    = _LoKi.Particles.MinVertexDistance   
## @see LoKi::Cuts::MINVD
MINVD    = VDMIN
## @see LoKi::Cuts::VDMINCHI2
VDMINCHI2= _LoKi.Particles.MinVertexChi2Distance   
## @see LoKi::Cuts::TIMEDIST
TIMEDIST = _LoKi.Particles.TimeDistance
## @see LoKi::Cuts::CTAU
CTAU     = TIMEDIST
## @see LoKi::Cuts::TDIST
TDIST    = TIMEDIST
## @see LoKi::Cuts::TD
TD       = TIMEDIST
## @see LoKi::Cuts::TSIGND   
TSIGND   = _LoKi.Particles.TimeSignedDistance
## @see LoKi::Cuts::TDSIGN    
TDSIGN   =  TSIGND 
## @see LoKi::Cuts::CTAUSIGN    
CTAUSIGN =  TSIGND 
## @see LoKi::Cuts::TDOT
TDOT     = _LoKi.Particles.TimeDotDistance
## @see LoKi::Cuts::CTAUDOT
CTAUDOT  = TDOT 
## @see LoKi::Cuts::CLAPP
CLAPP    = _LoKi.Particles.ClosestApproach 
## @see LoKi::Cuts::CLAPPCHI2
CLAPPCHI2= _LoKi.Particles.ClosestApproachChi2
## @see LoKi::Cuts::MINCLAPP
MINCLAPP = _LoKi.Particles.MinClosestApproach 
## @see LoKi::Cuts::MINCLAPPCHI2 
MINCLAPPCHI2 = _LoKi.Particles.MinClosestApproachChi2 
## @see LoKi::Cuts::IP
IP       = _LoKi.Particles.ImpPar
## @see LoKi::Cuts::IPCHI2 
IPCHI2   = _LoKi.Particles.ImpParChi2 
## @see LoKi::Cuts::IPMIN
IPMIN    = _LoKi.Particles.MinImpPar 
## @see LoKi::Cuts::MIP
MIP      =  IPMIN
## @see LoKi::Cuts::MINIP
MINIP    =  IPMIN
## @see LoKi::Cuts::MIPCHI2 
MIPCHI2  = _LoKi.Particles.MinImpParChi2 
## @see LoKi::Cuts::CHI2MIP 
CHI2MIP  = MIPCHI2
## @see LoKi::Cuts::INTREE
INTREE   = _LoKi.Particles.InTree
## @see LoKi::Cuts::NINTREE
NINTREE  = _LoKi.Particles.NinTree
## @see LoKi::Cuts::SUMTREE
SUMTREE  = _LoKi.Particles.SumTree
## @see LoKi::Cuts::MULTTREE
MULTTREE = _LoKi.Particles.MultTree
## @see LoKi::Cuts::MINTREE
MINTREE  = _LoKi.Particles.MinTree
## @see LoKi::Cuts::MAXTREE
MAXTREE  = _LoKi.Particles.MaxTree
## @see LoKi::Cuts::CHILD 
CHILD    = _LoKi.Particles.ChildFunction
## @see LoKi::Cuts::CHILDFUN
CHILDFUN = CHILD 
## @see LoKi::Cuts::CHILDCUT 
CHILDCUT = _LoKi.Particles.ChildPredicate
## @see LoKi::Cuts::VFASPF 
VFASPF   = _LoKi.Particles.VFunAsPFun
## @see LoKi::Cuts::HASPROTOS 
HASPROTOS= _LoKi.Particles.HasProtos 
## @see LoKi::Cuts::PROTOS
PROTOS   = HASPROTOS 
## @see LoKi::Cuts::HASPROTOSINTREE 
HASPROTOSINTREE = _LoKi.Particles.HasProtosInTree
## @see LoKi::Cuts::PROTOSINTREE
PROTOSINTREE = HASPROTOSINTREE 
## @see LoKi::Cuts::HASTRACKS
HASTRACKS = _LoKi.Particles.HasTracks 
## @see LoKi::Cuts::TRACKS
TRACKS    = HASTRACKS  
## @see LoKi::Cuts::HASTRACKSINTREE
HASTRACKSINTREE = _LoKi.Particles.HasTracksInTree
## @see LoKi::Cuts::TRACKSINTREE
TRACKSINTREE = HASTRACKSINTREE  
## @see LoKi::Cuts::PVTRACK
PVTRACK  = _LoKi.Particles.HasTracksFromPV
## @see LoKi::Cuts::TRACKFROMPV 
TRACKFROMPV = PVTRACK
## @see LoKi::Cuts::PVTRACKINTREE 
PVTRACKINTREE = _LoKi.Particles.HasTracksInTreeFromPV
## @see LoKi::Cuts::TRACKFROMPVINTREE 
TRACKFROMPVINTREE = PVTRACKINTREE 
## @see LoKi::Cuts::IS 
IS       = _LoKi.Particles.IsAParticle
## @see LoKi::Cuts::IN
IS       = IS
## @see LoKi::Cuts::ISINTREE 
ISINTREE = _LoKi.Particles.IsAParticleInTree 
## @see LoKi::Cuts::FROMTREE 
FROMTREE = _LoKi.Particles.IsAParticleInTree 
## @see LoKi::Cuts::FROM
FROM     = FROMTREE
## @see LoKi::Cuts::PPHASINFO
PPHASINFO= _LoKi.Particles.ProtoHasInfo
## @see LoKi::Cuts::PPINFO
PPINFO   = _LoKi.Particles.ProtoInfo
## @see LoKi::Cuts::PIDe
PIDe     = PPINFO( _LHCb.ProtoParticle.CombDLLe  , 0 , -1000 )
## @see LoKi::Cuts::DLLe 
DLLe     = PIDe
## @see LoKi::Cuts::PIDmu
PIDmu    = PPINFO( _LHCb.ProtoParticle.CombDLLmu , 0 , -1000 )
## @see LoKi::Cuts::DLLmu
DLLmu    = PIDmu
## @see LoKi::Cuts::PIDpi
PIDpi    = PPINFO( _LHCb.ProtoParticle.CombDLLpi , 0 , -1000 )
## @see LoKi::Cuts::DLLmu
DLLpi    = PIDpi
## @see LoKi::Cuts::PIDK
PIDK     = PPINFO( _LHCb.ProtoParticle.CombDLLk  , 0 , -1000 )
## @see LoKi::Cuts::DLLK  
DLLK     = PIDK  
## @see LoKi::Cuts::PIDp
PIDp     = PPINFO( _LHCb.ProtoParticle.CombDLLp  , 0 , -1000 )
## @see LoKi::Cuts::DLLp  
DLLp     = PIDp 
## @see LoKi::Cuts::PPHASRICH
PPHASRICH= _LoKi.Particles.ProtoHasRichPID     ()
## @see LoKi::Cuts::HASRICH
HASRICH  = PPHASRICH 
## @see LoKi::Cuts::PPHASMUON
PPHASMUON= _LoKi.Particles.ProtoHasMuonPID     () 
## @see LoKi::Cuts::HASMUON
HASMUON  = PPHASMUON
## @see LoKi::Cuts::PPHASCALOS
PPHASCALOS = _LoKi.Particles.ProtoHasCaloHypos () 
## @see LoKi::Cuts::HASCALOS
HASCALOS = PPHASCALOS 
## @see LoKi::Cuts::TRCHI2 
TRCHI2   = _LoKi.Particles.TrackChi2           ()
## @see LoKi::Cuts::TRCHI2DOF 
TRCHI2DOF= _LoKi.Particles.TrackChi2PerDoF     () 
## @see LoKi::Cuts::HASSTATE  
HASSTATE = _LoKi.Particles.TrackHasState 
## @see LoKi::Cuts::TRTYPE 
TRTYPE   = _LoKi.Particles.TrackType           () 
## @see LoKi::Cuts::ISLONG
ISLONG   = EQUALTO( TRTYPE , float(_LHCb.Track.Long)       )
## @see LoKi::Cuts::ISUP   
ISUP     = EQUALTO( TRTYPE , float(_LHCb.Track.Upstream)   ) 
## @see LoKi::Cuts::ISDOWN
ISDOWN   = EQUALTO( TRTYPE , float(_LHCb.Track.Downstream) ) 
## @see LoKi::Cuts::ISTTRACK
ISTTRACK = EQUALTO( TRTYPE , float(_LHCb.Track.Ttrack)     ) 
## @see LoKi::Cuts::TRHISTORY
TRHISTORY= _LoKi.Particles.TrackHistory            () 
## @see LoKi::Cuts::TRHISTFIT 
TRHISTFIT= _LoKi.Particles.TrackHistoryFit         () 
## @see LoKi::Cuts::TRSTATUS  
TRSTATUS = _LoKi.Particles.TrackStatus             () 
## @see LoKi::Cuts::FILTER    
FILTER   = _LoKi.Particles.Filter                      
## @see LoKi::Cuts::MCTRUTH 
MCTRUTH  = _LoKi.Particles.MCTruth



if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 
