#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDalitzPi0.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDalitzPi0.py
#  configuration file for 'Standard Loose Dalitz Pi0s and Pi0 with photon conversion' 
#  @author Olivier Deschamps
#  @date 2014-05-20
# =============================================================================
"""
Configuration file for 'Standard Loose DalitzPi0'
"""
__author__  = "Olivier Deschamps"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLoosePi02gee'      ,
    'StdLoosePi024e'      ,
    'StdLooseDalitzPi0' ,
    'locations'
    )
# =============================================================================
from GaudiKernel.SystemOfUnits import MeV
from CommonParticles.Utils     import *
from Configurables import CombineParticles

## create the algorithm  : pi0 -> gg(ee) (1 conversion)
pi02gee =  CombineParticles( "StdLoosePi02gee", DecayDescriptor = 'pi0 -> gamma gamma' )
pi02gee.Inputs = ["Phys/StdLooseAllPhotons/Particles","Phys/StdAllLooseGammaLL/Particles","Phys/StdAllLooseGammaDD/Particles"]
pi02gee.ParticleCombiners.update( {"" : "ParticleAdder"} )
pi02gee.Preambulo += ["cnv = switch(INTES('StdLooseAllPhotons',False),1,0)"]
pi02gee.MotherCut = " (MM  < 170*MeV) & (MM  > 90*MeV) & (1 == CHILD(1,cnv)+CHILD(2,cnv) )" 
locations      = updateDoD ( pi02gee )
StdLoosePi02gee=pi02gee

## create the algorithm :  pi0 -> g(ee)g(ee)  (2 conversions)
pi024e =  CombineParticles( "StdLoosePi024e", DecayDescriptor = 'pi0 -> gamma gamma' )
pi024e.Inputs = ["Phys/StdAllLooseGammaLL","Phys/StdAllLooseGammaDD/Particles"]
pi024e.ParticleCombiners.update( {"" : "ParticleAdder"} )
#pi024e.ParticleCombiners.update( {"" : "LoKi::VertexFitter"} )
pi024e.MotherCut = " (MM  < 170*MeV) & (MM  > 90*MeV)"
locations.update( updateDoD ( pi024e  ) )
StdLoosePi024e=pi024e

## create the algorithm :  pi0 -> gee (Dalitz decay)  - note : also contains Long-Long contribution to 1  conversion gg(ee) 
from CommonParticles.StdLooseDiElectron    import *
pi0Dalitz =  CombineParticles( "StdLooseDalitzPi0", DecayDescriptor = 'pi0 -> gamma gamma' )
pi0Dalitz.Inputs = ["Phys/StdLooseAllPhotons/Particles","Phys/StdDiElectronGamma/Particles"]
pi0Dalitz.CombinationCut = "(AM < 200*MeV) & (1 == ACHILD(1,cnv)+ACHILD(2,cnv) )"
pi0Dalitz.Preambulo += ["cnv = switch(INTES('StdLooseAllPhotons',False),1,0)"]
pi0Dalitz.MotherCut = " (MM  < 170*MeV) & (MM  > 90*MeV)" 
locations.update( updateDoD ( pi0Dalitz        ) )
StdLooseDalitzPi0=pi0Dalitz

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
