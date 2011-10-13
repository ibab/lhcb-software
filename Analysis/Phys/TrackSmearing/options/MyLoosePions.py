#!/usr/bin/env python
# =============================================================================
# $Id: StdLoosePions.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdLoosePions.py
#  configuration file for 'Standard Loose Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'MyLoosePions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'MyLoosePions'  ,
                                     Particle =  'pion'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

## add this line,
## this corresonds to the output of the smearing tool 'Rec/ProtoP/' is hard coded
algorithm.Input = 'Rec/ProtoP/Smeared'
##

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Pion' )
fltr.Selection = [ "" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
MyLoosePions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
