#!/usr/bin/env python
# =============================================================================
# $Id: StandardMCD0.py,v 1.4 2009-10-09 12:41:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonMCParticles/StandardMCD0.py
#  configuration file for 'Standard MC D0' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard MC D0' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl, Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================

_locations = {} 

from CommonMCParticles.StdMCD02HH import *
_locations.update ( locations )

from CommonMCParticles.StdMCD02Nbody import *
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


