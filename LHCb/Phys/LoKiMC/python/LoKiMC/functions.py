#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.22 2010-06-06 08:07:22 ibelyaev Exp $ 
# =============================================================================
## @file functions.py LoKiMC/functions.py
#  The set of basic functions from LoKiMC library
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
The set of basic functions from LoKiMC library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
""" 
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "????-??-??"
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision: 1.22 $ "
# =============================================================================

import LoKiCore.decorators as _LoKiCore

# Namespaces:
cpp      = _LoKiCore.cpp
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _LoKiCore.LHCb
Gaudi    = cpp.Gaudi 


_MCP = 'const LHCb::MCParticle*'
_MCV = 'const LHCb::MCVertex*'

_c1 = std.vector( _MCP )
_c2 = std.vector( _MCV )

## ranges
MCRange   = Gaudi.NamedRange_ ( _c1      )
MCVRange  = Gaudi.NamedRange_ ( _c2      )
## range-lists: 
MCRangeL  = LoKi.RangeList_  (  MCRange )
MCVRangeL = LoKi.RangeList_  ( MCVRange )

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::MCFunc
MCFunc  = LoKi.Functor               ( _MCP , 'double' ) 
## @see LoKi::Types::MCCuts
MCCuts  = LoKi.Functor               ( _MCP ,  bool    ) 
## @see LoKi::Types::MCFun
MCFun   = LoKi.FunctorFromFunctor    ( _MCP , 'double' ) 
## @see LoKi::Types::MCCut
MCCut   = LoKi.FunctorFromFunctor    ( _MCP ,  bool    ) 
## @see LoKi::Types::MCFunc
MCVFunc = LoKi.Functor               ( _MCV , 'double' ) 
## @see LoKi::Types::MCCuts
MCVCuts = LoKi.Functor               ( _MCV ,  bool    ) 
## @see LoKi::Types::MCFun
MCVFun  = LoKi.FunctorFromFunctor    ( _MCV , 'double' ) 
## @see LoKi::Types::MCCut
MCVCut  = LoKi.FunctorFromFunctor    ( _MCV ,  bool    ) 

# =============================================================================
## All concrete types 
# =============================================================================

## @see LoKi::Cuts::MCTRUE
MCTRUE     = LoKi.Constant( _MCP , bool )(True)
## @see LoKi::Cuts::MCFALSE
MCFALSE    = LoKi.Constant( _MCP , bool )(False)
## @see LoKi::Cuts::MCALL
MCALL      = MCTRUE
## @see LoKi::Cuts::MCNONE
MCNONE     = MCFALSE
## @see LoKi::Cuts::MCONE
MCONE      = LoKi.Constant     ( _MCP , 'double' ) (1.0)
## @see LoKi::Cuts::MCZERO
MCZERO     = LoKi.Constant     ( _MCP , 'double' ) (0.0)
## @see LoKi::Cuts::MCMIN
#MCMIN      = LoKi.Min          ( _MCP )
## @see LoKi::Cuts::MCMIN
#MCMAX      = LoKi.Max          ( _MCP )
## @see LoKi::Cuts::MCVALID
MCVALID    = LoKi.Valid        ( _MCP )()
## @see LoKi::Cuts::MCSAME
#MCSAME     = LoKi.TheSame      ( _MCP )
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

## @see LoKi::Cuts::MCINANCESTORS 
MCINANCESTORS  = LoKi.MCParticles.InAncestors
## @see LoKi::Cuts::MCNINANCESTORS 
MCNINANCESTORS = LoKi.MCParticles.NinAncestors

## @see LoKi::Cuts::MCY
MCY        = LoKi.MCParticles.Rapidity  ()
## @see LoKi::Cuts::MCY0
MCY0       = LoKi.MCParticles.Rapidity0 ()

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
MCVTRUE     = LoKi.Constant( _MCV  , bool )(True)
## @see LoKi::Cuts::MCVFALSE
MCVFALSE    = LoKi.Constant( _MCV  , bool )(False)
## @see LoKi::Cuts::MCVALL
MCVALL      = MCVTRUE
## @see LoKi::Cuts::MCVNONE
MCVNONE     = MCVFALSE
## @see LoKi::Cuts::MCVONE
MCVONE      = LoKi.Constant     ( _MCV , 'double' )(1.0)
## @see LoKi::Cuts::MCVZERO
MCVZERO     = LoKi.Constant     ( _MCV , 'double' )(0.0)
## @see LoKi::Cuts::MCVMIN
#MCVMIN      = LoKi.Min         ( _MCV )
## @see LoKi::Cuts::MCVMIN
#MCVMAX      = LoKi.Max         ( _MCV )
## @see LoKi::Cuts::MCVVALID
MCVVALID    = LoKi.Valid        ( _MCV )()
## @see LoKi::Cuts::MCVSAME
#MCVSAME     = LoKi.TheSame      ( _MCV )
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
## @see LoKi::Cuts::MCVPXFUN
MCVPXFUN    = LoKi.MCVertices.MCVPFunAsMCFun

## @see LoKi::Cuts::MCVKEY
MCVKEY      = LoKi.MCVertices.Key () 

## @see LoKi::Cuts::MCDECNODE
MCDECNODE   = LoKi.MCParticles.DecNode
## @see LoKi::Cuts::MCDECTREE
MCDECTREE   = LoKi.MCParticles.DecTree


## @see LoKi::Cuts::MCFROMDECAYS
MCFROMDECAYS = LoKi.MCParticles.FromDecays       ()
## @see LoKi::Cuts::MCFROMXS
MCFROMXS     = LoKi.MCParticles.FromInteractions ()

# functional part:

## functional part
##_vp       = std.vector ( _MCP    )
##_vv       = std.vector ( _MCV    )
##_vd       = std.vector ('double')
_vp       = 'std::vector<const LHCb::MCParticle*>'
_vv       = 'std::vector<const LHCb::MCVertex*>'
_vd       = 'std::vector<double>'

#
MCMaps      = LoKi.Functor             ( _vp , _vd      )
MCMap       = LoKi.FunctorFromFunctor  ( _vp , _vd      )
MCPipes     = LoKi.Functor             ( _vp , _vp      )
MCPipe      = LoKi.FunctorFromFunctor  ( _vp , _vp      )
MCFunVals   = LoKi.Functor             ( _vp , 'double' )
MCFunVal    = LoKi.FunctorFromFunctor  ( _vp , 'double' )
MCCutVals   = LoKi.Functor             ( _vp , bool     )
MCCutVal    = LoKi.FunctorFromFunctor  ( _vp , bool     )
MCElements  = LoKi.Functor             ( _vp , _MCP     ) 
MCElement   = LoKi.FunctorFromFunctor  ( _vp , _MCP     ) 
MCSources   = LoKi.Functor             ('void', _vp     )
MCSource    = LoKi.FunctorFromFunctor  ('void', _vp     )
#
MCVMaps     = LoKi.Functor             ( _vv , _vd      )
MCVMap      = LoKi.FunctorFromFunctor  ( _vv , _vd      )
MCVPipes    = LoKi.Functor             ( _vv , _vv      )
MCVPipe     = LoKi.FunctorFromFunctor  ( _vv , _vv      )
MCVFunVals  = LoKi.Functor             ( _vv , 'double' )
MCVFunVal   = LoKi.FunctorFromFunctor  ( _vv , 'double' )
MCVCutVals  = LoKi.Functor             ( _vv , bool     )
MCVCutVal   = LoKi.FunctorFromFunctor  ( _vv , bool     )
MCVElements = LoKi.Functor             ( _vv , _MCV     ) 
MCVElement  = LoKi.FunctorFromFunctor  ( _vv , _MCV     ) 
MCVSources  = LoKi.Functor             ('void', _vv     )
MCVSource   = LoKi.FunctorFromFunctor  ('void', _vv     )


MCSOURCE    = LoKi.MCParticles . SourceTES
MCVSOURCE   = LoKi.MCVertices  . SourceTES
MCNUM       = LoKi.MCParticles . TESCounter


MCEMPTY     = LoKi.Functors.Empty ( _MCP ) ()
MCVEMPTY    = LoKi.Functors.Empty ( _MCV ) ()
MCSIZE      = LoKi.Functors.Size  ( _MCP ) ()
MCVSIZE     = LoKi.Functors.Size  ( _MCV ) ()


## angles

## @see LoKi::Cuts::MCCOSPOL 
MCCOSPOL     = LoKi.MCParticles.PolarizationAngle
## @see LoKi::Cuts::MCSINCHI
MCSINCHI     = LoKi.MCParticles.SinChi
## @see LoKi::Cuts::MCCOSCHI
MCCOSCHI     = LoKi.MCParticles.CosChi
## @see LoKi::Cuts::MCANGLECHI
MCANGLECHI   = LoKi.MCParticles.AngleChi
## @see LoKi:Cuts::MCCOSTHETATR
MCCOSTHETATR = LoKi.MCParticles.CosThetaTr
## @see LoKi:Cuts::MCSINPHITR
MCSINPHITR   = LoKi.MCParticles.SinPhiTr
## @see LoKi:Cuts::MCCOSPHITR
MCCOSPHITR   = LoKi.MCParticles.CosPhiTr
## @see LoKi:Cuts::MCANGLEPHITR
MCANGLEPHITR = LoKi.MCParticles.AnglePhiTr


# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i 
        
# =============================================================================
# The END 
# =============================================================================
