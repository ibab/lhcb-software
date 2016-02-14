"""
Configuration of jet building in the HLT
"""

from os import environ, path
from LHCbKernel.Configuration import *
from Gaudi.Configuration import *

from Configurables import (GaudiSequencer,
                           CellularAutomatonAlg,
                           CaloClusterizationTool,
                           CaloClusterCovarianceAlg,
                           ClusterSpreadTool,
                           ClusterCovarianceMatrixTool,
                           CaloPhotonMatch,
                           InHcalAcceptanceAlg,
                           PhotonMatchAlg)

import GaudiKernel.ProcessJobOptions
from HltConf.ThresholdUtils import importLineConfigurables

import Hlt2Lines
_hlt2linesconfs = importLineConfigurables( Hlt2Lines )

class HltJetConf(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = _hlt2linesconfs
    __slots__ = {'ParticleFlowOptions' : {},
                 '__particleFlow'  : 0,
                 '__caloProcessor' : 0}

    def particleFlow(self):
        if not hasattr(self, '__particleFlow') or not self.__particleFlow:
            self.__configure()
        return self.__particleFlow

    def __configure(self):
        recoSeq = GaudiSequencer("Hlt2JetRecoSequence", ModeOR = True, ShortCircuit = False)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking

        # Long charged proto particles
        longTracking = Hlt2BiKalmanFittedForwardTracking()
        longChargedProtos = longTracking.hlt2ChargedNoPIDsProtos()
        longSeq = GaudiSequencer("Hlt2JetLongRecoSequence", Members = longChargedProtos.members())

        # Downstream charged proto particles
        downTracking = Hlt2BiKalmanFittedDownstreamTracking()
        downChargedProtos = downTracking.hlt2ChargedNoPIDsProtos()
        downSeq = GaudiSequencer("Hlt2JetDownRecoSequence", Members = downChargedProtos.members())

        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons as Photons
        from Hlt2SharedParticles.Pi0 import ResolvedPi0s
        from Hlt2SharedParticles.Pi0 import MergedPi0s
        from Hlt2SharedParticles.Ks import KsLLTF as KsLL
        from Hlt2SharedParticles.Ks import KsLLTF as KsDD
        from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted as LambdaLL
        from Hlt2SharedParticles.Lambda import LambdaDDTrackFitted as LambdaDD

        veloTracks = longTracking.hlt2VeloTracking()
        veloSeq = GaudiSequencer("Hlt2JetVeloRecoSequence", Members = veloTracks.members())

        self.__caloProcessor = longTracking.caloProcessor()
        ecalSeq = self.__caloProcessor.clusters()

        inputs = [
            ['Particle',       'particle', self.__sharedParticleInput(Photons, "Photons")],
            ['Particle',       'particle', self.__sharedParticleInput(ResolvedPi0s, "ResolvedPi0s")],
            ['Particle',       'particle', self.__sharedParticleInput(MergedPi0s, "MergedPi0s")],
            ['Particle',       'particle', self.__sharedParticleInput(KsLL, "KsLL")],
            ['Particle',       'particle', self.__sharedParticleInput(KsDD, "KsDD")],
            ['Particle',       'particle', self.__sharedParticleInput(LambdaLL, "LambdaLL")],
            ['Particle',       'particle', self.__sharedParticleInput(LambdaDD, "LambdaDD")],
            ['ProtoParticle',  'best',     (longSeq, longChargedProtos.outputSelection())],
            ['ProtoParticle',  'best',     (downSeq, downChargedProtos.outputSelection())],
            ['Track',          'pi+',      (None, veloTracks.outputSelection())],
            ['CaloCluster',    'gamma',    (None, self.__findCaloLocation(Photons.members(), CellularAutomatonAlg, "OutputData"))],
            ['CaloCluster',    'gamma',    self.__hcalClusters()],
            ['IClusTrTable2D', 'ecal',     (None, self.__findCaloLocation(Photons.members(), PhotonMatchAlg, "Output"))],
            ['IClusTrTable2D', 'hcal',     self.__hcal2Track(longTracking, Photons)]
        ]

        pfInputs = []
        for inputClass, inputType, (seq, loc) in inputs:
            if seq and seq not in recoSeq.Members:
                recoSeq.Members += [seq]
            pfInputs.append([inputClass, inputType, loc])

        from Configurables import HltParticleFlow
        particleFlow = HltParticleFlow("HltParticleFlow", **self.getProp('ParticleFlowOptions'))
        particleFlow.Inputs = pfInputs
        particleFlow.Output = "Hlt2/ParticleFlow/Particles"

        from HltLine.HltLine    import bindMembers
        self.__particleFlow = bindMembers("Hlt2Jet", [recoSeq, particleFlow])

    def __sharedParticleInput(self, sp, name):
        seq = GaudiSequencer("Hlt2Jet" + name + "Sequence", Members = sp.members())
        return seq, sp.outputSelection()

    def __hcalClusters(self):
        ## Create Sequencer
        hcalSeq = GaudiSequencer("Hlt2JetHcalClusterSequence",
                                 Members = [self.__caloProcessor.digits()])
        ## Call the cluster creation
        # name defines input and output locations.
        hcalClus = CellularAutomatonAlg('HcalClustForHlt')
        caloContext = 'Hlt'
        hcalClus.Context = caloContext
        hcalClus.OutputData = caloContext + '/Calo/HcalClusters'
        hcalSeq.Members += [hcalClus]
        ## Get the covariance matrix
        clustCov = CaloClusterCovarianceAlg('Hlt2HcalCov')
        clustCov.InputData = hcalClus.OutputData
        clustCov.Context = caloContext
        clustCov.EnergyTags = [ '2x2' ]
        clustCov.CovarianceParameters["Stochastic"] = [0.7]
        clustCov.CovarianceParameters["GainError"]  = [0.1]
        hcalSeq.Members += [clustCov]
        return hcalSeq, hcalClus.OutputData

    def __hcal2Track(self, tracking, photons):
        ## Get Association to tracks
        clusterSeq, clusterLoc = self.__hcalClusters()
        hcalSeq = GaudiSequencer("Hlt2JetHcal2TrackSequence", Members = [clusterSeq])
        hcal2Track = PhotonMatchAlg("Hcal2TrackMatching")
        caloContext = self.__caloProcessor.getProp('Context')
        hcal2Track.Context = caloContext
        hcal2Track.Tracks = tracking.allTrackLocations()
        hcal2Track.Calos  = [clusterLoc]
        hcal2Track.Output = caloContext + '/Calo/HcalClusterMatch'
        filterLoc = self.__findCaloLocation(photons.members(), InHcalAcceptanceAlg, "Output")
        hcal2Track.Filter = filterLoc
        hcal2Track.addTool(CaloPhotonMatch, "HcalMatch" )
        hcal2Track.Tool   = "CaloPhotonMatch/HcalMatch"
        hcal2Track.Threshold = "1000"
        hcal2Track.HcalMatch.Calorimeter =  "/dd/Structure/LHCb/DownstreamRegion/Hcal"
        hcal2Track.HcalMatch.Tolerance= "60"
        hcal2Track.HcalMatch.Extrapolator= "TrackRungeKuttaExtrapolator/Regular"
        hcalSeq.Members += [hcal2Track]
        return hcalSeq, hcal2Track.Output

    def __findCaloLocation(self, algs, caloAlgType, prop, inName = '', forceContext = ''):
        # Dirty way to find calorimeter locations. They even need to be updated
        # to silence the input/output matchmaker...
        for alg in algs:
            if hasattr(alg, "Members"):
                loc = self.__findCaloLocation(alg.Members, caloAlgType, prop)
                if loc: return loc
            elif type(alg) == caloAlgType and (not inName or inName in alg.name()):
                val = alg.getProp(prop)
                ctx = alg.getProp('Context') if not forceContext else forceContext
                if ctx and ctx != 'Offline' and ctx not in val:
                    val = val.replace('Rec', ctx)
                    if 'Hcal' in alg.name():
                        val = val.replace('Ecal', 'Hcal')
                    alg.setProp(prop, val)
                return val

    def __apply_configuration__(self):
        self.__configure()
