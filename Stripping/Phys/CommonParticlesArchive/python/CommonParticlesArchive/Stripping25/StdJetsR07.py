#!/usr/bin/env python
# =============================================================================
# $Id: StdAllNoPIDsPions.py,v 1.5 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdAllNoPIDsPions.py
#  configuration file for 'Standard NoPIDs Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdJets' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from CommonParticles.Utils import *

from JetAccessories.JetMaker_Config import JetMakerConf
algorithm = JetMakerConf("StdJetsR07", R=0.7).algorithms[0]
## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdJets = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
