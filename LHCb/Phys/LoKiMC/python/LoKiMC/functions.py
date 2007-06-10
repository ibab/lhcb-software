#!/usr/bin/env python
# =============================================================================
## @file functions.py LoKiMC/functions.py
#  The set of basic functions from LoKiMC library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The set of basic functions from LoKiMC library """ 
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore

# Namespaces:
_global  = _LoKiCore._global 
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _LoKiCore.LHCb

_MCP = 'const LHCb::MCParticle*'
_MCV = 'const LHCb::MCVertex*'

_c1 = std.vector( _MCP )
_c2 = std.vector( _MCV )

## ranges
MCRange   = LoKi.NamedRange_ ( _c1      )
MCVRange  = LoKi.NamedRange_ ( _c2      )
## range-lists: 
MCRangeL  = LoKi.RangeList_  (  MCRange )
MCVRangeL = LoKi.RangeList_  ( MCVRange )

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::MCFunc
MCFunc  = LoKi.Function               ( _MCP ) 
## @see LoKi::Types::MCCuts
MCCuts  = LoKi.Predicate              ( _MCP ) 
## @see LoKi::Types::MCFun
MCFun   = LoKi.FunctionFromFunction   ( _MCP ) 
## @see LoKi::Types::MCCut
MCCut   = LoKi.PredicateFromPredicate ( _MCP ) 
## @see LoKi::Types::MCFunc
MCVFunc = LoKi.Function               ( _MCV ) 
## @see LoKi::Types::MCCuts
MCVCuts = LoKi.Predicate              ( _MCV ) 
## @see LoKi::Types::MCFun
MCVFun  = LoKi.FunctionFromFunction   ( _MCV ) 
## @see LoKi::Types::MCCut
MCVCut  = LoKi.PredicateFromPredicate ( _MCV ) 

# =============================================================================
## All concrete types 
# =============================================================================

