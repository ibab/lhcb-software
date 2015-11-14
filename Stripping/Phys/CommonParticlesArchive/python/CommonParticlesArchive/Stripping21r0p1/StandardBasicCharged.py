#!/usr/bin/env python
# =============================================================================
# $Id: StandardBasicCharged.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StandardBasicCharged.py
#  configuration file for 'Standard BasicCharged' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Basic Charged' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================

_locations = {} 

from CommonParticles.StandardPions     import *
_locations.update ( locations )

from CommonParticles.StandardKaons     import *
_locations.update ( locations )

from CommonParticles.StandardMuons     import *
_locations.update ( locations )

from CommonParticles.StandardElectrons import *
_locations.update ( locations )

from CommonParticles.StandardProtons   import *
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


