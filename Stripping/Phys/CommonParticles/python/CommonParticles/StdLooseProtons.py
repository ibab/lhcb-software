#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseProtons.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseProtons.py
#  configuration file for 'Standard Loose Protons' 
#  @author Patrick Koppenburg 
#  @date 2011-07-18
# =============================================================================
"""
Configuration file for 'Standard Loose Protons wioth loose cuts'
"""
__author__  = "Patrick Koppenburg "
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdLooseProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  FilterDesktop( 'StdLooseProtons',
                            Inputs = ["Phys/StdAllLooseProtons/Particles"],
                            Code = defaultCuts() )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseProtons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
