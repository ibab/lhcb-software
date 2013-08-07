#!/usr/bin/env python
# =============================================================================
# $Id: StandardMCBasic.py,v 1.4 2009-10-09 12:41:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonMCParticles/StandardMCBasic.py
#  configuration file for 'Standard MC Basic' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'StandardMC MC Basic' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================

_locations = {} 

from CommonMCParticles.StandardMCBasicCharged import *
_locations.update ( locations )

from CommonMCParticles.StandardMCBasicNeutral import *
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


