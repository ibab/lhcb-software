#!/usr/bin/env python
# =============================================================================
# $Id: StandardPions.py,v 1.3 2009-10-08 12:37:06 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StandardPions.py
#  configuration file for 'Standard Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdAllNoPIDsPions import *
_locations.update ( locations )

from CommonParticles.StdNoPIDsPions import *
_locations.update ( locations )

from CommonParticles.StdNoPIDsDownPions import *
_locations.update ( locations )

from CommonParticles.StdNoPIDsUpPions import *
_locations.update ( locations )

from CommonParticles.StdAllLoosePions import *
_locations.update ( locations )

from CommonParticles.StdLoosePions import *
_locations.update ( locations )

from CommonParticles.StdTightPions import *
_locations.update ( locations )

from CommonParticles.StdAllLooseANNPions import *
_locations.update ( locations )

from CommonParticles.StdLooseANNPions import *
_locations.update ( locations )

from CommonParticles.StdTightANNPions import *
_locations.update ( locations )

from CommonParticles.StdLooseANNDownPions import *
_locations.update ( locations )

from CommonParticles.StdTightANNDownPions import *
_locations.update ( locations )

from CommonParticles.StdLooseANNUpPions import *
_locations.update ( locations )

from CommonParticles.StdTightANNUpPions import *
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


