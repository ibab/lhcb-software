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

## @see LoKi::Cuts::ANUM
ANUM          = LoKi.AParticles.Count  
## @see LoKi::Cuts::AUNIQUE
AUNIQUE       = LoKi.AParticles.Unique 

## @see LoKi::Cuts::AMASS
AMASS         = LoKi.AParticles.InvariantMass 
## @see LoKi::Cuts::AMASS
AM            = AMASS()
## @see LoKi::Cuts::AM12
AM12          = AMASS(1,2)
## @see LoKi::Cuts::AM13
AM13          = AMASS(1,3)
## @see LoKi::Cuts::AM14
AM14          = AMASS(1,4)
## @see LoKi::Cuts::AM23
AM23          = AMASS(2,3)
## @see LoKi::Cuts::AM34
AM24          = AMASS(2,4)
## @see LoKi::Cuts::AM34
AM34          = AMASS(3,4)
## @see LoKi::Cuts::AMOM
AMOM          = LoKi.AParticles.Momentum
## @see LoKi::Cuts::AP
AP            = AMOM()
## @see LoKi::Cuts::AMOMT
AMOMT         = LoKi.AParticles.TransverseMomentum
## @see LoKi::Cuts::APT
APT           = AMOMT()
## @see LoKi::Cuts::AETAP
AETAP         = LoKi.AParticles.Eta 
## @see LoKi::Cuts::AETA
AETA          = AETAP()


# =============================================================================
# The END 
# =============================================================================
