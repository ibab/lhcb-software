#!/usr/bin/env python
# =============================================================================
# $Id: StandardEta.py,v 1.1 2010-01-06 11:04:12 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StandardEta.py
#  configuration file for 'Standard Eta' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Eta'
"""
__author__  = "Patrick Koppenburg Patrick.Koppenburg@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdLooseResolvedEta import *
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


