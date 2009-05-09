### @file
#
#  Standard Pi0
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltConf.HltLine import bindMembers
from Configurables import PhotonMaker, ResolvedPi0Maker, MergedPi0Maker
from GaudiKernel.SystemOfUnits import MeV
##########################################################################
# Make the pi0
#
Hlt2ResolvedPi0s = ResolvedPi0Maker("Hlt2ResolvedPi0s")
Hlt2ResolvedPi0s.DecayDescriptor = "Pi0"
Hlt2ResolvedPi0s.addTool(PhotonMaker)
Hlt2ResolvedPi0s.PhotonMaker.Input = "Hlt/ProtoP/Neutrals"
Hlt2ResolvedPi0s.MassWindow = 30.* MeV
Hlt2ResolvedPi0s.PhotonMaker.PtCut = 200.*MeV
##########################################################################
# Make the pi0
#
Hlt2MergedPi0s = MergedPi0Maker("Hlt2MergedPi0s")
Hlt2MergedPi0s.DecayDescriptor = "Pi0" 
Hlt2MergedPi0s.Input= "Hlt/ProtoP/Neutrals"
Hlt2MergedPi0s.MassWindow = 60.* MeV
##########################################################################
# Make both
#
SeqHlt2Pi0 = GaudiSequencer('SeqHlt2Pi0 ',  ModeOR=True, ShortCircuit=False,
                            Members = [ Hlt2MergedPi0s, Hlt2ResolvedPi0s])
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.BasicParticles import Hlt2Muons
#

__all__ = ( 'MergedPi0s', 'ResolvedPi0s', 'AllPi0s' )

MergedPi0s    = bindMembers( None, [ Hlt2MergedPi0s ] )
ResolvedPi0s  = bindMembers( None, [ Hlt2ResolvedPi0s ] )
AllPi0s       = bindMembers( None, [ SeqHlt2Pi0 ] ).setOutputSelection( [ Hlt2MergedPi0s.name(), Hlt2ResolvedPi0s.name() ] )
