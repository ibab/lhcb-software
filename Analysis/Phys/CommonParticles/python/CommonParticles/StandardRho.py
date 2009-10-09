#!/usr/bin/env python
# =============================================================================
# $Id: StandardRho.py,v 1.1 2009-10-09 12:39:26 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StandardRho.py
#  configuration file for 'Standard Rho' 
#  @author Patrick Koppenburg
#  @date 2009-02-28
# =============================================================================
"""
Configuration file for 'Standard Rho'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdLooseRho0 import *
_locations.update ( locations )
from CommonParticles.StdLooseRhoPlus import *
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


