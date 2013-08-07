#!/usr/bin/env python
# =============================================================================
# $Id: StandardBasicNeutral.py,v 1.3 2010-01-06 11:04:12 pkoppenb Exp $ 
# =============================================================================
## @file  CommonMCParticles/StandardBasicNeutral.py
#  configuration file for 'Standard Basic Neutral' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard MC Basic Neutral' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================

_locations = {} 

from CommonMCParticles.StandardMCPhotons import *
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


