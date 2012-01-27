#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsKaons.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsKaons.py
#  configuration file for 'Standard NoPIDs Kaons' 
#  @author Patrick Koppenburg 
#  @date 2011-07-18
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Kaons with loose cuts'
"""
__author__  = "Patrick Koppenburg "
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdNoPIDsKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  FilterDesktop( 'StdNoPIDsKaons',
                            Inputs = ["Phys/StdAllNoPIDsKaons/Particles"],
                            Code = defaultCuts() )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsKaons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
