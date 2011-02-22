### @file
#
#  Standard good Particles with IP and Pt.
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers, Hlt2Member
from HltTracking.HltPVs import PV3D
from Configurables import FilterDesktop
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
##########################################################################
# Prepare the Kaons -- pt cuts
#
Kaons = Hlt2Member( FilterDesktop, "Kaons", Inputs = [ BiKalmanFittedKaons ], Code = "(PT > 300.*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)" )
##########################################################################
# Prepare the Pions -- pt cuts
#
Pions = Hlt2Member( FilterDesktop, "Pions", Inputs = [ BiKalmanFittedPions ], Code = "(PT > 300.*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)" )
##########################################################################
# TODO: why does clone not work anymore???
#Pions = Kaons.clone("Pions", Inputs = [BiKalmanFittedPions])
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
#

__all__ = ( 'GoodKaons','GoodPions' )

GoodKaons = bindMembers( 'Good', [ PV3D(), BiKalmanFittedKaons, Kaons ] )
GoodPions = bindMembers( 'Good', [ PV3D(), BiKalmanFittedPions, Pions ] )

