#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsVeloElectrons.py,v 1.1 2010-08-11 12:43:05 ukerzel Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsElectrons.py
#  configuration file for 'Standard NoPIDs Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'NoBrem NoPIDs Electrons with Ttrack tracks'
"""
__author__  = "Ulrich Kerzel Ulrich.Kerzel@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'NoBremNoPIDsTtrackElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'NoBremNoPIDsTtrackElectrons',
                                   DecayDescriptor = 'Electron' ,
                                   Particle = 'electron'  )

algorithm.AddBremPhotonTo = [] 

# configure the track selector
selector = trackSelector ( algorithm,trackTypes = [ "Ttrack" ]  ) 

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
NoBremNoPIDsTtrackElectrons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
