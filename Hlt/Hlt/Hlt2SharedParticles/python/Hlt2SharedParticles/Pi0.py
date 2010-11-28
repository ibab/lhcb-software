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
##########################################################################
# Make the pi0
#
Hlt2ResolvedPi0s 			= ResolvedPi0Maker("Hlt2ResolvedPi0s")
Hlt2ResolvedPi0s.DecayDescriptor 	= "Pi0"
Hlt2ResolvedPi0s.addTool(PhotonMaker)
Hlt2ResolvedPi0s.PhotonMaker.Input 	= neutralProtos.outputSelection() 
Hlt2ResolvedPi0s.MassWindow 		= 30.* MeV
Hlt2ResolvedPi0s.PhotonMaker.PtCut 	= 200.*MeV
##########################################################################
# Make the pi0
#
Hlt2MergedPi0s 			= MergedPi0Maker("Hlt2MergedPi0s")
Hlt2MergedPi0s.DecayDescriptor 	= "Pi0" 
Hlt2MergedPi0s.Input		= neutralProtos.outputSelection()
Hlt2MergedPi0s.MassWindow 	= 60.* MeV
##########################################################################
# Make both
#
SeqHlt2Pi0 = GaudiSequencer('SeqHlt2Pi0 ',  ModeOR=True, ShortCircuit=False,
                            Members = [ Hlt2MergedPi0s, Hlt2ResolvedPi0s])
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.BasicParticles import Hlt2MergedPi0s
#

__all__ = ( 'MergedPi0s', 'ResolvedPi0s', 'AllPi0s' )

MergedPi0s    = bindMembers( None, [ neutralProtos, Hlt2MergedPi0s ] )
ResolvedPi0s  = bindMembers( None, [ neutralProtos, Hlt2ResolvedPi0s ] )
AllPi0s       = bindMembers( None, [ SeqHlt2Pi0 ] ).setOutputSelection( [ Hlt2MergedPi0s.name(), Hlt2ResolvedPi0s.name() ] )
