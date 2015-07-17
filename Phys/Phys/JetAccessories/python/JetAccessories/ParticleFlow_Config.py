__version__ = "$Id: ParticleFlow_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, TrackSelector, DelegatingTrackSelector, ParticleFlow, CellularAutomatonAlg, CaloClusterizationTool,CaloClusterCovarianceAlg,ClusterSpreadTool,ClusterCovarianceMatrixTool, CaloPhotonMatch , PhotonMatchAlg , PVRelatorAlg, ChargedProtoParticleMaker, FilterDesktop)


class ParticleFlowConf:
    def __init__(self, _name, _InputParticles = ['Photons','NeutralHadrons','Charged','Pi0s','V0s','PFNeutrals'], _MCCor = False, _MCSeq = False , _params = {} ):
        # _MCCor is kept in config to not break anything but not used, it is
        # fixed now in a postconfiguration based on DaVinci Simulation status
        # or not.
         self.name = _name
         self.InputParticles = _InputParticles
         self.MC = _MCSeq
         ## Default parameters:
         self.paramDef = {  ### Location
             "PFOutputLocation"   : "Phys/PFParticles/Particles" ,
             "PFProtoParticlesOutputLocation": "Rec/ProtoP/PF" ,
             "PFCaloHypoOutputLocation": "Rec/Calo/Hadrons",
             "ParticleLocations": [],
             "CompositeParticleLocations": [],
             "CandidateToBanLocation": [],
             "CandidatesToKeepLocation": [],
             "VerticesLocation": "Rec/Vertex/Primary" ,
             ### Tracks selection
             # For track selector
             "LongProbNNghostCut": 0.5 ,
             "DownProbNNghostCut": 0.5 ,
             "UpProbNNghostCut": 0.5 ,
             'TrackSelector':{'Downstream':      {'Chi2Max': 1.5 , 'PtMin': 0. , 'MaxGhostProb': 10.},
                              'Long':{'Chi2Max': 5.  , 'PtMin': 0. , 'MaxGhostProb': 10.},
                              'Upstream':  {'Chi2Max': 1.5 , 'PtMin': 100.}},
             'TrackVelo':{'Chi2Max': 10.},
             #"UseTTHits" : False ,
             "MinInfMomentumCut": 10. ,
             "MinInfMomentumCutDown": 10. ,
             "MinInfMomentumCutUp": 2. ,
             ##"MaxChi2NoTT": 5. ,
             "UseVelo": True ,
             ## Neutral selection
             "MinPhotonID4Photon": -1. ,
             "MinPhotonID4PhotonTtrack": -2. ,
             "MinPhotonIDMax4ResolvedPi0" : -4. ,
             "MinPhotonIDMin4ResolvedPi0": -2.,
             "MinIsoPhotonEt":200.,
             "MinPhotonEt":200.,
             "MinBadPhotonEt":2000.,
             "MinBadPhotonMatchingTEt":2000.,
             "UseHCAL" : True,
             "MinHCALE":0.,
             "MinHCALEt":500.,
             "MinHCALEt4Eta4":1000.,
             "UseTTrackBanning": True,
             ### Neutral recovery
             "MaxMatchECALTr" : 25. ,
             "MaxMatchECALTr_T": 16. ,
             "MaxMatchHCALLowEValue": 5000.,
             "MaxMatchHCALHighEValue":10000.,
             "MaxMatchHCALTrSmallE":25. ,
             "MaxMatchHCALTrMediumE": 16. ,
             "MaxMatchHCALTrLargeE" : 16. ,
             "NeutralRecovery": True ,
             "MinE": 0.,
             "MinEt": 500.,
             "MC_recovery": True,
             "MaximumFracNeutrReco":1.8,
             "BanInfMomentumFromNR": False,
             "OnlyBestCaloMatchForNR": True,
             "scalingANDsmearing" : False
             }
         # set the datafile
         ### Get all the name coherantly
         self.MCCor = _MCCor
         self.PFSeq = GaudiSequencer(_name+"Particles",IgnoreFilterPassed = True)
         self.algorithms = []
         self.setupParam(_params)
         self.setupPF()


    def setupTypeTrackSelector(self,tsname,selector,trackCuts):
        selector.addTool(TrackSelector,name=tsname)
        ts = getattr(selector,tsname)
        # Set Cuts
        ts.TrackTypes = [tsname]
        if tsname in trackCuts :
            for name,cut in trackCuts[tsname].iteritems() :
                if name.find("Min")>-0.5 or  name.find("Max")>-0.5 or name.find("AcceptClones")>-0.5:
                    ts.setProp(name,cut)
                else:
                    ts.setProp("Min"+name,cut[0])
                    ts.setProp("Max"+name,cut[1])







    def setupParam(self,params):
        self.paramDef.update(params)


    ## Configure the jet maker
    def setupPF(self ):


        ## set the algorithm
        alg = ParticleFlow ( self.name )

        if "Velo" in self.paramDef['TrackSelector'].keys() and not self.paramDef['UseVelo']: self.paramDef['UseVelo'] = True

        # set all params
        for prop, value in self.paramDef.items():
            if prop != 'TrackSelector' and  prop != 'TrackVelo' and prop!= "scalingANDsmearing":
                setattr(alg, prop, value)
            else:
                self.__dict__[prop] = value

        if (self.name != "PF"):
            alg.PFOutputLocation = "Phys/"+self.name+"/Particles"



        ## List of algorithms to put in the sequencer
        if self.scalingANDsmearing:
            from Configurables import TrackScaleState as SCALER
            scaler= SCALER('PFStateScale')
            self.PFSeq.Members+= [scaler]
            if self.MCCor:
                from Configurables import TrackSmearState as SMEAR
                smear= SMEAR('PFStateSmear')
                self.PFSeq.Members+= [  smear ]




        ## Definition of cuts to apply to input tracks for inputselection
        TrackCuts = {}
        if self.paramDef['UseVelo'] and not "Velo" in self.TrackSelector.keys():
            self.TrackSelector["Velo"] = self.TrackVelo

        for trtype in self.TrackSelector.keys():
            if trtype == "Long":
                TrackCuts[trtype]={   "Chi2Cut" : [0,self.TrackSelector[trtype]['Chi2Max']]  ,
                                      "MinPtCut": self.TrackSelector[trtype]['PtMin'],
                                      "MaxGhostProbCut": self.TrackSelector[trtype]['MaxGhostProb']}
            if trtype == "Downstream":
                TrackCuts[trtype]={  "Chi2Cut" : [0,self.TrackSelector[trtype]['Chi2Max']]   ,
                                     "MinPtCut": self.TrackSelector[trtype]['PtMin'] ,
                                     "MaxGhostProbCut": self.TrackSelector[trtype]['MaxGhostProb']}
            if trtype == "Upstream":
                TrackCuts[trtype]={  "Chi2Cut" : [0,self.TrackSelector[trtype]['Chi2Max']],
                                     "MinPtCut": self.TrackSelector[trtype]['PtMin'] }
            if trtype == "Velo":
                TrackCuts[trtype]={  "Chi2Cut" : [0,self.TrackSelector[trtype]['Chi2Max']]    }

                protos = ChargedProtoParticleMaker("VeloProtoPMaker")
                protos.Inputs = ["Rec/Track/Best"]
                protos.Output = "Rec/ProtoP/VeloProtoPMaker"
                protos.addTool( DelegatingTrackSelector, name="TrackSelector" )
                protos.TrackSelector.TrackTypes = ["Velo"]
                self.setupTypeTrackSelector( "Velo" , protos.TrackSelector ,TrackCuts["Velo"] )
                self.PFSeq.Members+= [ protos ]



        pLocations = []
        pCompLocations = []
        alg.MC_recovery = self.MCCor



        #re set default to false
        alg.UseHCAL = False
        alg.NeutralRecovery = False

        for t in self.InputParticles:
            if t=='Photons':
                pLocations.append('Phys/StdLooseAllPhotons/Particles')

            elif t=='NeutralHadrons':
                ## Set the HCAL uses
                alg.UseHCAL = True
                self.setupHCAL()

            elif t=='Charged':
               ## Track selector
                alg.TrackSelectorType = "DelegatingTrackSelector"
                alg.addTool( DelegatingTrackSelector, name="TrackSelector" )
                tracktypes = TrackCuts.keys()
                alg.TrackSelector.TrackTypes = self.TrackSelector.keys()
                for type in tracktypes : self.setupTypeTrackSelector( type, alg.TrackSelector,TrackCuts )


            elif t=='Pi0s':
                pLocations.append('Phys/StdLooseResolvedPi0/Particles')
                pLocations.append('Phys/StdLooseMergedPi0/Particles')

            elif t=='V0s':
                pCompLocations.append("Phys/StdKs2PiPiLL/Particles" )
                pCompLocations.append("Phys/StdKs2PiPiDD/Particles" )
                pCompLocations.append("Phys/StdLambda2PPiLL/Particles" )
                pCompLocations.append("Phys/StdLambda2PPiDD/Particles" )

            elif t=='PFNeutrals':
                alg.NeutralRecovery = True

            else :
                print t,"are not supported!"
                exit(1)

            if len( self.paramDef['CompositeParticleLocations']) > 0.5:
                for t in self.paramDef['CompositeParticleLocations']:
                    pCompLocations.append(t)

        alg.ParticleLocations = pLocations
        alg.CompositeParticleLocations = pCompLocations
        self.PFSeq.Members += [alg]
        self.algorithms.append(self.PFSeq)


    def setupHCAL(self):
       ## Create Sequencer
        ## Call the cluster creation
        hcalClus = CellularAutomatonAlg('HcalClusterization')  # name is enough to setup I/O
        self.PFSeq.Members += [hcalClus]
        ## Get the covariance matrix
        clustCov = CaloClusterCovarianceAlg('HcalCov')
        clustCov.EnergyTags = [ '2x2' ]
        clustCov.CovarianceParameters["Stochastic"] = [0.7]
        clustCov.CovarianceParameters["GainError"]  = [0.1]
        self.PFSeq.Members += [clustCov]

        ## Get Association to tracks
        hcal2Track = PhotonMatchAlg("Hcal2TrackMatching")
        hcal2Track.Calos  = ["Rec/Calo/HcalClusters" ]
        hcal2Track.Output = "Rec/Calo/HcalClusterMatch"
        hcal2Track.Filter = "Rec/Calo/InAccHcal"
        hcal2Track.addTool(CaloPhotonMatch, "HcalMatch" )
        hcal2Track.Tool   = "CaloPhotonMatch/HcalMatch"
        hcal2Track.Threshold = "1000"
        hcal2Track.HcalMatch.Calorimeter =  "/dd/Structure/LHCb/DownstreamRegion/Hcal"
        hcal2Track.HcalMatch.Tolerance= "60"
        hcal2Track.HcalMatch.Extrapolator= "TrackRungeKuttaExtrapolator/Regular"

        self.PFSeq.Members += [hcal2Track]

    def _fixMCDATAConfig():
        #from Configurables import DaVinci
        from Configurables import PhysConf
        #if (DaVinci().getProp("Simulation")):
        if (PhysConf().getProp("Simulation")):
            from Gaudi.Configuration import allConfigurables
            for c in allConfigurables.values():
                if hasattr(c,"MC_recovery"):
                    c.MC_recovery = True

    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction( _fixMCDATAConfig )


# ----------------------------------------------------------------------------------

