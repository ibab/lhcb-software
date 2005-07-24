#!/usr/bin/env python
# =============================================================================
# $Id: bendermcparticlecuts.py,v 1.7 2005-07-24 14:04:41 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.7 $
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.6  2005/05/20 10:55:19  ibelyaev
#  prepare for v4r8
#
# Revision 1.5  2005/02/02 19:15:10  ibelyaev
#  add new functions
#
# Revision 1.4  2004/08/26 19:34:36  ibelyaev
#  remove explict PyLCGDict
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
MCFunc    = helper.loadFunMCP ( gbl.LoKi.Function               ( 'const MCParticle*' ) )
# interface functions, load & add the operators!!!
MCCuts    = helper.loadCutMCP ( gbl.LoKi.Predicate              ( 'const MCParticle*' ) )
# load & add the operators 
MCFun     = helper.loadFunMCP ( gbl.LoKi.FunctionFromFunction   ( 'const MCParticle*' ) ) 
# load & add the operators 
MCCut     = helper.loadCutMCP ( gbl.LoKi.PredicateFromPredicate ( 'const MCParticle*' ) ) 
# load & add the operators 
MCPCONST  = helper.loadFunMCP ( gbl.LoKi.Constant               ( 'const MCParticle*' ) ) 
# load & add the operators 
MCPBCONST = helper.loadCutMCP ( gbl.LoKi.BooleanConstant        ( 'const MCParticle*' ) ) 

MCONE      = MCPCONST  ( 1.0 )
MCPONE     = MCONE
MCPZERO    = MCPCONST  ( 0.0 )
MCZERO     = MCPZERO

MCPALL     = MCPBCONST ( 1 )
MCALL      = MCPALL 
MCPTRUE    = MCPALL
MCTRUE     = MCPALL 
MCPNONE    = MCPBCONST ( 0 )
MCNONE     = MCPNONE 
MCPFALSE   = MCPNONE
MCFALSE    = MCPNONE 

MCPMIN     = helper.loadFunMCP ( gbl.LoKi.Min ( 'const MCParticle*' ) )
MCMIN      = MCPMIN 
MCPMAX     = helper.loadFunMCP ( gbl.LoKi.Max ( 'const MCParticle*' ) )
MCMAX      = MCPMAX

# objects 
MCP     = helper.loadFunMCP     ( gbl.LoKi.MCParticles.Momentum           ) ()
MCE     = helper.loadFunMCP     ( gbl.LoKi.MCParticles.Energy             ) ()
MCPT    = helper.loadFunMCP     ( gbl.LoKi.MCParticles.TransverseMomentum ) ()
MCPX    = helper.loadFunMCP     ( gbl.LoKi.MCParticles.MomentumX          ) ()
MCPY    = helper.loadFunMCP     ( gbl.LoKi.MCParticles.MomentumY          ) ()
MCPZ    = helper.loadFunMCP     ( gbl.LoKi.MCParticles.MomentumZ          ) ()
MCMASS  = helper.loadFunMCP     ( gbl.LoKi.MCParticles.Mass               ) ()
MCID    = helper.loadFunMCID    ( gbl.LoKi.MCParticles.Identifier         ) ()
MCABSID = helper.loadFunMCABSID ( gbl.LoKi.MCParticles.AbsIdentifier      ) ()
MC3Q    = helper.loadFunMCP     ( gbl.LoKi.MCParticles.ThreeCharge        ) ()
MCCTAU  = helper.loadFunMCP     ( gbl.LoKi.MCParticles.ProperLifeTime     ) ()
MCTIME  = MCCTAU


# objects 
CHARGED = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsCharged ) ()
NEUTRAL = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsNeutral ) ()
LEPTON  = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsLepton  ) ()
MESON   = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsMeson   ) ()
BARYON  = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsBaryon  ) ()
HADRON  = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsHadron  ) ()
NUCLEUS = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsNucleus ) ()

# function
MCQUARK = helper.loadCutMCP ( gbl.LoKi.MCParticles.HasQuark )

# objects 
BEAUTY  = MCQUARK( 5 ) 
CHARM   = MCQUARK( 4 ) 
STRANGE = MCQUARK( 3 )

# function 
FROMMCTREE = helper.loadCutMCP ( gbl.LoKi.MCParticles.FromMCDecayTree )
NINMCDOWN  = helper.loadFunMCP ( gbl.LoKi.MCParticles.NinMCdownTree   )

# adapter functions
MCVXFUN    = helper.loadFunMCP ( gbl.LoKi.MCVertices.MCVFunAsMCFun      ) 
MCMOTHER   = helper.loadFunMCP ( gbl.LoKi.MCParticles.MCMotherFunction  )
MCMOTHCUT  = helper.loadCutMCP ( gbl.LoKi.MCParticles.MCMotherPredicate )

ISMCCO     = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsContainedObject )
ISMCPART   = helper.loadCutMCP ( gbl.LoKi.MCParticles.IsParticle        )

# matching function 
RCTRUTH    = helper.loadCutMCP ( gbl.LoKi.MCParticles.RCTruth         ) 

# =============================================================================
# The END 
# =============================================================================