## @see LoKi::Cuts::MCTRUE
MCTRUE     = LoKi.BooleanConstant( _MCP )(True)
## @see LoKi::Cuts::MCFALSE
MCFALSE    = LoKi.BooleanConstant( _MCP )(False)
## @see LoKi::Cuts::MCALL
MCALL      = MCTRUE
## @see LoKi::Cuts::MCNONE
MCNONE     = MCFALSE
## @see LoKi::Cuts::MCONE
MCONE      = LoKi.Constant     ( _MCP )(1.0)
## @see LoKi::Cuts::MCZERO
MCZERO     = LoKi.Constant     ( _MCP )(0.0)
## @see LoKi::Cuts::MCMIN
MCMIN      = LoKi.Min          ( _MCP )
## @see LoKi::Cuts::MCMIN
MCMAX      = LoKi.Max          ( _MCP )
## @see LoKi::Cuts::MCSWITCH
MCSWITCH   = LoKi.Switch       ( _MCP )
## @see LoKi::Cuts::MCSSWITCH
MCSSWITCH  = LoKi.SimpleSwitch ( _MCP )
## @see LoKi::Cuts::MCVALID
MCVALID    = LoKi.Valid        ( _MCP )()
## @see LoKi::Cuts::MCSAME
MCSAME     = LoKi.TheSame      ( _MCP )
## @see LoKi::Cuts::OSCILLATED
OSCILLATED = LoKi.MCParticles.Oscillated   ()
## @see LoKi::Cuts::MCOSCILLATED
MCOSCILLATED = OSCILLATED 
## @see LoKi::Cuts::MCP
MCP        = LoKi.MCParticles.Momentum () 
## @see LoKi::Cuts::MCE
MCE        = LoKi.MCParticles.Energy   ()
## @see LoKi::Cuts::MCPT
MCPT       = LoKi.MCParticles.TransverseMomentum () 
## @see LoKi::Cuts::MCPX
MCPX       = LoKi.MCParticles.MomentumX ()
## @see LoKi::Cuts::MCPY
MCPY       = LoKi.MCParticles.MomentumY () 
## @see LoKi::Cuts::MCPZ
MCPZ       = LoKi.MCParticles.MomentumZ () 
## @see LoKi::Cuts::MCETA
MCETA      = LoKi.MCParticles.PseudoRapidity() 
## @see LoKi::Cuts::MCPHI
MCPHI      = LoKi.MCParticles.Phi() 
## @see LoKi::Cuts::MCTHETA
MCTHETA    = LoKi.MCParticles.Theta() 
## @see LoKi::Cuts::MCMASS
MCMASS     = LoKi.MCParticles.Mass() 
## @see LoKi::Cuts::MCM
MCM        = MCMASS 
## @see LoKi::Cuts::MCID 
MCID       = LoKi.MCParticles.Identifier () 
## @see LoKi::Cuts::MCABSID 
MCABSID    = LoKi.MCParticles.AbsIdentifier () 
## @see LoKi::Cuts::MC3Q
MC3Q       = LoKi.MCParticles.ThreeCharge () 
## @see LoKi::Cuts::MCTIME
MCTIME     = LoKi.MCParticles.ProperLifeTime() 
## @see LoKi::Cuts::MCCTAU
MCCTAU     = MCTIME 
## @see LoKi::Cuts::MCQUARK
MCQUARK    = LoKi.MCParticles.HasQuark
## @see LoKi::Cuts::MCCHARM
##MCCHARM    =  MCQUARK( _LHCb.ParticleID.charm   ) 
MCCHARM    =  MCQUARK( 4  ) 
## @see LoKi::Cuts::MCBEAUTY
## MCBEAUTY   =  MCQUARK( _LHCb.ParticleID.bottom  ) 
MCBEAUTY   =  MCQUARK( 5  ) 
## @see LoKi::Cuts::MCTOP
##MCTOP      =  MCQUARK( _LHCb.ParticleID.top     ) 
MCTOP      =  MCQUARK( 6  ) 
## @see LoKi::Cuts::MCSTRANGE 
##MCSTRANGE  =  MCQUARK( _LHCb.ParticleID.strange ) 
MCSTRANGE  =  MCQUARK( 3  ) 
## @see LoKi::Cuts::MCCHARGED
MCCHARGED  = LoKi.MCParticles.IsCharged () 
## @see LoKi::Cuts::MCNEUTRAL
MCNEUTRAL  = LoKi.MCParticles.IsNeutral () 
## @see LoKi::Cuts::MCLEPTON
MCLEPTON   = LoKi.MCParticles.IsLepton  () 
## @see LoKi::Cuts::MCMESON
MCMESON    = LoKi.MCParticles.IsMeson   () 
## @see LoKi::Cuts::MCBARYON
MCBARYON   = LoKi.MCParticles.IsBaryon  () 
## @see LoKi::Cuts::MCHADRON
MCHADRON   = LoKi.MCParticles.IsHadron  () 
## @see LoKi::Cuts::MCNUCLEUS
MCNUCLEUS  = LoKi.MCParticles.IsNucleus () 
## @see LoKi::Cuts::FROMMCTREE 
FROMMCTREE = LoKi.MCParticles.FromMCDecayTree
## @see LoKi::Cuts::NINMCDOWN
NINMCDOWN  = LoKi.MCParticles.NinMCdownTree
## @see LoKi::Cuts::MCMOTHER 
MCMOTHER   = LoKi.MCParticles.MCMotherFunction
## @see LoKi::Cuts::MCMOTHCUT
MCMOTHCUT  = LoKi.MCParticles.MCMotherPredicate
## @see LoKi::Cuts::MCMOMDIST
MCMOMDIST  = LoKi.MCParticles.MomentumDistance
## @see LoKi::Cuts::MCPTDIR 
MCPTDIR    = LoKi.MCParticles.TransverseMomentumRel
## @see LoKi::Cuts::MCDPHI 
MCDPHI     = LoKi.MCParticles.DeltaPhi
## @see LoKi::Cuts::MCDETA
MCDETA     = LoKi.MCParticles.DeltaEta
## @see LoKi::Cuts::MCOVALID 
MCOVALID   = LoKi.MCParticles.ValidOrigin ()
## @see LoKi::Cuts::MCVFASPF 
MCVFASPF   = LoKi.MCParticles.MCVertexFunAdapter
## @see LoKi::Cuts::MCDECAY
MCDECAY    = LoKi.MCParticles.MCDecayPattern
## @see LoKi::Cuts::MCREC
MCREC      = LoKi.MCParticles.MCReconstructible
## @see LoKi::Cuts::MCRECAS
MCRECAS    = LoKi.MCParticles.MCReconstructibleAs
## @see LoKi::Cuts::MCCHILD
MCCHILD    = LoKi.MCParticles.ChildFunction 
## @see LoKi::Cuts::MCCHILDFUN
MCCHILDFUN = LoKi.MCParticles.ChildFunction 
## @see LoKi::Cuts::MCCHILDCUT
MCCHILDCUT = LoKi.MCParticles.ChildPredicate
## @see LoKi::Cuts::MCINTREE
MCINTREE   = LoKi.MCParticles.InTree
## @see LoKi::Cuts::MCNINTREE
MCNINTREE  = LoKi.MCParticles.NinTree
## @see LoKi::Cuts::MCSUMTREE
MCSUMTREE  = LoKi.MCParticles.SumTree
## @see LoKi::Cuts::MCMULTTREE
MCMULTTREE = LoKi.MCParticles.MultTree

