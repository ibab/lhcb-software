#!/usr/bin/env python
# =============================================================================
# $Id: StandardDstar.py,v 1.3 2010-01-18 10:08:49 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StandardDstar.py
#  configuration file for 'Standard D*' 
#  @author Patrick Koppenburg
#  @date 2009-02-28
# =============================================================================
"""
Configuration file for 'Standard D*'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================

_locations = {}

from CommonParticles.StdLooseDstarWithD2HH import *
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


