#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdLooseANNKaons.py
#  configuration file for 'Standard Loose Kaons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Loose Kaons with loose ANN cuts'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdLooseANNKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop
from CommonParticles.Utils import *

## create the algorithm 
algorithm = FilterDesktop( 'StdLooseANNKaons',
                           Inputs = ["Phys/StdAllLooseANNKaons/Particles"],
                           Code = defaultCuts() )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseANNKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