## @see LoKi::Cuts::CHARM
CHARM      =  MCCHARM 
## @see LoKi::Cuts::BEAUTY
BEAUTY     =  MCBEAUTY 
## @see LoKi::Cuts::TOP
TOP        =  MCTOP 
## @see LoKi::Cuts::STRANGE 
STRANGE    =  MCSTRANGE 
## @see LoKi::Cuts::CHARGED
CHARGED    = MCCHARGED 
## @see LoKi::Cuts::NEUTRAL
NEUTRAL    = MCNEUTRAL 
## @see LoKi::Cuts::LEPTON
LEPTON     = MCLEPTON
## @see LoKi::Cuts::MESON
MESON      = MCMESON
## @see LoKi::Cuts::BARYON
BARYON     = MCBARYON
## @see LoKi::Cuts::HADRON
HADRON     = MCHADRON
## @see LoKi::Cuts::NUCLEUS
NUCLEUS    = MCNUCLEUS

## @see LoKi::Cuts::MCVTRUE
MCVTRUE     = LoKi.BooleanConstant( _MCV )(True)
## @see LoKi::Cuts::MCVFALSE
MCVFALSE    = LoKi.BooleanConstant( _MCV )(False)
## @see LoKi::Cuts::MCVALL
MCVALL      = MCVTRUE
## @see LoKi::Cuts::MCVNONE
MCVNONE     = MCVFALSE
## @see LoKi::Cuts::MCVONE
MCVONE      = LoKi.Constant     ( _MCV )(1.0)
## @see LoKi::Cuts::MCVZERO
MCVZERO     = LoKi.Constant     ( _MCV )(0.0)
## @see LoKi::Cuts::MCVMIN
MCVMIN      = LoKi.Min          ( _MCV )
## @see LoKi::Cuts::MCVMIN
MCVMAX      = LoKi.Max          ( _MCV )
## @see LoKi::Cuts::MCVSWITCH
MCVSWITCH   = LoKi.Switch       ( _MCV )
## @see LoKi::Cuts::MCVSSWITCH
MCVSSWITCH  = LoKi.SimpleSwitch ( _MCV )
## @see LoKi::Cuts::MCVVALID
MCVVALID    = LoKi.Valid        ( _MCV )()
## @see LoKi::Cuts::MCVSAME
MCVSAME     = LoKi.TheSame      ( _MCV )
## @see LoKi::Cuts::MCVTYPE
MCVTYPE     = LoKi.MCVertices.TypeOfMCVertex ()
## @see LoKi::Cuts::MCVFOT
MCVTOF      = LoKi.MCVertices.TimeOfFlight   ()
## @see LoKi::Cuts::MCVX 
MCVX        = LoKi.MCVertices.VertexPositionX()
## @see LoKi::Cuts::MCVY
MCVY        = LoKi.MCVertices.VertexPositionY()
## @see LoKi::Cuts::MCVZ 
MCVZ        = LoKi.MCVertices.VertexPositionZ()
## @see LoKi::Cuts::MCVTIME
MCVTIME     = LoKi.MCVertices.VertexTime     ()
## @see LoKi::Cuts::MCPRIMARY
MCPRIMARY   = LoKi.MCVertices.Primary        ()
## @see LoKi::Cuts::MCISDECAY
MCISDECAY   = LoKi.MCVertices.Decay          ()
## @see LoKi::Cuts::MCCDIST
MCVDIST     = LoKi.MCVertices.MCVertexDistance
## @see LoKi::Cuts::MCVXFUN
MCVXFUN     = LoKi.MCVertices.MCVFunAsMCFun

_MCChild = LoKi.Child.MCChild 

# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o
        
# =============================================================================
# The END 
# =============================================================================
