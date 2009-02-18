#!/usr/bin/env python
# =============================================================================
# $Id: StandardDiMuon.py,v 1.1 2009-02-18 15:29:29 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLoose.py
#  configuration file for 'Standard Loose ' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose '
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLoose' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLoose = CombineParticles ( 'StdLoose' )


## configure Data-On-Demand service 
locations = updateDoD ( StdLoose )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
