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

__all__ = ( 'ConvPhotonLL', 'ConvPhotonDD' )
 
BA = Tool(type = BremAdder, name = 'BremAdder', BremInput = "Hlt2/Hlt2BiKalmanFittedPhotons/Particles")
Hlt2DiEl_LL = Hlt2Member(DiElectronMaker
        ,'ConvPhotonLL'
        , DecayDescriptor = "gamma -> e+ e-"
        , ElectronInputs = [Electrons.outputSelection()]
        , DeltaY = 3.
        , DeltaYmax = 200. * mm
        , DiElectronMassMax = 80.*MeV
        , DiElectronPtMin = 200.*MeV
        , tools = [BA]
        )
Hlt2DiEl_DD = Hlt2Member(DiElectronMaker
        ,'ConvPhotonDD'
        , DecayDescriptor = "gamma -> e+ e-"
        , ElectronInputs = [DownElectrons.outputSelection()]
        , DeltaY = 3.
        , DeltaYmax = 200. * mm
        , DiElectronMassMax = 100.*MeV
        , DiElectronPtMin = 200.*MeV
        , tools = [BA]
        )

ConvPhotonLL = bindMembers( "ConvPhotonLL", [ PV3D('Hlt2'), Photons, Electrons, Hlt2DiEl_LL ] )
ConvPhotonDD = bindMembers( "ConvPhotonDD", [ PV3D('Hlt2'), Photons, DownElectrons, Hlt2DiEl_DD ] )
