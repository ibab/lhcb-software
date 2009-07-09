#!/usr/bin/env python
# =============================================================================
# $Id: StandardDiMuon.py,v 1.3 2009-07-09 12:42:31 rlambert Exp $ 
# =============================================================================
## @file  CommonParticles/StdLoose.py
#  configuration file for 'Standard Loose DiMuon' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose DiMuon'
"""
__author__  = "Rob Lambert"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
from Gaudi.Configuration import *

## ============================================================================
## create the algorithm 

_locations = {} 

from CommonParticles.StdLooseDiMuon import *
_locations.update ( locations )

# redefine the locations 
locations = _locations

## configure Data-On-Demand service 
#locations = updateDoD ( StdLoose )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    
    from CommonParticles.Utils import locationsDoD
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
