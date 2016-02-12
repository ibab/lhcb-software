#!/usr/bin/env python
# =============================================================================
# $Id: StandardLambdac.py,v 1.2 2010-09-03 20:59:11 roldeman Exp $ 
# =============================================================================
## @file  CommonParticles/StandardDplus.py
#  configuration file for 'Standard Lambda_c' 
#  @author Patrick Koppenburg
#  @date 2009-02-28
# =============================================================================
"""
Configuration file for 'Standard Lambda_c'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdLooseLambdac import *
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


