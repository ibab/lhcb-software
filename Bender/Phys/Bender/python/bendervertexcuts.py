#!/usr/bin/env python
# =============================================================================
# $Id: bendervertexcuts.py,v 1.1 2004-07-11 15:47:06 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
#
# defintion of all Lo?ki/Bender functions/cuts 
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
VFunc    = helper.loadFunV   ( gbl.LoKi.Function  ( 'const Vertex*'     ) ) 

# interface functions, load & add the operators!!!
VCuts    = helper.loadCutV   ( gbl.LoKi.Predicate ( 'const Vertex*'     ) )

# load & add the operators 
VFun     = helper.loadFunV   ( gbl.LoKi.FunctionFromFunction   ( 'const Vertex*'     ) )

# load & add the operators 
VCut     = helper.loadCutV   ( gbl.LoKi.PredicateFromPredicate ( 'const Vertex*'     ) ) 

# load & add the operators 
VCONST   = helper.loadFunV   ( gbl.LoKi.Constant ( 'const Vertex*'     ) )

# load & add the operators 
VBCONST   = helper.loadCutP   ( gbl.LoKi.BooleanConstant ( 'const Vertex*'     ) )

VALL       = VBCONST   ( 1 )
VTRUE      = VALL
VNONE      = VBCONST   ( 0 )
VFALSE     = VNONE

# functions 
VIP     = helper.loadFunV ( gbl.LoKi.Vertices.VertexImpPar     ) 
VIPCHI2 = helper.loadFunV ( gbl.LoKi.Vertices.VertexImpParChi2 )
# alias 
CHI2VIP = VIPCHI2

#functions 
VPSD    = helper.loadFunV ( gbl.LoKi.Vertices.VertexParticleSignedDistance )

#object 
VCHI2   = helper.loadFunV ( gbl.LoKi.Vertices.VertexChi2   )  ()
#alias 
CHI2V   = VCHI2
#object 
VTYPE   = helper.loadFunV ( gbl.LoKi.Vertices.VertexType    ) () 
VDOF    = helper.loadFunV ( gbl.LoKi.Vertices.VertexDoF     ) () 
VX      = helper.loadFunV ( gbl.LoKi.Vertices.VertexX       ) () 
VY      = helper.loadFunV ( gbl.LoKi.Vertices.VertexY       ) () 
VZ      = helper.loadFunV ( gbl.LoKi.Vertices.VertexZ       ) () 
VPRONGS = helper.loadFunV ( gbl.LoKi.Vertices.VertexProngs  ) () 
VTRACKS = helper.loadFunV ( gbl.LoKi.Vertices.VertexTracks  ) () 

#function 
VMCHI2  = helper.loadFunV ( gbl.LoKi.Vertices.MinVertexChi2Distance )
CHI2VM  = VMCHI2

