#!/usr/bin/env python
# =============================================================================
# $Id: bendermcparticlecuts.py,v 1.3 2004-08-08 13:41:11 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.2  2004/08/06 12:07:06  ibelyaev
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

# adapter function 
MCVXFUN    = helper.loadFunMCP ( gbl.LoKi.MCVertices.MCVFunAsMCFun    ) 

# matching function 
RCTRUTH    = helper.loadCutMCP ( gbl.LoKi.MCParticles.RCTruth         ) 
