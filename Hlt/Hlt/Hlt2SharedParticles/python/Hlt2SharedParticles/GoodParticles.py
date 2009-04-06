### @file
#
#  Standard good Particles with IP and Pt.
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import FilterDesktop, PhysDesktop
##########################################################################
# Prepare the Kaons -- pt cuts
#
Hlt2GoodKaons = FilterDesktop("Hlt2GoodKaons")
Hlt2GoodKaons.addTool(PhysDesktop)
Hlt2GoodKaons.PhysDesktop.InputLocations = [ "Hlt2Kaons" ]
Hlt2GoodKaons.Code = "(PT > 300.*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4.)"
##########################################################################
# Prepare the Pions -- pt cuts
#
Hlt2GoodPions = Hlt2GoodKaons.clone("Hlt2GoodPions")
Hlt2GoodPions.PhysDesktop.InputLocations = [ "Hlt2Pions" ]


#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
#

__all__ = { 'GoodKaons','GoodPions' }

GoodKaons = Hlt2GoodKaons
GoodPions = Hlt2GoodPions


