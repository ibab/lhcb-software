# =============================================================================
# @file:   Definition of the electrons used by the DiElectron lines
# @author: Miguel Ramos Pernas miguel.ramos.pernas@cern.ch
# @author: Jessica Prisciandaro jessica.prisciandaro@cern.ch
# @date:   2016-02-12
# =============================================================================

__all__    = ( 'SoftElectrons', 'TrackFittedDetachedDiElectron' )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, BremAdder, DiElectronMaker
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons, BiKalmanFittedPhotons
from HltLine.HltLine import bindMembers, Hlt1Tool, Hlt2Member
from HltTracking.HltPVs import PV3D

Electrons = Hlt2Member(FilterDesktop, "Electrons", Inputs = [BiKalmanFittedElectrons], Code = "(MIPDV(PRIMARY) > 0.4 * mm) & (MIPCHI2DV(PRIMARY) > 5) & (TRGHOSTPROB < 0.15) & (PT > 80 * MeV)")

SoftElectrons = bindMembers('Soft', [PV3D('Hlt2'), BiKalmanFittedElectrons, Electrons])

BA = Hlt1Tool(type = BremAdder, name = 'BremAdder', BremInput = "Hlt2/Hlt2BiKalmanFittedPhotons/Particles")
Hlt2SharedTrackFittedDetachedDiElectron = Hlt2Member( DiElectronMaker
                                                      , "TrackFittedDetachedDiElectron"
                                                      , ElectronInputs = [ SoftElectrons.outputSelection() ]
                                                      , Particle = 'KS0'
                                                      , DecayDescriptor = "KS0 -> e+ e-"
                                                      , ElectronPtMin=80
                                                      , DiElectronPtMin=200
                                                      , DiElectronMassMax=1000
                                                      , tools = [BA]
                                                      )

TrackFittedDetachedDiElectron = bindMembers("Shared", [PV3D('Hlt2'), SoftElectrons, Hlt2SharedTrackFittedDetachedDiElectron])
