#!/usr/bin/env python
# =============================================================================
# $Id: bendervertexcuts.py,v 1.3 2004-08-26 19:34:36 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.2  2004/08/06 12:07:10  ibelyaev
#  minor improvements
#
# Revision 1.1  2004/07/11 15:47:06  ibelyaev
#  regular incrment
#
# =============================================================================


# =============================================================================
# @file
#
# defintion of all Lo?ki/Bender functions/cuts 
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
VFunc    = helper.loadFunV ( gbl.LoKi.Function               ( 'const Vertex*' ) ) 
# interface functions, load & add the operators!!!
VCuts    = helper.loadCutV ( gbl.LoKi.Predicate              ( 'const Vertex*' ) )
# load & add the operators 
VFun     = helper.loadFunV ( gbl.LoKi.FunctionFromFunction   ( 'const Vertex*' ) )
# load & add the operators 
VCut     = helper.loadCutV ( gbl.LoKi.PredicateFromPredicate ( 'const Vertex*' ) ) 
# load & add the operators 
VCONST   = helper.loadFunV ( gbl.LoKi.Constant               ( 'const Vertex*' ) )
# load & add the operators 
VBCONST  = helper.loadCutV ( gbl.LoKi.BooleanConstant        ( 'const Vertex*' ) )

VONE      = VCONST  ( 1.0 )
VZERO     = VCONST  ( 0.0 )
VALL      = VBCONST (   1 )
VTRUE     = VALL
VNONE     = VBCONST (   0 )
VFALSE    = VNONE

VMIN      = helper.loadFunV ( gbl.LoKi.Min ( 'const Vertex*'     ) )
VMAX      = helper.loadFunV ( gbl.LoKi.Max ( 'const Vertex*'     ) )

VCHI2     = helper.loadFunV ( gbl.LoKi.Vertices.VertexChi2   )  ()
CHI2V     = VCHI2

VTYPE     = helper.loadFunV ( gbl.LoKi.Vertices.VertexType    ) () 
VDOF      = helper.loadFunV ( gbl.LoKi.Vertices.VertexDoF     ) () 
VX        = helper.loadFunV ( gbl.LoKi.Vertices.VertexX       ) () 
VY        = helper.loadFunV ( gbl.LoKi.Vertices.VertexY       ) () 
VZ        = helper.loadFunV ( gbl.LoKi.Vertices.VertexZ       ) () 
VPRONGS   = helper.loadFunV ( gbl.LoKi.Vertices.VertexProngs  ) () 
VTRACKS   = helper.loadFunV ( gbl.LoKi.Vertices.VertexTracks  ) () 

VCHI2D    = helper.loadFunV ( gbl.LoKi.Vertices.VertexChi2Distance )
VMCHI2D   = helper.loadFunV ( gbl.LoKi.Vertices.VertexChi2Distance )

VIP       = helper.loadFunV ( gbl.LoKi.Vertices.VertexImpPar     ) 
VIPCHI2   = helper.loadFunV ( gbl.LoKi.Vertices.VertexImpParChi2 )
CHI2VIP   = VIPCHI2

VPSD      = helper.loadFunV ( gbl.LoKi.Vertices.VertexParticleSignedDistance )

VMCHI2    = helper.loadFunV ( gbl.LoKi.Vertices.MinVertexChi2Distance )
CHI2VM    = VMCHI2

