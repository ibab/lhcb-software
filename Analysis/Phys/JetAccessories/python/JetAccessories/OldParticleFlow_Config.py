__version__ = "$Id: ParticleFlow_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, TrackSelector, DelegatingTrackSelector, ParticleFlow4Jets, CellularAutomatonAlg, CaloClusterizationTool,CaloClusterCovarianceAlg,ClusterSpreadTool,ClusterCovarianceMatrixTool, CaloPhotonMatch , PhotonMatchAlg , CaloClusterMCTruth,CaloDigit2MCLinks2Table,NeutralPP2MC , PVRelatorAlg, ChargedProtoParticleMaker)


#####################################
# Parameter that induce a loss:
# 
# Chi2MaxLong : kill long tracks with chi2Ndof > Chi2MaxLong
# Chi2MaxDown : kill downstream tracks with chi2Ndof > Chi2MaxDown
# Chi2MaxUp   : kill upstream tracks with chi2Ndof > Chi2MaxUp
# PtMinLong   : kill long tracks with pt < PtMinLong
# PtMinDown   : kill long tracks with pt < PtMinDown
# PtMinUp     : kill long tracks with pt < PtMinUp
# UseTTHits   : kill long tracks that should have a TT hit but don't
# MaxChi2NoTT : kill long tracks that have no TT expectation but a chi2Ndof < MaxChi2NoTT
# UseHCAL     : if set to false, does not use HCAL clusters
# MinHCALE    : minimum HCAL energy needed to be taken into account
# MaxMatchECALTr : kill ECAL cluster that match with a track with chi2 < MaxMatchECALTr (charged clusters)
# MaxMatchHCALTrSmallE and MaxMatchHCALLowEValue:
#     kill HCAL clusters with E < MaxMatchHCALLowEValue that match with a track with chi2 < MaxMatchHCALTrSmallE (charged clusters)
# MaxMatchHCALTrMediumE, MaxMatchHCALLowEValue and MaxMatchHCALHighEValue:
#     kill HCAL clusters with MaxMatchHCALLowEValue < E < MaxMatchHCALHighEValue  that match with a track with chi2 < MaxMatchHCALTrMediumE (charged clusters)
# MaxMatchHCALTrHighE and MaxMatchHCALHighEValue:
#     kill HCAL clusters with E > MaxMatchHCALHighEValue  that match with a track with chi2 < MaxMatchHCALTrHighE (charged clusters)
# MaxMatchHCALTr_T : kill HCAL clusters matching with a TTrack with chi2 < MaxMatchHCALTr_T (not implemented)
#
####################################
# Parameter that change particle classification (from PFParticles to PFBannedParticles):
#
# MinPhotonID4Photon : if photon ID < MinPhotonID4Photon, neutral ECAL goes to PFBannedParticles
# MaxMatchECALTr_T and MinPhotonID4PhotonTtrack :
#     if a ECAL cluster match a TTrack with chi2 < MaxMatchECALTr_T, neutral ECAL goes to PFBannedParticles
#     Nonetheless if photon ID > MinPhotonID4PhotonTtrack and CaloNeutralSpd > 1 it stays in PFParticles
# MinPhotonIDMax4ResolvedPi0 and MinPhotonIDMin4ResolvedPi0: for resolved pi0 min gammaID > MinPhotonIDMin4ResolvedPi0
#     and max gammaID > MinPhotonIDMax4ResolvedPi0 (a be neutral ie not matched to a track),
#     otherwise the photons are considered separatly and should fullfill the previous rules
# MinInfMomentumCut : if track has qOvp/err_qOvP < MinInfMomentumCut it is passed to PFBannedParticles
#                     but is still used to separate charged and neutral clusters (unless it does not pass the other cuts)
#
###################################
# Banned tag (stored in extrainfo 951) Relevant for Correction :
#
# if p->info(951,-10.)== -10. : NOT BANNED
# if p->info(951,-10.)== 0    : INFINITE MOMENTUM TRACK                   ---> to be corrected
# if p->info(951,-10.)== 2    : NEUTRAL ECAL that match TT and do not pass gammaID cut ---> to be corrected
# if p->info(951,-10.)== 6    : NEUTRAL ECAL that do not match TT and do not pass gammaID cut ---> to be corrected









class OldParticleFlowConf:
    def __init__(self, _name, _InputParticles = ['Photons','NeutralHadrons','Charged','Pi0s','V0s','PFNeutrals'], _MCSeq = False , _params = {} ):
         self.name = _name
         self.InputParticles = _InputParticles
         self.MC = _MCSeq
         ## Default parameter: the closer from what we used before + HCAL
         self.paramDef = {'Chi2MaxLong': 5. , 'PtMinLong': 0. , 'AcceptClone': False , 'PtMinDown': 0. , 'TrackType':['Downstream','Long','Upstream'],
                          'Chi2MaxDown': 10. , 'Chi2MaxUp': 10. , 'PtMinUp': 0. ,'Chi2MaxVelo': 10. ,'UseTTHits':False ,'MaxChi2NoTT': 5.,
                          'UseHCAL': True ,'MaxMatchECALTr': 4.,'MinHCALE': 1800.,'UsePIDInfo':True,
                          'MaxMatchHCALLowEValue': 5000.,'MaxMatchHCALHighEValue': 10000.,
                          'MaxMatchHCALTrSmallE': 25.,'MaxMatchHCALTrMediumE':16. ,'MaxMatchHCALTrLargeE':16. ,'MaxMatchHCALTr_T': 16.,
                          'MinPhotonID4PhotonTtrack': -2. , 'MinPhotonID4Photon': -1. ,'MaxMatchECALTr_T': 16. ,
                          'MinPhotonIDMax4ResolvedPi0':  -2.,'MinPhotonIDMin4ResolvedPi0':-4., 'MinInfMomentumCut':10. ,
                          'VerticesLocation':"Rec/Vertex/Primary",  "PFCaloHypoOutputLocation":"Rec/Calo/Hadrons",
                          'CandidateToBanLocation':'','PFProtoParticlesOutputLocation':"Rec/ProtoP/PF",
                          'PFOutputLocation': "Phys/PFParticles/Particles",'PFBannedOutputLocation':"Phys/PFBannedParticles/Particles",
                          'PFHiddenNeutralOutputLocation':"Phys/PFNeutralParticles/Particles",
                          "CalibECAL_EovP": 0.1,"CalibHCAL_EovP": 0.9,"NSigmaForCaloRecovery": 0.,
                          "MinECALE_NeutralRecovery": 1000.,"MinHCALE_NeutralRecovery": 2000.,'UseVelo':False
                          }
