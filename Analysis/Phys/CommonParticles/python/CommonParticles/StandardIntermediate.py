#!/usr/bin/env python
# =============================================================================
# $Id: StandardIntermediate.py,v 1.7 2009-12-09 16:58:47 pkoppenb Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $"
# =============================================================================

_locations = {} 

from CommonParticles.StandardV0 import *
_locations.update ( locations )
from CommonParticles.StandardKstar import *
_locations.update ( locations )
from CommonParticles.StandardRho import *
_locations.update ( locations )
from CommonParticles.StandardPhi import *
_locations.update ( locations )
from CommonParticles.StandardD0 import *
_locations.update ( locations )
from CommonParticles.StandardDplus import *
_locations.update ( locations )
from CommonParticles.StandardDstar import *
_locations.update ( locations )
from CommonParticles.StandardJpsi import *
_locations.update ( locations )
from CommonParticles.StandardDiMuon import *
_locations.update ( locations )
from CommonParticles.StandardDiElectron import *
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


