#!/usr/bin/env python
# =============================================================================
# $Id: benderparticlecuts.py,v 1.7 2005-02-02 19:15:10 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
__author__ = "Vanya BELYAEV belyaev@lapp.in2p3.fr"
# =============================================================================
# @file
#
# defintion of all LoKii/Bender Particle functions/cuts 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule        as      gaudi
import benderhelper       as      helper

# global namespace  
gbl = gaudi.gbl 

# load the minimal set of dictionaries
gaudi.loaddict('EventDict')
gaudi.loaddict('PhysEventDict')
gaudi.loaddict('BenderDict')

# interface functions, load & add the operators!!!
Func     = helper.loadFunP ( gbl.LoKi.Function               ( 'const Particle*' ) ) 
# interface functions, load & add the operators!!!
Cuts     = helper.loadCutP ( gbl.LoKi.Predicate              ( 'const Particle*' ) )
# load & add the operators 
Fun      = helper.loadFunP ( gbl.LoKi.FunctionFromFunction   ( 'const Particle*' ) )
# load & add the operators 
Cut      = helper.loadCutP ( gbl.LoKi.PredicateFromPredicate ( 'const Particle*' ) ) 
# load & add the operators 
PCONST   = helper.loadFunP ( gbl.LoKi.Constant               ( 'const Particle*' ) )
# load & add the operators 
PBCONST  = helper.loadCutP ( gbl.LoKi.BooleanConstant        ( 'const Particle*' ) )

# see table 13.2 in LoKi manual
ONE        = PCONST  ( 1.0 )
PONE       = ONE
PZERO      = PCONST  ( 0.0 )
PALL       = PBCONST ( 1   )
PTRUE      = PALL
PNONE      = PBCONST ( 0   )
PFALSE     = PNONE 

PMIN       = helper.loadFunP ( gbl.LoKi.Min ( 'const Particle*' ) )
PMAX       = helper.loadFunP ( gbl.LoKi.Max ( 'const Particle*' ) )
MIN        = PMIN
MAX        = PMAX

# functions 
ID    =  helper.loadFunID    ( gbl.LoKi.Particles.Identifier         ) () 
ABSID =  helper.loadFunABSID ( gbl.LoKi.Particles.AbsIdentifier      ) () 
CL    =  helper.loadFunP     ( gbl.LoKi.Particles.ConfidenceLevel    ) () 
Q     =  helper.loadFunP     ( gbl.LoKi.Particles.Charge             ) ()      
SUMQ  =  helper.loadFunP     ( gbl.LoKi.Particles.SumCharge          ) ()      
P     =  helper.loadFunP     ( gbl.LoKi.Particles.Momentum           ) ()  
P2    =  helper.loadFunP     ( gbl.LoKi.Particles.Momentum2          ) ()  
PT    =  helper.loadFunP     ( gbl.LoKi.Particles.TransverseMomentum ) ()  
PX    =  helper.loadFunP     ( gbl.LoKi.Particles.MomentumX          ) () 
PY    =  helper.loadFunP     ( gbl.LoKi.Particles.MomentumY          ) ()  
PZ    =  helper.loadFunP     ( gbl.LoKi.Particles.MomentumZ          ) () 
E     =  helper.loadFunP     ( gbl.LoKi.Particles.Energy             ) () 
M     =  helper.loadFunP     ( gbl.LoKi.Particles.Mass               ) () 
MM    =  helper.loadFunP     ( gbl.LoKi.Particles.MeasuredMass       ) ()

# functions 
DMASS    =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMass             ) 
DMMASS   =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMeasuredMass     )
ADMASS   =  helper.loadFunP ( gbl.LoKi.Particles.AbsDeltaMass          ) 
ADMMASS  =  helper.loadFunP ( gbl.LoKi.Particles.AbsDeltaMeasuredMass  ) 
CHI2M    =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMeasuredMassChi2 ) 
MCHI2    =  CHI2M
MASSCHI2 =  CHI2M
CHI2MASS =  CHI2M

# function
MASS =  helper.loadFunP ( gbl.LoKi.Particles.InvariantMass )
# objects 
M12  =  MASS ( 1 , 2     )
M13  =  MASS ( 1 , 3     )
M14  =  MASS ( 1 , 4     )
M23  =  MASS ( 2 , 3     )
M24  =  MASS ( 2 , 4     )
M34  =  MASS ( 3 , 4     )
M123 =  MASS ( 1 , 2 , 3 )
M124 =  MASS ( 1 , 2 , 4 )
M134 =  MASS ( 1 , 3 , 4 )
M234 =  MASS ( 2 , 3 , 4 )

