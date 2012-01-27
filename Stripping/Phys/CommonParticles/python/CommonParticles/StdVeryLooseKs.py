#!/usr/bin/env python
# =============================================================================
# $Id: StdVeryLooseKs.py,v 1.5 2009-05-26 13:48:13 shaines Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseVeryKs.py
#  configuration file for 'Standard Very Loose Ks' 
#  @author Susan Haines
#  @date 2011-08-27
# =============================================================================
"""
Configuration file for 'Standard Very Loose Ks' using StdAllNoPIDsPions, i.e. no pT cut on LL pi daughters.  Other cuts tighter vs. StdLooseKs.
"""
__author__  = "Susan Haines"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdVeryLooseKsLL' ,
    'StdVeryLooseKsDD' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdVeryLooseKsLL = CombineParticles ( 'StdVeryLooseKsLL' )

StdVeryLooseKsLL.Inputs = [ "Phys/StdAllNoPIDsPions/Particles" ]
StdVeryLooseKsLL.DecayDescriptor = "KS0 -> pi+ pi-" 

StdVeryLooseKsLL.DaughtersCuts = { "pi+" : "(P > 2.*GeV) & (P < 100.*GeV) & (BPVIPCHI2() > 9.)" } 
StdVeryLooseKsLL.CombinationCut = "(ADAMASS('KS0') < 35.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<1.5)"
StdVeryLooseKsLL.MotherCut = "(ADMASS('KS0') < 30.*MeV) & (VFASPF(VCHI2/VDOF) < 16.) & (BPVVDCHI2 > 4.) "

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseKsLL )

## ============================================================================
#  configuration file for 'Standard Very Loose Downstream ' 
#  @author Susan Haines
#  @date 2011-08-27
# ============================================================================= 
StdVeryLooseKsDD = CombineParticles ( 'StdVeryLooseKsDD' )

StdVeryLooseKsDD.Inputs = [ "Phys/StdNoPIDsDownPions/Particles" ]
StdVeryLooseKsDD.DecayDescriptor = "KS0 -> pi+ pi-" 

StdVeryLooseKsDD.DaughtersCuts = { "pi+" : "(P > 2.*GeV) & (P < 100.*GeV) & (BPVIPCHI2() > 6.25)  & ((-PIDK) > 0.)" } 
StdVeryLooseKsDD.CombinationCut = "(ADAMASS('KS0') < 50.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<22.)"
StdVeryLooseKsDD.MotherCut = "(ADMASS('KS0') < 42.*MeV) & (VFASPF(VCHI2/VDOF) < 16.) & (BPVVDCHI2 > 4.)"

## configure Data-On-Demand service 
locations.update( updateDoD ( StdVeryLooseKsDD ))

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