##          self.paramDef = {'Chi2MaxLong': 5. , 'PtMinLong': 0. , 'AcceptClone': False , 'PtMinDown': 0. ,
##                           'Chi2MaxDown': 7. , 'Chi2MaxUp': 7. , 'PtMinUp': 0. , 'UseTTHits':True ,'MaxChi2NoTT': 3.,
##                           'UseHCAL': True ,'MaxMatchECALTr': 4.,'MinHCALE': 1800.,
##                           'MaxMatchHCALLowEValue': 5000.,'MaxMatchHCALHighEValue': 10000.,
##                           'MaxMatchHCALTrSmallE': 16.,'MaxMatchHCALTrMediumE':9. ,'MaxMatchHCALTrLargeE':9. ,'MaxMatchHCALTr_T': 9.,
##                           'MinPhotonID4PhotonTtrack': -2. , 'MinPhotonID4Photon': -1. ,'MaxMatchECALTr_T': 16. ,
##                           'MinPhotonIDMax4ResolvedPi0':  -2.,'MinPhotonIDMin4ResolvedPi0':-4., 'MinInfMomentumCut':10. ,
##                           'VerticesLocation':"Rec/Vertex/Primary",  "PFCaloHypoOutputLocation":"Rec/Calo/Hadrons",
##                           'CandidateToBanLocation':'','PFProtoParticlesOutputLocation':"Rec/ProtoP/PF",
##                           'PFOutputLocation': "Phys/PFParticles/Particles",'PFBannedOutputLocation':"Phys/PFBannedParticles/Particles"
##                            }
         # set the datafile
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
                    print name,cut[0],cut[1]
                    ts.setProp("Min"+name,cut[0])
                    ts.setProp("Max"+name,cut[1])

    def setupParam(self,params):
        for key in self.paramDef.keys():
            if params.has_key(key): self.__dict__[key] = params[key]
            else: self.__dict__[key] = self.paramDef[key]
        if "Velo" in self.TrackType and not self.UseVelo: self.UseVelo=True


    
    ## Configure the jet maker
    def setupPF(self ):
        ## List of algorithms to put in the sequencer
        
        ## set the algorithm
        alg = ParticleFlow4Jets ( self.name )

        ## Definition of cuts to apply to input tracks for inputselection
        
        TrackCuts = {}
        print '###################################### ',self.TrackType
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
        
        alg.UseTTrackBanning       = True
        alg.CandidateToBanLocation = self.CandidateToBanLocation
        alg.VerticesLocation       = self.VerticesLocation
        alg.PFOutputLocation       = self.PFOutputLocation
        alg.PFBannedOutputLocation = self.PFBannedOutputLocation
        alg.PFHiddenNeutralOutputLocation = self.PFHiddenNeutralOutputLocation
        alg.PFProtoParticlesOutputLocation = self.PFProtoParticlesOutputLocation
        alg.PFCaloHypoOutputLocation = self.PFCaloHypoOutputLocation
        
        for t in self.InputParticles:
            if t=='Photons':
                pLocations.append('Phys/StdLooseAllPhotons/Particles')
                alg.MinPhotonID4Photon = self.MinPhotonID4Photon
                alg.MinPhotonID4PhotonTtrack = self.MinPhotonID4PhotonTtrack
        
            if t=='NeutralHadrons':
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
                alg.MaxMatchHCALTr_T = self.MaxMatchHCALTr_T 
                
            if t=='Charged':
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
                alg.UsePIDInfo = self.UsePIDInfo
                
            if t=='Pi0s':               
                pLocations.append('Phys/StdLooseResolvedPi0/Particles')
                pLocations.append('Phys/StdLooseMergedPi0/Particles')
                alg.MinPhotonIDMax4ResolvedPi0 = self.MinPhotonIDMax4ResolvedPi0 
                alg.MinPhotonIDMin4ResolvedPi0 = self.MinPhotonIDMin4ResolvedPi0 
                
            if t=='V0s':
                pLocations.append('Phys/StdLooseKsDD/Particles')
                pLocations.append('Phys/StdLooseKsLL/Particles')
                pLocations.append('Phys/StdLooseLambdaDD/Particles')
                pLocations.append('Phys/StdLooseLambdaLL/Particles')

            if t=='PFNeutrals':
                alg.NeutralRecovery = True
                alg.UsePIDInfo = True
                alg.CalibECAL_EovP = self.CalibECAL_EovP
                alg.CalibHCAL_EovP = self.CalibHCAL_EovP
                alg.NSigmaForCaloRecovery = self.NSigmaForCaloRecovery
                
        alg.ParticleLocations = pLocations
        #alg.OutputLevel = 0
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

