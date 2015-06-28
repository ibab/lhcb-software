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
# Make the pi0
#
Hlt2ResolvedPi0s 			= ResolvedPi0Maker("Hlt2ResolvedPi0s")
Hlt2ResolvedPi0s.Input      = neutralProtos.outputSelection()
Hlt2ResolvedPi0s.DecayDescriptor 	= "Pi0"
Hlt2ResolvedPi0s.Output 	= 'Hlt2/Hlt2ResolvedPi0s/Particles'
Hlt2ResolvedPi0s.addTool(PhotonMaker)
Hlt2ResolvedPi0s.PhotonMaker.Input 	= neutralProtos.outputSelection()
Hlt2ResolvedPi0s.MassWindow 		= 60.* MeV # was 30.* MeV
Hlt2ResolvedPi0s.PhotonMaker.PtCut 	= 200.*MeV
##########################################################################
# Make the pi0
#
Hlt2MergedPi0s 			= MergedPi0Maker("Hlt2MergedPi0s")
Hlt2MergedPi0s.Output        = 'Hlt2/Hlt2MergedPi0s/Particles'
Hlt2MergedPi0s.DecayDescriptor 	= "Pi0"
Hlt2MergedPi0s.Input		= neutralProtos.outputSelection()
Hlt2MergedPi0s.MassWindow 	= 60.* MeV # this is as in  offline. too tight? 
##########################################################################
# Make both
#
SeqHlt2Pi0 = GaudiSequencer('SeqHlt2Pi0',  ModeOR=True, ShortCircuit=False,
                            Members = [ Hlt2MergedPi0s, Hlt2ResolvedPi0s])
##########################################################################
# Make the pi0
#
Hlt2ResolvedPi0sFromL0 			= ResolvedPi0Maker("Hlt2ResolvedPi0sFromL0")
Hlt2ResolvedPi0sFromL0.Input      = neutralProtosFromL0.outputSelection()
Hlt2ResolvedPi0sFromL0.DecayDescriptor 	= "Pi0"
Hlt2ResolvedPi0sFromL0.Output 	= 'Hlt2/Hlt2ResolvedPi0sFromL0/Particles'
Hlt2ResolvedPi0sFromL0.addTool(PhotonMaker)
Hlt2ResolvedPi0sFromL0.PhotonMaker.Input 	= neutralProtosFromL0.outputSelection()
Hlt2ResolvedPi0sFromL0.MassWindow 		= 60.* MeV # was 30.* MeV
Hlt2ResolvedPi0sFromL0.PhotonMaker.PtCut 	= 200.*MeV
##########################################################################
# Make the pi0
#
Hlt2MergedPi0sFromL0 			= MergedPi0Maker("Hlt2MergedPi0sFromL0")
Hlt2MergedPi0sFromL0.Output        = 'Hlt2/Hlt2MergedPi0sFromL0/Particles'
Hlt2MergedPi0sFromL0.DecayDescriptor 	= "Pi0"
Hlt2MergedPi0sFromL0.Input		= neutralProtosFromL0.outputSelection()
Hlt2MergedPi0sFromL0.MassWindow 	= 60.* MeV 
##########################################################################
# Make both
#
SeqHlt2Pi0FromL0 = GaudiSequencer('SeqHlt2Pi0FromL0 ',  ModeOR=True, ShortCircuit=False,
                            Members = [ Hlt2MergedPi0sFromL0, Hlt2ResolvedPi0sFromL0])
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.BasicParticles import Hlt2MergedPi0s
#

__all__ = ( 'MergedPi0s', 'ResolvedPi0s', 'AllPi0s', 'MergedPi0sFromL0', 'ResolvedPi0sFromL0', 'AllPi0sFromL0' )

MergedPi0s    = bindMembers( None, [ neutralProtos, Hlt2MergedPi0s ] ).setOutputSelection(Hlt2MergedPi0s.Output)
ResolvedPi0s  = bindMembers( None, [ neutralProtos, Hlt2ResolvedPi0s ] ).setOutputSelection(Hlt2ResolvedPi0s.Output)
AllPi0s       = bindMembers( None, [ neutralProtos, SeqHlt2Pi0 ] ).setOutputSelection( [ Hlt2MergedPi0s.Output, Hlt2ResolvedPi0s.Output ] )
MergedPi0sFromL0    = bindMembers( None, [ neutralProtosFromL0, Hlt2MergedPi0sFromL0 ] ).setOutputSelection(Hlt2MergedPi0sFromL0.Output)
ResolvedPi0sFromL0  = bindMembers( None, [ neutralProtosFromL0, Hlt2ResolvedPi0sFromL0 ] ).setOutputSelection(Hlt2ResolvedPi0sFromL0.Output)
AllPi0sFromL0       = bindMembers( None, [ SeqHlt2Pi0FromL0 ] ).setOutputSelection([Hlt2MergedPi0sFromL0.Output, Hlt2ResolvedPi0sFromL0.Output])
