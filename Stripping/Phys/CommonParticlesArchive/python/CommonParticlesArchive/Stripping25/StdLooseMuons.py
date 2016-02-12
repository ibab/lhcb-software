#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseMuons.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseMuons.py
#  configuration file for 'Standard Loose Muons' 
#  @author Patrick Koppenburg 
#  @date 2011-07-18
# =============================================================================
"""
Configuration file for 'Standard Loose Muons wioth loose cuts'
"""
__author__  = "Patrick Koppenburg "
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdLooseMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  FilterDesktop( 'StdLooseMuons',
                            Inputs = ["Phys/StdAllLooseMuons/Particles"],
                            Code = defaultCuts() )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseMuons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