#functions 
LV0  = helper.loadFunP( gbl.LoKi.Particles.RestFrameAngle )
# objects 
LV01 =  LV0  ( 1 )
LV02 =  LV0  ( 2 )
LV03 =  LV0  ( 3 )
LV04 =  LV0  ( 4 )

# function
PID   = helper.loadFunP ( gbl.LoKi.Particles.ParticleIdEstimator )
# see  $PHYSEVENTROOT/Event/ProtoParticle.h
## enum detectorPID
## { NoPID=0, 
##   RichElectron,  //  1
##   RichMuon,      //  2
##   RichPion,      //  3 
##   RichKaon,      //  4 
##   RichProton,    //  5 
##   MuonMuon,      //  6 
##   CaloTrMatch,   //  7 
##   CaloDepositID, //  8 
##   ClusterMass,   //  9 
##   ShowerShape,   // 10 
##   CaloEMatch,    // 11 
##   BremMatch,     // 12 
##   ECalPIDe,      // 13 
##   PrsPIDe,       // 14
##   BremPIDe,      // 15
##   HcalPIDe,      // 16 
##   HcalPIDmu,     // 17 
##   EcalPIDmu,     // 18 
##   MuonPIDmu,     // 19 
##   RichPIDe,      // 20
##   RichPIDmu,     // 21 
##   RichPIDpi,     // 22 
##   RichPIDK,      // 23 
##   RichPIDp,      // 24 
##   LkhPIDe,       // 25 
##   LkhPIDmu,      // 26 
##   LkhPIDpi,      // 27 
##   LkhPIDK,       // 28 
##   LkhPIDp,       // 29 
##   PhotonID       // 30
## };
PIDe  = PID ( 25 )
PIDmu = PID ( 26 )
PIDpi = PID ( 27 )
PIDK  = PID ( 28 )
PIDp  = PID ( 29 ) 

# object 
TCHI2NDF  = helper.loadFunP ( gbl.LoKi.Particles.TrackChi2PerDoF ) ()
TRCHI2NDF = TCHI2NDF
TLASTCHI2 = helper.loadFunP ( gbl.LoKi.Particles.TrackLastChi2   ) ()
TRCHI2    = TLASTCHI2
TRDOF     = helper.loadFunP ( gbl.LoKi.Particles.TrackDoF        ) ()


# funstions 
IP       = helper.loadFunP ( gbl.LoKi.Vertices.ImpPar     ) 
MIP      = helper.loadFunP ( gbl.LoKi.Vertices.MinImpPar     ) 
IPCHI2   = helper.loadFunP ( gbl.LoKi.Vertices.ImpParChi2 )
CHI2IP   = IPCHI2
MIPCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.MinImpParChi2 )
CHI2MIP  = MIPCHI2


VD       = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistance     ) 
VDCHI2   = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistanceChi2 )
CHI2VD   = VDCHI2

MVDCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.MinChi2Distance )
CHI2MVD  = MVDCHI2

VDDOT    = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistanceDot  ) 

VDSIGN   = helper.loadFunP ( gbl.LoKi.Vertices.SignedVertexDistance ) 
VDTIME   = helper.loadFunP ( gbl.LoKi.Vertices.SignedTimeDistance   ) 
VDDTIME  = helper.loadFunP ( gbl.LoKi.Vertices.DotTimeDistance      ) 

PLTIME    = helper.loadFunP ( gbl.LoKi.Particles.LifeTime      )
PLTERR    = helper.loadFunP ( gbl.LoKi.Particles.LifeTimeError )
PLTCHI2   = helper.loadFunP ( gbl.LoKi.Particles.LifeTimeChi2  )

DTR      = helper.loadFunP ( gbl.LoKi.Vertices.ClosestApproach      ) 
DTRCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.ClosestApproachChi2  )
CHI2DTR  = DTRCHI2

DIRA     = helper.loadFunP ( gbl.LoKi.Vertices.DirectionAngle )
DDANG    = DIRA

