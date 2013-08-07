#!/usr/bin/env python
# =============================================================================
# $Id: StandardMCIntermediate.py,v 1.8 2009-12-14 11:45:32 poluekt Exp $ 
# =============================================================================
## @file  CommonMCParticles/StandardMCMCIntermediates.py
#  configuration file for 'StandardMC MC Intermediate particles
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'StandardMC MC Intermediate' particles 
"""
__author__  = "Patrick Koppenburg, Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================

_locations = {} 

from CommonMCParticles.StandardMCKstar import *
_locations.update ( locations )
from CommonMCParticles.StandardMCPhi import *
_locations.update ( locations )
from CommonMCParticles.StandardMCD0 import *
_locations.update ( locations )
from CommonMCParticles.StandardMCDplus import *
_locations.update ( locations )
from CommonMCParticles.StandardMCDsplus import *
_locations.update ( locations )
from CommonMCParticles.StandardMCDstar import *
_locations.update ( locations )
from CommonMCParticles.StandardMCJpsi import *
_locations.update ( locations )
from CommonMCParticles.StandardMCTau import *
_locations.update ( locations )

# redefine the locations 
locations = _locations

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__    
    from CommonMCParticles.Utils import locationsDoD
    print locationsDoD ( locations )
    
# =============================================================================
# The END 
# =============================================================================


