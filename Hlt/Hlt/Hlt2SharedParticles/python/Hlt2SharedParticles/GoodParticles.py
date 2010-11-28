### @file
#
#  Standard good Particles with IP and Pt.
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from HltTracking.HltPVs import PV3D
from Configurables import FilterDesktop
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
##########################################################################
# Prepare the Kaons -- pt cuts
#
Hlt2GoodKaons = FilterDesktop("Hlt2GoodKaons")
Hlt2GoodKaons.InputLocations = [ BiKalmanFittedKaons.outputSelection() ]
Hlt2GoodKaons.Code = "(PT > 300.*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)"
##########################################################################
# Prepare the Pions -- pt cuts
#
Hlt2GoodPions = Hlt2GoodKaons.clone("Hlt2GoodPions")
Hlt2GoodPions.InputLocations = [ BiKalmanFittedPions.outputSelection()]
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
#

__all__ = ( 'GoodKaons','GoodPions' )

GoodKaons = bindMembers( None, [ PV3D(), BiKalmanFittedKaons, Hlt2GoodKaons ] )
GoodPions = bindMembers( None, [ PV3D(), BiKalmanFittedPions, Hlt2GoodPions ] )

