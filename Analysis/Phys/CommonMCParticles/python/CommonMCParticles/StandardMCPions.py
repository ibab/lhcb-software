#!/usr/bin/env python
# =============================================================================
# $Id: StandardMCPions.py,v 1.4 2009-10-09 12:41:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonMCParticles/StandardMCPions.py
#  configuration file for 'Standard MC Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard MC Pions' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================

_locations = {} 

from CommonMCParticles.StdMCPions import *
_locations.update ( locations )

from CommonMCParticles.StdMCDownPions import *
_locations.update ( locations )

# redefine the locations 
locations = _locations

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


