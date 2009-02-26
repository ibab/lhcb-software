#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsDownProtons.py,v 1.1 2009-02-26 16:53:33 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsDownProtons.py
#  configuration file for 'Standard NoPIDs Downstream Protons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Downstream Protons'
"""
__author__  = "Alessio Sarti <Alessio.Sarti@lnf.infn.it>"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdNoPIDsDownProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import PreLoadParticles 
from Configurables         import NoPIDsParticleMaker

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdNoPIDsDownProtons'         ,
                                DecayDescriptor = 'Proton' )

# configure particle maker: 
maker  = particleMaker ( algorithm , NoPIDsParticleMaker ) 
maker.Particle = 'proton'  
maker.UseLongTracks = False;
maker.UseDownstreamTracks = True;

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsDownProtons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
