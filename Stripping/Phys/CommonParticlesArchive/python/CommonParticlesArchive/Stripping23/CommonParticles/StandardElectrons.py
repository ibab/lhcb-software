#!/usr/bin/env python
# =============================================================================
# $Id: StandardElectrons.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StandardElectrons.py
#  configuration file for 'Standard Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Electrons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdAllNoPIDsElectrons import *
_locations.update ( locations )

from CommonParticles.StdNoPIDsElectrons import *
_locations.update ( locations )

from CommonParticles.StdAllLooseElectrons import *
_locations.update ( locations )

from CommonParticles.StdLooseElectrons import *
_locations.update ( locations )

from CommonParticles.StdAllLooseANNElectrons import *
_locations.update ( locations )

from CommonParticles.StdLooseANNElectrons import *
_locations.update ( locations )

from CommonParticles.StdTightElectrons import *
_locations.update ( locations )

from CommonParticles.StdTightANNElectrons import *
_locations.update ( locations )

from CommonParticles.StdLooseANNDownElectrons import *
_locations.update ( locations )

from CommonParticles.StdTightANNDownElectrons import *
_locations.update ( locations )

from CommonParticles.StdLooseANNUpElectrons import *
_locations.update ( locations )

from CommonParticles.StdTightANNUpElectrons import *
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


