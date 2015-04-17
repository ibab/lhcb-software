##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from Configurables import PhotonMaker, ResolvedPi0Maker, MergedPi0Maker
from GaudiKernel.SystemOfUnits import MeV
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
Hlt2BiKalmanFittedForwardTracking = Hlt2BiKalmanFittedForwardTracking()
##########################################################################
#
# Neutral protoparticles
#
neutralProtos = Hlt2BiKalmanFittedForwardTracking.hlt2NeutralProtos()
neutralProtosFromL0 = Hlt2BiKalmanFittedForwardTracking.hlt2Pi0FromL0()
##########################################################################
# Make the eta
#
Hlt2ResolvedEtas 			= ResolvedPi0Maker("Hlt2ResolvedEtas")
Hlt2ResolvedEtas.Input      = neutralProtos.outputSelection() 
Hlt2ResolvedEtas.DecayDescriptor 	= "Eta"
Hlt2ResolvedEtas.Output 	= 'Hlt2/Hlt2ResolvedEtas/Particles'
Hlt2ResolvedEtas.addTool(PhotonMaker)
Hlt2ResolvedEtas.PhotonMaker.Input 	= neutralProtos.outputSelection() 
Hlt2ResolvedEtas.MassWindow 		= 105. * MeV #
Hlt2ResolvedEtas.PhotonMaker.PtCut 	= 200.*MeV
##########################################################################
# Make the eta
#
#Hlt2ResolvedEtasFromL0 			= ResolvedPi0Maker("Hlt2ResolvedEtasFromL0")
#Hlt2ResolvedEtasFromL0.Input      = neutralProtosFromL0.outputSelection() 
#Hlt2ResolvedEtasFromL0.DecayDescriptor 	= "Eta"
#Hlt2ResolvedEtasFromL0.Output 	= 'Hlt2/Hlt2ResolvedEtasFromL0/Particles'
#Hlt2ResolvedEtasFromL0.addTool(PhotonMaker)
#Hlt2ResolvedEtasFromL0.PhotonMaker.Input 	= neutralProtosFromL0.outputSelection() 
#Hlt2ResolvedEtasFromL0.MassWindow 		= 105.* MeV  #
#Hlt2ResolvedEtasFromL0.PhotonMaker.PtCut 	= 200.*MeV
##########################################################################
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.BasicParticles import Hlt2ResolvedEtas
#

#__all__ = ( 'ResolvedEtas', 'ResolvedEtasFromL0' )
__all__ = ( 'ResolvedEtas' )

ResolvedEtas  = bindMembers( None, [ neutralProtos, Hlt2ResolvedEtas ] )
#ResolvedEtasFromL0  = bindMembers( None, [ neutralProtosFromL0, Hlt2ResolvedEtasFromL0 ] )