# functions
CHILD     = helper.loadFunP ( gbl.LoKi.Particles.ChildFunction ) 
VXFUN     = helper.loadFunP ( gbl.LoKi.Adapters.VFuncAsFun     ) 
FILTER    = helper.loadCutP ( gbl.LoKi.Adapters.FilterAsCut    ) 

FROMTREE  = helper.loadCutP ( gbl.LoKi.Particles.FromDecayTree ) 
NOOVERLAP = helper.loadCutP ( gbl.LoKi.Particles.NoOverlap     ) 

# functions 
TRFLAG    = helper.loadCutP ( gbl.LoKi.Particles.TrackFlags    ) 
TRALLFLAG = helper.loadCutP ( gbl.LoKi.Particles.AllTrackFlags )
# see $LOKIROOT/LoKi/Tracks.h
TRISLONG   = TRFLAG (   1 )
TRISUP     = TRFLAG (   2 )
TRISDOWN   = TRFLAG (   3 )
TRISVELO   = TRFLAG (   4 )
TRISBACK   = TRFLAG (   5 )
TRIST      = TRFLAG (   6 )
# 
TRUNIQUE   = TRFLAG ( 101 )
TRVELO     = TRFLAG ( 102 )
TRSEED     = TRFLAG ( 103 )
TRMATCH    = TRFLAG ( 104 )
TRFORWARD  = TRFLAG ( 105 )
TRFOLLOW   = TRFLAG ( 106 )
TRVELOTT   = TRFLAG ( 107 )
TRVELOBACK = TRFLAG ( 108 )
TRKSTRACK  = TRFLAG ( 109 )

MINTREE   = helper.loadFunP ( gbl.LoKi.TreeFunctions.MinTree       )
MAXTREE   = helper.loadFunP ( gbl.LoKi.TreeFunctions.MaxTree       )
TREEMULT  = helper.loadFunP ( gbl.LoKi.TreeFunctions.AccTreeMult   )
TREESUM   = helper.loadFunP ( gbl.LoKi.TreeFunctions.AccTreeSum    )
NINTREE   = helper.loadFunP ( gbl.LoKi.TreeFunctions.NinTree       )
INTREE    = helper.loadCutP ( gbl.LoKi.TreeFunctions.InTree        )


HASORIGIN = helper.loadCutP ( gbl.LoKi.Particles.HasOrigin         ) () 
HASPROTOP = helper.loadCutP ( gbl.LoKi.Particles.HasProtoParticle  ) () 
HASTRACK  = helper.loadCutP ( gbl.LoKi.Particles.HasTrack          ) () 

INPVFIT   = helper.loadCutP ( gbl.LoKi.Particles.UsedForPrimaryVertex )

PIDAGREE  = helper.loadCutP ( gbl.LoKi.Particles.PPIsPIDCompatible )
BESTPID   = helper.loadFunP ( gbl.LoKi.Particles.PPbestPID         ) ()
BESTPIDPR = helper.loadFunP ( gbl.LoKi.Particles.PPprobOfBestPID   ) ()

PIDBITS   = helper.loadCutP ( gbl.LoKi.Particles.PPbitsPID         )
# see $LOKIROOT/LoKi/ProtoParticles.h
PPBITNONE  = PIDBITS (   0 )
#
PPBITRICH  = PIDBITS (   1 )
PPBITMUON  = PIDBITS (   2 )
PPBITCALO  = PIDBITS (   3 )
#
PPRICHCOMB = PIDBITS ( 101 )
PPMUONCOMB = PIDBITS ( 102 )
PPCALOCOMB = PIDBITS ( 103 )
#
PPHASRICH  = PIDBITS ( 201 )
PPHASMUON  = PIDBITS ( 202 )
PPHASCALO  = PIDBITS ( 203 )

MCTRUTH    = helper.loadCutP ( gbl.LoKi.Particles.MCTruth       ) 

HASTRGTR   = helper.loadCutP ( gbl.LoKi.TrgTracks.HasTrgTrack   ) ()

ISCONTOBJ    = helper.loadCutP ( gbl.LoKi.Particles.IsContainedObject )
ISPARTICLE   = helper.loadCutP ( gbl.LoKi.Particles.IsParticle        )
ISORIGIN     = helper.loadCutP ( gbl.LoKi.Particles.IsOrigin          )
ISFROMORIGIN = helper.loadCutP ( gbl.LoKi.Particles.IsFromOrigin      )
