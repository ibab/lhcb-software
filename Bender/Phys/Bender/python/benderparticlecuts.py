#!/usr/bin/env python
# =============================================================================
# $Id: benderparticlecuts.py,v 1.1 2004-07-11 15:47:06 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
#
# defintion of all LoKii/Bender Particle functions/cuts 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import PyLCGDict 
import gaudimodule        as      gaudi
import benderhelper       as      helper

# global namespace  
gbl = gaudi.gbl 

# application manager         
g = gaudi.AppMgr()
# load the minimal set of dictionaries
g.loaddict('EventDict')
g.loaddict('PhysEventDict')
g.loaddict('BenderDict')

# interface functions, load & add the operators!!!
Func     = helper.loadFunP   ( gbl.LoKi.Function  ( 'const Particle*'   ) ) 

# interface functions, load & add the operators!!!
Cuts     = helper.loadCutP   ( gbl.LoKi.Predicate ( 'const Particle*'   ) )

# load & add the operators 
Fun      = helper.loadFunP   ( gbl.LoKi.FunctionFromFunction   ( 'const Particle*'   ) )

# load & add the operators 
Cut      = helper.loadCutP   ( gbl.LoKi.PredicateFromPredicate ( 'const Particle*'   ) ) 

# load & add the operators 
PCONST   = helper.loadFunP   ( gbl.LoKi.Constant ( 'const Particle*'   ) )

# load & add the operators 
PBCONST   = helper.loadCutP   ( gbl.LoKi.BooleanConstant ( 'const Particle*'   ) )

PALL       = PBCONST   ( 1 )
PTRUE      = PALL
PNONE      = PBCONST   ( 0 )
PFALSE     = PNONE

# functions 
ID    =  helper.loadFunID ( gbl.LoKi.Particles.Identifier         ) () 
ABSID =  helper.loadFunP  ( gbl.LoKi.Particles.AbsIdentifier      ) () 
CL    =  helper.loadFunP  ( gbl.LoKi.Particles.ConfidenceLevel    ) () 
Q     =  helper.loadFunP  ( gbl.LoKi.Particles.Charge             ) ()      
SUMQ  =  helper.loadFunP  ( gbl.LoKi.Particles.SumCharge          ) ()      
P     =  helper.loadFunP  ( gbl.LoKi.Particles.Momentum           ) ()  
P2    =  helper.loadFunP  ( gbl.LoKi.Particles.Momentum2          ) ()  
PT    =  helper.loadFunP  ( gbl.LoKi.Particles.TransverseMomentum ) ()  
PX    =  helper.loadFunP  ( gbl.LoKi.Particles.MomentumX          ) () 
PY    =  helper.loadFunP  ( gbl.LoKi.Particles.MomentumY          ) ()  
PZ    =  helper.loadFunP  ( gbl.LoKi.Particles.MomentumZ          ) () 
E     =  helper.loadFunP  ( gbl.LoKi.Particles.Energy             ) () 
M     =  helper.loadFunP  ( gbl.LoKi.Particles.Mass               ) () 
MM    =  helper.loadFunP  ( gbl.LoKi.Particles.MeasuredMass       ) ()

# functions 
DMASS  =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMass             ) 
ADMASS =  helper.loadFunP ( gbl.LoKi.Particles.AbsDeltaMass          ) 
DMMASS =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMeasuredMass     )
CHI2M  =  helper.loadFunP ( gbl.LoKi.Particles.DeltaMeasuredMassChi2 ) 

# aliases 
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

# object 
TCHI2NDF = helper.loadFunP ( gbl.LoKi.Particles.TrackChi2PerDoF ) ()

# funstions 
IP       = helper.loadFunP ( gbl.LoKi.Vertices.ImpPar     ) 
IPCHI2   = helper.loadFunP ( gbl.LoKi.Vertices.ImpParChi2 )
# alias 
CHI2IP   = IPCHI2

# functions 
MIP      = helper.loadFunP ( gbl.LoKi.Vertices.MinImpPar     ) 
MIPCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.MinImpParChi2 )
# alias 
CHI2MIP  = MIPCHI2

#function 
MVDCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.MinChi2Distance )
#alias 
CHI2MVD  = MVDCHI2

# functions 
VD       = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistance     ) 
VDDOT    = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistanceDot  ) 
VDCHI2   = helper.loadFunP ( gbl.LoKi.Vertices.VertexDistanceChi2 )
# alias 
CHI2VD   = VDCHI2

# functions 
VDSIGN   = helper.loadFunP ( gbl.LoKi.Vertices.SignedVertexDistance ) 
VDTIME   = helper.loadFunP ( gbl.LoKi.Vertices.SignedTimeDistance   ) 
VDDTIME  = helper.loadFunP ( gbl.LoKi.Vertices.DotTimeDistance      ) 
DTR      = helper.loadFunP ( gbl.LoKi.Vertices.ClosestApproach      ) 
DTRCHI2  = helper.loadFunP ( gbl.LoKi.Vertices.ClosestApproachChi2  )

#aliases
CHI2DTR  = DTRCHI2

#function 
DIRA     = helper.loadFunP ( gbl.LoKi.Vertices.DirectionAngle )
#alias 
DDANG    = DIRA

# function
PID      = helper.loadFunP ( gbl.LoKi.Particles.ParticleIdEstimator )
# objects 
#   PIDe
#   PIDmu
#   PIDpi
#   PIDK
#   PIDp

# functions
CHILD     = helper.loadFunP ( gbl.LoKi.Particles.ChildFunction ) 
FROMTREE  = helper.loadCutP ( gbl.LoKi.Particles.FromDecayTree ) 
NOOVERLAP = helper.loadCutP ( gbl.LoKi.Particles.NoOverlap     ) 
FILTER    = helper.loadCutP ( gbl.LoKi.Adapters.FilterAsCut    ) 

PLTIME    = helper.loadFunP ( gbl.LoKi.Particles.LifeTime      )
PLTERR    = helper.loadFunP ( gbl.LoKi.Particles.LifeTimeError )
PLTCHI2   = helper.loadFunP ( gbl.LoKi.Particles.LifeTimeChi2  )

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

VXFUN     = helper.loadFunP ( gbl.LoKi.Adapters.VFuncAsFun     ) 
MCTRUTH   = helper.loadCutP ( gbl.LoKi.Particles.MCTruth       ) 
