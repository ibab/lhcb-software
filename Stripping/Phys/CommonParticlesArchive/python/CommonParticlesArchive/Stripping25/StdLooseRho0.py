#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseRho0.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $
# =============================================================================
## @file  CommonParticles/StdLooseRho0.py
#  configuration file for 'Standard Loose Rho0'
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose '
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseRho0' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm
StdLooseRho0 = CombineParticles ( 'StdLooseRho0' )

StdLooseRho0.Inputs  = [ "Phys/StdLoosePions/Particles" ]
StdLooseRho0.DecayDescriptor = "rho(770)0 -> pi+ pi-"
StdLooseRho0.CombinationCut = "(ADAMASS('rho(770)0')<100*MeV)& (ADOCACHI2CUT(15, ''))"
StdLooseRho0.MotherCut = "(BPVVDZ>0) & (VFASPF(VCHI2)<9) & (BPVDIRA>0.95) & (BPVVDCHI2>25)"

## configure Data-On-Demand service
locations = updateDoD ( StdLooseRho0 )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

# =============================================================================
# The END
# =============================================================================
