__version__ = "$Id: ParticleFlow_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, TrackSelector, DelegatingTrackSelector, ParticleFlow, CellularAutomatonAlg, CaloClusterizationTool,CaloClusterCovarianceAlg,ClusterSpreadTool,ClusterCovarianceMatrixTool, CaloPhotonMatch , PhotonMatchAlg , CaloClusterMCTruth,CaloDigit2MCLinks2Table,NeutralPP2MC , PVRelatorAlg, ChargedProtoParticleMaker)


class ParticleFlowConf:
    def __init__(self, _name, _InputParticles = ['Photons','NeutralHadrons','Charged','Pi0s','V0s','PFNeutrals'], _MCCor = False, _MCSeq = False , _params = {} ):
         self.name = _name
         self.InputParticles = _InputParticles
         self.MC = _MCSeq
         ## Default parameters:
         self.paramDef = {  ### Location
             "PFOutputLocation"   : "Phys/PFParticles/Particles" ,
             "PFProtoParticlesOutputLocation": "Rec/ProtoP/PF" ,
             "PFCaloHypoOutputLocation": "Rec/Calo/Hadrons",
             "ParticleLocations": "Phys/PFParticles/Particles",
             "CompositeParticleLocations": [],
             "CandidateToBanLocation": [],
             "VerticesLocation": "Rec/Vertex/Primary" ,
             ### Tracks selection
             # For track selector
             'TrackType':['Downstream','Long','Upstream'],
             'Chi2MaxLong': 5. , 'PtMinLong': 0. , 'AcceptClone': False ,
             'Chi2MaxDown': 10. ,'PtMinDown': 0. ,
             'Chi2MaxUp': 10. , 'PtMinUp': 0. ,
             'Chi2MaxVelo': 10. ,
             "UseTTHits" : False ,
             "MinInfMomentumCut": 10. ,
             "MinInfMomentumCutDown": 0. ,
             "MinInfMomentumCutUp": 10. ,
             "MaxChi2NoTT": 5. ,
             "UseVelo": True  ,
             ## Neutral selection
             "MinPhotonID4Photon": -1. ,
             "MinPhotonID4PhotonTtrack": -2. ,
             "MinPhotonIDMax4ResolvedPi0" : -4. ,
             "MinPhotonIDMin4ResolvedPi0": -2.,
             "UseHCAL" : True,
             "MinHCALE":2000.,
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
             "MinE": 1000.,
             "MC_recovery": True,
             "MaximumFracNeutrReco":1.5,
             "BanInfMomentumFromNR": False,
             "OnlyBestCaloMatchForNR": True
             }
         # set the datafile
         self.MCCor = _MCCor
         self.algorithms = []
         self.setupParam(_params)
         self.setupPF()
         if self.MC and 'NeutralHadrons' in self.InputParticles:
             self.setupHCALMC()

    
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
        for key in self.paramDef.keys():
            
            if params.has_key(key):
                self.__dict__[key] = params[key]
            else:
                self.__dict__[key] = self.paramDef[key]
        if "Velo" in self.TrackType and not self.UseVelo: self.UseVelo=True


    
    ## Configure the jet maker
    def setupPF(self ):
        ## List of algorithms to put in the sequencer
        
        ## set the algorithm
        alg = ParticleFlow ( self.name )

        ## Definition of cuts to apply to input tracks for inputselection
        TrackCuts = {}
        if self.UseVelo and not "Velo" in self.TrackType:
            self.TrackType.append("Velo")
        for trtype in self.TrackType:
            if trtype == "Long":
                TrackCuts["Long"]={   "Chi2Cut" : [0,self.Chi2MaxLong]  , "MinPtCut": self.PtMinLong, "CloneDistCut" : [5000, 9e+99 ],"AcceptClones" : self.AcceptClone }
            if trtype == "Downstream":
                TrackCuts["Downstream"]={  "Chi2Cut" : [0,self.Chi2MaxDown]   , "MinPtCut": self.PtMinDown }
            if trtype == "Upstream":
                TrackCuts["Upstream"]={  "Chi2Cut" : [0,self.Chi2MaxUp]     , "MinPtCut": self.PtMinUp }
            if trtype == "Velo":
                TrackCuts["Velo"]={  "Chi2Cut" : [0,self.Chi2MaxVelo]    }
                protos = ChargedProtoParticleMaker("VeloProtoPMaker")
                protos.Inputs = ["Rec/Track/Best"]
                protos.Output = "Rec/ProtoP/VeloProtoPMaker"
                protos.addTool( DelegatingTrackSelector, name="TrackSelector" )
                protos.TrackSelector.TrackTypes = ["Velo"]
                self.setupTypeTrackSelector( "Velo" , protos.TrackSelector ,TrackCuts["Velo"] )
                self.algorithms.append(  protos )   
                
        
        ## Neutral related cuts
        pLocations = []
        pCompLocations = []
        alg.MC_recovery = self.MCCor

        alg.UseTTrackBanning       = self.UseTTrackBanning
        alg.CandidateToBanLocation = self.CandidateToBanLocation
        alg.VerticesLocation       = self.VerticesLocation
        alg.PFOutputLocation       = self.PFOutputLocation
        alg.PFProtoParticlesOutputLocation = self.PFProtoParticlesOutputLocation
        alg.PFCaloHypoOutputLocation = self.PFCaloHypoOutputLocation
        
        for t in self.InputParticles:
            if t=='Photons':
                pLocations.append('Phys/StdLooseAllPhotons/Particles')
                alg.MinPhotonID4Photon = self.MinPhotonID4Photon
                alg.MinPhotonID4PhotonTtrack = self.MinPhotonID4PhotonTtrack
        
            elif t=='NeutralHadrons':
                ## Set the HCAL uses
                alg.UseHCAL = self.UseHCAL
                self.setupHCAL()
                alg.MinHCALE = self.MinHCALE
                ## Match track and clusters
                alg.MaxMatchHCALLowEValue = self.MaxMatchHCALLowEValue 
                alg.MaxMatchHCALHighEValue = self.MaxMatchHCALHighEValue 
                alg.MaxMatchHCALTrSmallE = self.MaxMatchHCALTrSmallE 
                alg.MaxMatchHCALTrMediumE = self.MaxMatchHCALTrMediumE 
                alg.MaxMatchHCALTrLargeE = self.MaxMatchHCALTrLargeE
                
            elif t=='Charged':
               ## Track selector
                alg.TrackSelectorType = "DelegatingTrackSelector"
                alg.addTool( DelegatingTrackSelector, name="TrackSelector" )
                tracktypes = TrackCuts.keys()
                alg.TrackSelector.TrackTypes = self.TrackType
                for type in tracktypes : self.setupTypeTrackSelector( type, alg.TrackSelector,TrackCuts )
                
                ## Track related cuts
                alg.MinInfMomentumCut = self.MinInfMomentumCut
                alg.MaxChi2NoTT = self.MaxChi2NoTT
                alg.UseTTHits = self.UseTTHits 
                alg.MaxMatchECALTr = self.MaxMatchECALTr 
                alg.MaxMatchECALTr_T = self.MaxMatchECALTr_T
                
            elif t=='Pi0s':               
                pLocations.append('Phys/StdLooseResolvedPi0/Particles')
                pLocations.append('Phys/StdLooseMergedPi0/Particles')
                alg.MinPhotonIDMax4ResolvedPi0 = self.MinPhotonIDMax4ResolvedPi0 
                alg.MinPhotonIDMin4ResolvedPi0 = self.MinPhotonIDMin4ResolvedPi0 
                
            elif t=='V0s':
                pCompLocations.append("Phys/StdKs2PiPiLL/Particles" )
                pCompLocations.append("Phys/StdKs2PiPiDD/Particles" )
                pCompLocations.append("Phys/StdLambda2PPiLL/Particles" )
                pCompLocations.append("Phys/StdLambda2PPiDD/Particles" )

            elif t=='PFNeutrals':
                alg.NeutralRecovery = True
                alg.MinE = self.MinE
                alg.MaximumFracNeutrReco = self.MaximumFracNeutrReco
            else :
                print t,"are not supported!"
                exit(1)
                
            if len( self.CompositeParticleLocations)>0.5:
                for t in self.CompositeParticleLocations:
                    pCompLocations.append(t)
                
        alg.ParticleLocations = pLocations
        alg.CompositeParticleLocations = pCompLocations
        self.algorithms.append(alg)
                

    def setupHCAL(self):
       ## Create Sequencer
        ## Call the cluster creation
        hcalClus = CellularAutomatonAlg('HcalClusterization')
        hcalClus.OutputData = 'Rec/Calo/HcalClusters'
        hcalClus.addTool(CaloClusterizationTool,name="CaloClusterizationTool")
        hcalClus.CaloClusterizationTool.CellSelectorForEnergy = '2x2'
        self.algorithms.append(hcalClus)
        ## Get the covariance matrix
        clustCov = CaloClusterCovarianceAlg('HcalCov')
        clustCov.InputData =  'Rec/Calo/HcalClusters'
        clustCov.Resolution = 0.7
        clustCov.GainError = 0.1
        clustCov.Detector = '/dd/Structure/LHCb/DownstreamRegion/Hcal'
        clustCov.CovarianceName = 'HcalCovarTool'
        clustCov.SpreadName = 'HcalSpreadTool'
        clustCov.addTool(ClusterSpreadTool,name ='HcalSpreadTool' )
        clustCov.HcalSpreadTool.Detector= '/dd/Structure/LHCb/DownstreamRegion/Hcal'
        clustCov.addTool(ClusterCovarianceMatrixTool,name ='HcalCovarTool' )
        clustCov.HcalCovarTool.Detector= '/dd/Structure/LHCb/DownstreamRegion/Hcal'
        #clustCov.SubClusterType = "SubClusterSelector3x3"
        self.algorithms.append(clustCov)
        
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
        
        self.algorithms.append(hcal2Track)
                

    def setupHCALMC(self):
        caloDigitMC= CaloDigit2MCLinks2Table('CaloDigit2MCLinks2TableHCAL')
        caloDigitMC.Inputs+=['Raw/Hcal/Digits']
        caloDigitMC.Output = 'Relations/Raw/Hcal/Digits'
        self.algorithms.append(caloDigitMC)
        caloClusterMC= CaloClusterMCTruth('CaloClusterMCTruthHCAL')
        caloClusterMC.Clusters += ['Rec/Calo/HcalClusters']
        caloClusterMC.Input = 'Relations/Raw/Hcal/Digits'
        caloClusterMC.Output = 'Relations/Rec/Calo/HcalClusters' 
        self.algorithms.append(caloClusterMC)
        HCALPP2MC = NeutralPP2MC('NeutralPP2MCHCAL')
        HCALPP2MC.OutputTable = 'Relations/Rec/ProtoP/Neutrals'
        HCALPP2MC.MCCaloTable = 'Relations/Rec/Calo/HcalClusters'
        HCALPP2MC.InputData = ['Rec/ProtoP/PF','Rec/ProtoP/Neutrals']
        self.algorithms.append(HCALPP2MC)


        


# ----------------------------------------------------------------------------------

