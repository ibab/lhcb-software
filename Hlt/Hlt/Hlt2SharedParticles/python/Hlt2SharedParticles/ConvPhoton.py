### @file
#
# Standard converted photons
#  @author S. Benson sean.benson@cern.ch
#  @date 2015-02-07
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons as Electrons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownElectrons as DownElectrons
from HltLine.HltLine import bindMembers, Hlt2Member
from HltTracking.HltPVs import PV3D

__all__ = ( 'ConvPhotonLL', 'ConvPhotonDD' )
 
Hlt2DiEl_LL = Hlt2Member( CombineParticles
        , "ConvPhotonLL"
        , DecayDescriptors = [ "gamma -> e+ e-" ]
        , DaughtersCuts = { "e+": "(TRCHI2DOF<4.0) & (PT>200.0*MeV)",
            "e-": "(TRCHI2DOF<4.0) & (PT>200.0*MeV)" }
        , CombinationCut = "(AM<100.0*MeV)" 
        , MotherCut = "(VFASPF(VCHI2)<25.0)"
        , Inputs = [ Electrons ]
        )
Hlt2DiEl_DD = Hlt2Member( CombineParticles
        , "ConvPhotonDD"
        , DecayDescriptors = [ "gamma -> e+ e-" ]
        , DaughtersCuts = { "e+": "(TRCHI2DOF<4.0) & (PT>200.0*MeV)",
            "e-": "(TRCHI2DOF<4.0) & (PT>200.0*MeV)"}
        , CombinationCut = "(AM<100.0*MeV)"
        , MotherCut = "(VFASPF(VCHI2)<25.0)"
        , Inputs = [ DownElectrons ]
        )

ConvPhotonLL = bindMembers( "Shared", [ PV3D('Hlt2'), Electrons, Hlt2DiEl_LL ] )
ConvPhotonDD = bindMembers( "Shared", [ PV3D('Hlt2'), DownElectrons, Hlt2DiEl_DD ] )
