#!/usr/bin/env python
# =============================================================================
# $Id: StandardProtons.py,v 1.2 2009-02-26 16:53:14 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StandardProtons.py
#  configuration file for 'Standard Protons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Protons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdAllNoPIDsProtons import *
_locations.update ( locations )

from CommonParticles.StdNoPIDsProtons import *
_locations.update ( locations )

from CommonParticles.StdNoPIDsDownProtons import *
_locations.update ( locations )

from CommonParticles.StdAllLooseProtons  import *
_locations.update ( locations )

from CommonParticles.StdAllLooseANNProtons  import *
_locations.update ( locations )

from CommonParticles.StdLooseProtons  import *
_locations.update ( locations )

from CommonParticles.StdLooseANNProtons  import *
_locations.update ( locations )

from CommonParticles.StdTightProtons  import *
_locations.update ( locations )

from CommonParticles.StdTightANNProtons  import *
_locations.update ( locations )

from CommonParticles.StdLooseANNDownProtons  import *
_locations.update ( locations )

from CommonParticles.StdTightANNDownProtons  import *
_locations.update ( locations )

from CommonParticles.StdLooseANNUpProtons  import *
_locations.update ( locations )

from CommonParticles.StdTightANNUpProtons  import *
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


