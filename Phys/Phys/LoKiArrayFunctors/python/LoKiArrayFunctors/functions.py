#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# $URL$
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
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#   ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#    
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-06-09
#
#  $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The set of basic objects from LoKiTrack library

This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

$Revision$
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__version__ = " $Revision$ "
__date__    = " 2007-06-09 "
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

# Namespaces:
cpp      = _LoKiCore.cpp
std      = cpp.std
LoKi     = cpp.LoKi
LHCb     = cpp.LHCb
Gaudi    = cpp.Gaudi

_x_R = 'Gaudi::Range_<std::vector<const LHCb::Particle*> >'
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
ATRUE    = LoKi.Constant ( _T , bool ) ( True  )
AFALSE   = LoKi.Constant ( _T , bool ) ( False )
AALL     = ATRUE 
ANONE    = AFALSE 


## @see LoKi::Cuts::ACHI2DOCA
ACHI2DOCA     = LoKi.AParticles.DOCAChi2

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
## @see LoKi::Cuts::ADAMASS
ADAMASS       = LoKi.AParticles.AbsDeltaMass 
## @see LoKi::Cuts::ADOCA
ADOCA         = LoKi.AParticles.DOCA
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
## @see LoKi::Cuts::APHI
APHI          = LoKi.AParticles.Phi
## @see LoKi::Cuts::APHI0
APHI0         = LoKi.AParticles.Phi ( 0 )
## @see LoKi::Cuts::AY
AY            = LoKi.AParticles.Y
## @see LoKi::Cuts::AY0
AY0           = LoKi.AParticles.Y ( 0 )
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
## @see LoKi::Cuts::ALV
ALV           = LoKi.AParticles.DeltaAngle
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

## @see LoKi::Cuts::ASUM
ASUM          = LoKi.AParticles.Sum


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

## @see LoKi::Cuts::DAMASS
DAMASS        = LoKi.AParticles.DeltaMass 

## @see LoKi::Cuts::ADOCAMIN
ADOCAMIN      = LoKi.AParticles.MinDOCA
## @see LoKi::Cuts::AMINDOCA
AMINDOCA      = LoKi.AParticles.MinDOCA

## @see LoKi::Cuts::AALLSAMEBPV_
AALLSAMEBPV_  = LoKi.AParticles.AllSameBestPV
## @see LoKi::Cuts::AALLSAMEBPV
AALLSAMEBPV   = LoKi.AParticles.AllSameBestPV ()

## @see LoKi::Cuts::AOVERLAP 
AOVERLAP   = LoKi.AParticles.Overlap 


## @see LoKi::Cuts::ACUTV 
ACUTV         = LoKi.AParticles.ACutV
## @see LoKi::Cuts::AFUNV 
AFunV         = LoKi.AParticles.AFunV


AVAL_MIN   = LoKi.AParticles.MinVal
AVAL_MAX   = LoKi.AParticles.MaxVal

## functional part
## _va = 'std::vector<LoKi::Range_<std::vector<const LHCb::Particle*> > >' ## std.vector ( _T    )
## _va = std.vector ( _T    )
## _vd = 'std::vector<double>'                                             ## std.vector ('double')
##
##AMaps      = LoKi.Functor             ( _va , _vd       )
##AMap       = LoKi.FunctorFromFunctor  ( _va , _vd       )
##APipes     = LoKi.Functor             ( _va , _va       )
##APipe      = LoKi.FunctorFromFunctor  ( _va , _va       )
##AFunVals   = LoKi.Functor             ( _va , 'double'  )
##AFunVal    = LoKi.FunctorFromFunctor  ( _va , 'double'  )
##ACutVals   = LoKi.Functor             ( _va ,  bool     )
##ACutVal    = LoKi.FunctorFromFunctor  ( _va ,  bool     )
#

# ==============================================================================
## Find the daughter particle, which maximizes the value of 'cmp'-functor
#     and evaluate for it 'val' functor
def aval_max ( val , cmp , *args ) :
    """
    Find the daughter particle, which maximased the valeu of 'cmp'-functor
    and evaluate for it 'val' functor

    Evaluate P for the child with max PT:
    
    >>> fun = aval_max ( P , PT )

    Evaluate P for the basic child with max PT:
    
    >>> fun = aval_max ( P , PT , BASIC &  ( 0 != Q ) , -1.e+10 ) 
    
    """
    return LoKi.AParticles.MaxVal ( val , cmp , *args )


# ==============================================================================
## Find the daughter particle, which minimizes the value of 'cmp'-functor
#     and evaluate for it 'val' functor
def aval_min ( val , cmp , *args ) :
    """
    Find the daughter particle, which minimizes the valeu of 'cmp'-functor
    and evaluate for it 'val' functor

    Evaluate P for the child with min PT:
    
    >>> fun = aval_min ( P , PT )

    Evaluate P for the basic child with min PT:
    
    >>> fun = aval_min ( P , PT , BASIC &  ( 0 != Q ) , 1.e+20 ) 
    
    """
    return LoKi.AParticles.MinVal ( val , cmp , *args )


# =============================================================================
## Trivial function which returns the PDG mass for the given 'object'    
def PDGM ( o ) :
    """
    Trivial function which returns the PDG mass for the given 'object'
    
    >>> m = PDGM('B0')
    >>> m = PDGM(LHCb.ParticleID(22))
    >>> m = PGDM(22)
    >>> particle = ...
    >>> m = PDGM( particle )
    >>> mcparticle = ...
    >>> m = PDGM( mcparticle )
    >>> genparticle = ...
    >>> m = PDGM( genparticle )
    
    """
    if   str == type ( o ) :
        return LoKi.Particles.massFromName ( o )
    elif LHCb.ParticleID == type ( o ) :
        return LoKi.Particles.massFromPID  ( o )
    elif hassattr ( o , "particleID" ) :
        return LoKi.Particles.massFromPID  ( o.particleID() )
    elif hassattr ( o , "pdg_id" ) :
        return LoKi.Particles.massFromPID  ( LHCb.ParticleID( o.pdg_id() ) ) 
    
    return PDGM ( LHCb.PartielID( o ) ) 

PDGM. __doc__ += '\n' + LoKi.Particles.massFromName . __doc__ 
PDGM. __doc__ += '\n' + LoKi.Particles.massFromPID  . __doc__

# =============================================================================
if '__main__' == __name__ :
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
