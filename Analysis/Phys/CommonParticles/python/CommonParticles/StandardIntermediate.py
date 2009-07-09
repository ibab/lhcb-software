#!/usr/bin/env python
# =============================================================================
# $Id: StandardIntermediate.py,v 1.5 2009-07-09 15:23:57 rlambert Exp $ 
# =============================================================================
## @file  CommonParticles/StandardIntermediates.py
#  configuration file for 'Standard Intermediate particles
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Intermediate' particles 
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================

_locations = {} 

from CommonParticles.StandardD0 import *
_locations.update ( locations )
from CommonParticles.StandardDstar import *
_locations.update ( locations )
from CommonParticles.StandardJpsi import *
_locations.update ( locations )
from CommonParticles.StandardDiMuon import *
_locations.update ( locations )

from CommonParticles.StdLooseDplus import *
_locations.update ( locations )
from CommonParticles.StdLooseDstarWithD2HH import *
_locations.update ( locations )
from CommonParticles.StdLooseKs import *
_locations.update ( locations )
from CommonParticles.StdLooseKstar import *
_locations.update ( locations )
from CommonParticles.StdLooseDetachedKstar import *
_locations.update ( locations )
from CommonParticles.StdLoosePhi import *
_locations.update ( locations )
from CommonParticles.StdLooseDetachedPhi import *
_locations.update ( locations )
from CommonParticles.StdLooseRho0 import *
_locations.update ( locations )
from CommonParticles.StdLooseRhoPlus import *
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


