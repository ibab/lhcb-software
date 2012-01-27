#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsElectrons.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsElectrons.py
#  configuration file for 'Standard NoPIDs Electrons' 
#  @author Patrick Koppenburg 
#  @date 2011-07-18
# =============================================================================
"""
Configuration file for 'No Brem NoPIDs Electrons wioth loose cuts'
"""
__author__  = "Patrick Koppenburg "
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'NoBremNoPIDsElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import NoPIDsParticleMaker 
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'NoBremNoPIDsElectrons'    ,
                                   DecayDescriptor = 'Electron' ,
                                   Particle = 'electron'  )

selector = trackSelector ( algorithm , trackTypes = [ "Long" ])

algorithm.AddBremPhotonTo = []

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
NoBremNoPIDsElectrons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
