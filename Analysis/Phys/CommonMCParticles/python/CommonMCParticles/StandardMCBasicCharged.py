#!/usr/bin/env python
# =============================================================================
# $Id: StandardBasicCharged.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
# =============================================================================
## @file  CommonMCParticles/StandardMCBasicCharged.py
#  configuration file for 'Standard MC BasicCharged' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard MC Basic Charged' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nli, Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================

_locations = {} 

from CommonMCParticles.StandardMCPions     import *
_locations.update ( locations )

from CommonMCParticles.StandardMCKaons     import *
_locations.update ( locations )

from CommonMCParticles.StandardMCMuons     import *
_locations.update ( locations )

from CommonMCParticles.StandardMCElectrons import *
_locations.update ( locations )

from CommonMCParticles.StandardMCProtons   import *
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


