#!/usr/bin/env python
# =============================================================================
# $Id: StandardBasic.py,v 1.4 2009-10-09 12:41:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StandardBasic.py
#  configuration file for 'Standard Basic' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Basic' particles 
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================

_locations = {} 

from CommonParticles.StdParticleFlow import *
_locations.update ( locations )

from CommonParticles.StdJets import *
_locations.update ( locations )

from CommonParticles.StdJetsR07 import *
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


