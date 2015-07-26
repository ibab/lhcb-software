### @file
#
# Standard converted photons
#  @author S. Benson sean.benson@cern.ch
#  @date 2015-02-07
#
##
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
from Configurables import CombineParticles, DiElectronMaker, BremAdder
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons as Electrons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectronsFromL0 as L0Electrons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons as Photons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownElectrons as DownElectrons
from HltLine.HltLine import bindMembers, Hlt2Member
from HltTracking.HltPVs import PV3D
from HltLine.HltLine import Hlt1Tool as Tool

## Tracking configurations to silence useless warnings
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
LongTracking = Hlt2BiKalmanFittedForwardTracking()
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
DownstreamTracking = Hlt2BiKalmanFittedDownstreamTracking()


__all__ = ( 'ConvPhotonLL', 'ConvPhotonDD' )

BA = Tool(type = BremAdder, name = 'BremAdder', BremInput = Photons.outputSelection())
Hlt2DiEl_LL = Hlt2Member(DiElectronMaker
        ,'LL'
        , DecayDescriptor = "gamma -> e+ e-"
        ## Inputs are not actually used, but this silences the silly warnings
        , Inputs = [LongTracking.hlt2ChargedAllPIDsProtos()]
        , ElectronInputs = [Electrons.outputSelection()]
        , DeltaY = 3.
        , DeltaYmax = 200. * mm
        , DiElectronMassMax = 80.*MeV
        , DiElectronPtMin = 200.*MeV
        , tools = [BA]
        )
Hlt2DiEl_DD = Hlt2Member(DiElectronMaker
        ,'DD'
        , DecayDescriptor = "gamma -> e+ e-"
        ## Inputs are not actually used, but this silences the silly warnings
        , Inputs = [DownstreamTracking.hlt2ChargedCaloProtos()]
        , ElectronInputs = [DownElectrons.outputSelection()]
        , DeltaY = 3.
        , DeltaYmax = 200. * mm
        , DiElectronMassMax = 100.*MeV
        , DiElectronPtMin = 200.*MeV
        , tools = [BA]
        )

ConvPhotonLL = bindMembers( "ConvPhoton", [ PV3D('Hlt2'), Photons, Electrons, Hlt2DiEl_LL ] )
ConvPhotonDD = bindMembers( "ConvPhoton", [ PV3D('Hlt2'), Photons, DownElectrons, Hlt2DiEl_DD ] )
