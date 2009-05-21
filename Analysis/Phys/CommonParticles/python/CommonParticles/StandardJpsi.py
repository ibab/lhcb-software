#!/usr/bin/env python
# =============================================================================
# $Id: StandardJpsi.py,v 1.2 2009-05-21 18:59:53 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StandardJpsi.py
#  configuration file for 'Standard Jpsi' 
#  @author Patrick Koppenburg
#  @date 2009-02-28
# =============================================================================
"""
Configuration file for 'Standard Jpsi'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdLooseJpsi2MuMu import *
_locations.update ( locations )

from CommonParticles.StdLooseJpsi2ee import *
_locations.update ( locations )

from CommonParticles.StdLTUnbiasedJpsi2MuMu import *
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


