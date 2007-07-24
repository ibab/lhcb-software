#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic objects from LoKiHlt library
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
""" The set of basic objects from LoKiTrack library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

# Namespaces:
_global  = _LoKiCore._global 
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _global.LHCb

_T = 'std::vector<const LHCb::Particle*>' 

# =============================================================================
## "Ordinary" functions for Tracks: "Tr"
# =============================================================================

## @see LoKi::Types::TrFunc
AFunc  = LoKi.Function               ( _T ) 
## @see LoKi::Types::TrCuts
ACuts  = LoKi.Predicate              ( _T ) 
## @see LoKi::Types::TrFun
AFun   = LoKi.FunctionFromFunction   ( _T ) 
## @see LoKi::Types::TrCut
ACut   = LoKi.PredicateFromPredicate ( _T )


# =============================================================================
## concrete functions:
# =============================================================================

## @see LoKi::Cuts::ASIZE
ASIZE         = LoKi.AParticles.Size   ()
## @see LoKi::Cuts::AVALID
AVALID        = LoKi.Valid(_T)         ()
## @see LoKi::Cuts::ASWITCH
ASWITCH       = LoKi.Switch(_T)        
## @see LoKi::Cuts::ASSWITCH
ASSWITCH      = LoKi.SimpleSwitch(_T)  

## @see LoKi::Cuts::ANUM
ANUM          = LoKi.AParticles.Count  
## @see LoKi::Cuts::AUNIQUE
AUNIQUE       = LoKi.AParticles.Unique 

## @see LoKi::Cuts::AMASS
AMASS         = LoKi.AParticles.InvariantMass 
## @see LoKi::Cuts::AM
AM            = AMASS(0)
## @see LoKi::Cuts::AM0
AM0           = AMASS(0)
## @see LoKi::Cuts::AM1
AM1           = AMASS(1)
## @see LoKi::Cuts::AM12
AM12          = AMASS(1,2)
## @see LoKi::Cuts::AM13
AM13          = AMASS(1,3)
## @see LoKi::Cuts::AM14
AM14          = AMASS(1,4)
## @see LoKi::Cuts::AM2
AM2           = AMASS(2)
## @see LoKi::Cuts::AM23
AM23          = AMASS(2,3)
## @see LoKi::Cuts::AM34
AM24          = AMASS(2,4)
## @see LoKi::Cuts::AM3
AM3           = AMASS(3)
## @see LoKi::Cuts::AM34
AM34          = AMASS(3,4)
## @see LoKi::Cuts::AM4
AM4           = AMASS(4)
## @see LoKi::Cuts::AMOM
AMOM          = LoKi.AParticles.Momentum
## @see LoKi::Cuts::AP
AP            = AMOM(0)
## @see LoKi::Cuts::AP0
AP0           = AMOM(0)
## @see LoKi::Cuts::AP0
AP1           = AMOM(1)
## @see LoKi::Cuts::AP0
AP2           = AMOM(2)
## @see LoKi::Cuts::AP0
AP3           = AMOM(3)
## @see LoKi::Cuts::AP0
AP4           = AMOM(4)
## @see LoKi::Cuts::AMOMT
AMOMT         = LoKi.AParticles.TransverseMomentum
## @see LoKi::Cuts::APT
APT           = AMOMT(0)
## @see LoKi::Cuts::APT0
APT0          = AMOMT(0)
## @see LoKi::Cuts::APT0
APT1          = AMOMT(1)
## @see LoKi::Cuts::APT0
APT2          = AMOMT(2)
## @see LoKi::Cuts::APT0
APT3          = AMOMT(3)
## @see LoKi::Cuts::APT0
APT4          = AMOMT(4)
## @see LoKi::Cuts::AETAP
AETAP         = LoKi.AParticles.Eta 
## @see LoKi::Cuts::AETA
AETA          = AETAP()
## @see LoKi::Cuts::AMINCHILD
AMINCHILD     = LoKi.AParticles.MinChild
## @see LoKi::Cuts::AMAXCHILD
AMAXCHILD     = LoKi.AParticles.MaxChild
## @see LoKi::Cuts::AHASCHILD
AHASCHILD     = LoKi.AParticles.HasChild
## @see LoKi::Cuts::ACHILD
ACHILD        = LoKi.AParticles.ChildFun
## @see LoKi::Cuts::ACHILDFUN
ACHILDFUN     = LoKi.AParticles.ChildFun
## @see LoKi::Cuts::AFUNCHILD
AFUNCHILD     = LoKi.AParticles.ChildFun
## @see LoKi::Cuts::ACHILDCUT
ACHILDCUT     = LoKi.AParticles.ChildCut
## @see LoKi::Cuts::ACUTCHILD
ACUTCHILD     = LoKi.AParticles.ChildCut

## @see LoKi::Cuts::AVCHI2
AVCHI2        = LoKi.AParticles.VertexChi2
## @see LoKi::Cuts::ACHI2V
ACHI2V        = LoKi.AParticles.VertexChi2
## @see LoKi::Cuts::ACHI2VX
ACHI2VX       = LoKi.AParticles.VertexChi2

## @see LoKi::Cuts::AMAXDOCA
AMAXDOCA      = LoKi.AParticles.MaxDOCA
## @see LoKi::Cuts::ADOCAMAX
ADOCAMAX      = LoKi.AParticles.MaxDOCA

## @see LoKi::Cuts::ACHI2DOCA
ACHI2DOCA     = LoKi.AParticles.MaxDOCAChi2
## @see LoKi::Cuts::ADOCACHI2
ADOCACHI2     = LoKi.AParticles.MaxDOCAChi2

## @see LoKi::Cuts::ACUTDOCA
ACUTDOCA      = LoKi.AParticles.MaxDOCACut
## @see LoKi::Cuts::ADOCACUT
ADOCACUT      = LoKi.AParticles.MaxDOCACut

## @see LoKi::Cuts::ACUTDOCACHI2
ACUTDOCACHI2  = LoKi.AParticles.MaxDOCAChi2Cut
## @see LoKi::Cuts::ADOCACHI2CUT
ADOCACHI2CUT  = LoKi.AParticles.MaxDOCAChi2Cut





# =============================================================================
# The END 
# =============================================================================
