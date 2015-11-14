#!/usr/bin/env python
# =============================================================================
# $Id: StandardDplus.py,v 1.2 2009-12-14 11:45:32 poluekt Exp $ 
# =============================================================================
## @file  CommonParticles/StandardDplus.py
#  configuration file for 'Standard D+' 
#  @author Patrick Koppenburg
#  @date 2009-02-28
# =============================================================================
"""
Configuration file for 'Standard D+'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdLooseDplus import *
_locations.update ( locations )


from CommonParticles.StdTightDplus import *
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


