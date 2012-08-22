#!/usr/bin/env python
# =============================================================================
# $Id: StandardDipion.py,v1.0 2012-08-07 14:44:42 elsasser Exp $
# =============================================================================
## @file  CommonParticles/StandardDipion.py
#  configuration file for 'Standard Dipion' 
#  @author Donal Hill, Conor Fitzpatrick, Christian Elsasser
#  @date 2012-08-07
# =============================================================================
"""
Configuration file for 'Standard Dipion'
"""
__author__  = "Donal Hill, Conor Fitzpatrick, Christian Elsasser"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.0 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdLooseDipion import *
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


