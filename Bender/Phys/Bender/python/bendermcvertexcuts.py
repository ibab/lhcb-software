#!/usr/bin/env python
# =============================================================================
# $Id: bendermcvertexcuts.py,v 1.1 2004-07-11 15:47:06 ibelyaev Exp $ 
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

from   benderunits        import  *
from   bendertypes        import  * 
from   benderaux          import  * 

# global namespace  
gbl = gaudi.gbl 

# application manager         
g = gaudi.AppMgr()
# load the minimal set of dictionaries
g.loaddict('EventDict')
g.loaddict('PhysEventDict')
g.loaddict('BenderDict')


# interface functions, load & add the operators!!!
MCVFunc  = helper.loadFunMCV ( gbl.LoKi.Function  ( 'const MCVertex*'   ) ) 

# interface functions, load & add the operators!!!
MCVCuts  = helper.loadCutMCV ( gbl.LoKi.Predicate ( 'const MCVertex*'   ) )

# load & add the operators 
MCVFun   = helper.loadFunMCV ( gbl.LoKi.FunctionFromFunction   ( 'const MCVertex*'   ) ) 

# load & add the operators 
MCVCut   = helper.loadCutMCV ( gbl.LoKi.PredicateFromPredicate ( 'const MCVertex*'   ) )

# load & add the operators 
MCVCONST = helper.loadFunMCV ( gbl.LoKi.Constant ( 'const MCVertex*'   ) ) 

# load & add the operators 
MCVBCONST = helper.loadCutMCV ( gbl.LoKi.BooleanConstant ( 'const MCVertex*'   ) )

MCVALL     = MCVBCONST ( 1 )
MCVTRUE    = MCVALL
MCVNONE    = MCVBCONST ( 0 )
MCVFALSE   = MCVNONE


# objects 
MCVTYPE    = helper.loadFunMCV ( gbl.LoKi.MCVertices.TypeOfMCVertex   ) ()
MCVTOF     = helper.loadFunMCV ( gbl.LoKi.MCVertices.TimeOfFlight     ) ()
MCVX       = helper.loadFunMCV ( gbl.LoKi.MCVertices.VertexPositionX  ) ()
MCVY       = helper.loadFunMCV ( gbl.LoKi.MCVertices.VertexPositionY  ) ()
MCVZ       = helper.loadFunMCV ( gbl.LoKi.MCVertices.VertexPositionZ  ) ()

# function 
MCVDIST    = helper.loadFunMCV ( gbl.LoKi.MCVertices.MCVertexDistance ) 
