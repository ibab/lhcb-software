#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdAllLooseANNElectrons.py
#  configuration file for 'Standard Loose ANN Electrons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Loose Electrons using ANN PID'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdAllLooseANNElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdAllLooseANNElectrons' ,
                                     Particle = 'electron' )

# configure the track selector
selector = trackSelector ( algorithm )

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter , 'Electron' )
#fltr.Selection = [ "RequiresDet='CALO' ProbNNe>'0.09'" ]
#fltr.Selection = [ "ProbNNe>'0.1'" ]
fltr.Selection = [ "RequiresDet='RICHORCALO' ProbNNe>'0.03'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllLooseANNElectrons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
