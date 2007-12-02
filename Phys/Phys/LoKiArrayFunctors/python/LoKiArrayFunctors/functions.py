#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.8 2007-12-02 17:10:45 ibelyaev Exp $ 
# =============================================================================
## @file
#  The set of basic objects from LoKiHlt library
#  The file is a part of LoKi and Bender projects
#
#  This file is a part of LoKi project - 
#   'C++ ToolKit  for Smart and Friendly Physics Analysis'
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
"""
The set of basic objects from LoKiTrack library

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

# Namespaces:
_global  = _LoKiCore._global 
std      = _global.std
LoKi     = _global.LoKi
LHCb     = _global.LHCb

_x_R = 'LoKi::Range_<std::vector<const LHCb::Particle*> >'
_T   = _x_R

# =============================================================================
## "Ordinary" functions for Tracks: "Tr"
# =============================================================================

## @see LoKi::Types::TrFunc
AFunc  = LoKi.Functor               ( _T , 'double' ) 
## @see LoKi::Types::TrCuts
ACuts  = LoKi.Functor               ( _T ,  bool    ) 
## @see LoKi::Types::TrFun
AFun   = LoKi.FunctorFromFunctor    ( _T , 'double' ) 
## @see LoKi::Types::TrCut
ACut   = LoKi.FunctorFromFunctor    ( _T ,  bool    )


# =============================================================================
## concrete functions:
# =============================================================================

## @see LoKi::Cuts::ACHI2DOCA
ACHI2DOCA     = LoKi.AParticles.MaxDOCAChi2

## @see LoKi::Cuts::ACHI2V
ACHI2V        = LoKi.AParticles.VertexChi2
## @see LoKi::Cuts::ACHI2VX
ACHI2VX       = LoKi.AParticles.VertexChi2
## @see LoKi::Cuts::ACHILD
ACHILD        = LoKi.AParticles.ChildFun
## @see LoKi::Cuts::ACHILDCUT
ACHILDCUT     = LoKi.AParticles.ChildCut
## @see LoKi::Cuts::ACHILDFUN
ACHILDFUN     = LoKi.AParticles.ChildFun
## @see LoKi::Cuts::ACUTCHILD
ACUTCHILD     = LoKi.AParticles.ChildCut
## @see LoKi::Cuts::ACUTDOCA
ACUTDOCA      = LoKi.AParticles.MaxDOCACut
## @see LoKi::Cuts::ACUTDOCACHI2
ACUTDOCACHI2  = LoKi.AParticles.MaxDOCAChi2Cut
## @see LoKi::Cuts::ADOCACHI2
ADOCACHI2     = LoKi.AParticles.MaxDOCAChi2
## @see LoKi::Cuts::ADOCACHI2CUT
ADOCACHI2CUT  = LoKi.AParticles.MaxDOCAChi2Cut
## @see LoKi::Cuts::ADOCACUT
ADOCACUT      = LoKi.AParticles.MaxDOCACut
## @see LoKi::Cuts::ADOCAMAX
ADOCAMAX      = LoKi.AParticles.MaxDOCA
## @see LoKi::Cuts::AETA
AETA          = LoKi.AParticles.Eta ( 0 )
## @see LoKi::Cuts::AETA0
AETA0         = LoKi.AParticles.Eta ( 0 )
## @see LoKi::Cuts::AETAP
AETAP         = LoKi.AParticles.Eta 
## @see LoKi::Cuts::AFUNCHILD
AFUNCHILD     = LoKi.AParticles.ChildFun
## @see LoKi::Cuts::AHASCHILD
AHASCHILD     = LoKi.AParticles.HasChild
## @see LoKi::Cuts::ALV0
ALV0          = LoKi.AParticles.DecayAngle
## @see LoKi::Cuts::ALV01
ALV01         = LoKi.AParticles.DecayAngle ( 1 ) 
## @see LoKi::Cuts::ALV02
ALV02         = LoKi.AParticles.DecayAngle ( 2 ) 
## @see LoKi::Cuts::ALV03
ALV03         = LoKi.AParticles.DecayAngle ( 3 ) 
## @see LoKi::Cuts::ALV04
ALV04         = LoKi.AParticles.DecayAngle ( 4 ) 
## @see LoKi::Cuts::AM
AM            = LoKi.AParticles.InvariantMass ( 0 ) 
## @see LoKi::Cuts::AM0
AM0           = LoKi.AParticles.InvariantMass ( 0 )
## @see LoKi::Cuts::AM1
AM1           = LoKi.AParticles.InvariantMass ( 1 ) 
## @see LoKi::Cuts::AM12
AM12          = LoKi.AParticles.InvariantMass ( 1 , 2 ) 
## @see LoKi::Cuts::AM13
AM13          = LoKi.AParticles.InvariantMass ( 1 , 3 ) 
## @see LoKi::Cuts::AM14
AM14          = LoKi.AParticles.InvariantMass ( 1 , 4 ) 
## @see LoKi::Cuts::AM2
AM2           = LoKi.AParticles.InvariantMass ( 2 ) 
## @see LoKi::Cuts::AM23
AM23          = LoKi.AParticles.InvariantMass ( 2 , 3 ) 
## @see LoKi::Cuts::AM24
AM24          = LoKi.AParticles.InvariantMass ( 2 , 4 ) 
## @see LoKi::Cuts::AM3
AM3           = LoKi.AParticles.InvariantMass ( 3 ) 
## @see LoKi::Cuts::AM34
AM34          = LoKi.AParticles.InvariantMass ( 3 , 4 ) 
## @see LoKi::Cuts::AM4
AM4           = LoKi.AParticles.InvariantMass ( 4 ) 
## @see LoKi::Cuts::AMASS
AMASS         = LoKi.AParticles.InvariantMass 
## @see LoKi::Cuts::AMAXCHILD
AMAXCHILD     = LoKi.AParticles.MaxChild
## @see LoKi::Cuts::AMAXDOCA
AMAXDOCA      = LoKi.AParticles.MaxDOCA
## @see LoKi::Cuts::AMINCHILD
AMINCHILD     = LoKi.AParticles.MinChild
## @see LoKi::Cuts::AMOM
AMOM          = LoKi.AParticles.Momentum
## @see LoKi::Cuts::AMOMT
AMOMT         = LoKi.AParticles.TransverseMomentum
## @see LoKi::Cuts::ANUM
ANUM          = LoKi.AParticles.Count  
## @see LoKi::Cuts::AP
AP            = LoKi.AParticles.Momentum ( 0 )  
## @see LoKi::Cuts::AP0
AP0           = LoKi.AParticles.Momentum ( 0 )  
## @see LoKi::Cuts::AP1
AP1           = LoKi.AParticles.Momentum ( 1 )  
## @see LoKi::Cuts::AP2
AP2           = LoKi.AParticles.Momentum ( 2 )  
## @see LoKi::Cuts::AP3
AP3           = LoKi.AParticles.Momentum ( 3 )  
## @see LoKi::Cuts::AP4
AP4           = LoKi.AParticles.Momentum ( 4 )  

## @see LoKi::Cuts::APT
APT           = LoKi.AParticles.TransverseMomentum ( 0 )
## @see LoKi::Cuts::APT0
APT0          = LoKi.AParticles.TransverseMomentum ( 0 )
## @see LoKi::Cuts::APT1
APT1          = LoKi.AParticles.TransverseMomentum ( 1 )
## @see LoKi::Cuts::APT2
APT2          = LoKi.AParticles.TransverseMomentum ( 2 )
## @see LoKi::Cuts::APT3
APT3          = LoKi.AParticles.TransverseMomentum ( 3 )
## @see LoKi::Cuts::APT4
APT4          = LoKi.AParticles.TransverseMomentum ( 4 )

## @see LoKi::Cuts::ASAME
# ASAME         = LoKi.TheSame    ( _T )
## @see LoKi::Cuts::ASIZE
ASIZE         = LoKi.AParticles.Size   ()

## @see LoKi::Cuts::AUNIQUE
AUNIQUE       = LoKi.AParticles.Unique 
## @see LoKi::Cuts::AVCHI2
AVCHI2        = LoKi.AParticles.VertexChi2
## @see LoKi::Cuts::AVALID
AVALID        = LoKi.Valid(_T)         ()
## @see LoKi::Cuts::AWM
AWM           = LoKi.AParticles.WrongMass 
## @see LoKi::Cuts::AWRONGMASS
AWRONGMASS    = LoKi.AParticles.WrongMass 


## functional part
_va      = std.vector ( _T    )
_vd       = std.vector ('double')
#
AMaps      = LoKi.Functor             ( _va , _vd       )
AMap       = LoKi.FunctorFromFunctor  ( _va , _vd       )
APipes     = LoKi.Functor             ( _va , _va       )
APipe      = LoKi.FunctorFromFunctor  ( _va , _va       )
AFunVals   = LoKi.Functor             ( _va , 'double'  )
AFunVal    = LoKi.FunctorFromFunctor  ( _va , 'double'  )
AElements  = LoKi.Functor             ( _va , _T        ) 
AElement   = LoKi.FunctorFromFunctor  ( _va , _T        ) 
#




# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o        


# =============================================================================
# The END 
# =============================================================================
